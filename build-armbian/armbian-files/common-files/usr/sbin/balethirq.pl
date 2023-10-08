#!/usr/bin/perl
# Copyright (C) 2021- https://github.com/unifreq

use strict;
our %irq_map;
our %cpu_map;
our %min_cpu_map;
our %uniq_eth_cpu_map;
our $all_cpu_mask = 0;

# rpscpu掩码是否要排除eth0占用的核(0:否 1:是)
our $rpscpu_exclude_eth0_core=1;
# rpscpu掩码是否要排除eth1占用的核(0:否 1:是)
our $rpscpu_exclude_eth1_core=1;

our $usb_as_eth1 = 0;

&read_config();
&read_irq_data();
&update_smp_affinity();
&enable_eth_rps_rfs();
&board_special_config();
exit(0);

############################## sub functions #########################
# 读取 /etc/balance_irq 或 /etc/config/balance_irq 配置文件
sub read_config {
    my $cpu_count = &get_cpu_count();
    my $fh;
    my $config_file;
    if(-f "/etc/balance_irq") {
        $config_file = "/etc/balance_irq";
    } elsif(-f "/etc/config/balance_irq") {
        $config_file = "/etc/config/balance_irq";
    } else {
	exit(0);
    }

    open $fh, "<", $config_file or die $!;
    while(<$fh>) {
        chomp;
	# 跳过注释行
	next if(/^#/);
	# 跳过空行
	next if(/^\s*$/);
        my($name, $value) = split;
        my @cpus = split(',', $value);
        # ARMV8 当前最多CPU核数是8核
        my $min_cpu = 9;

        foreach my $cpu (@cpus) {
            if($cpu > $cpu_count) {
                $cpu = $cpu_count;
            } elsif($cpu < 1) {
                $cpu = 1;
            }
            if($min_cpu > $cpu) {
                $min_cpu = $cpu;
            }
        } # foreach

        $cpu_map{$name} = \@cpus;
        $min_cpu_map{$name} = $min_cpu;
    } # while
    close $fh;
}

# 计算cpu核数
sub get_cpu_count {
    my $fh;
    open $fh, "<", "/proc/cpuinfo" or die $!;
    my $count=0;
    while(<$fh>) {
        chomp;
        my @ary = split;
        if($ary[0] eq "processor") {
            $count++;
            $all_cpu_mask += 1<<($count-1);
        }
    }
    close $fh;
    return $count;
}

sub read_irq_data {
    my $fh;
    open $fh, "<", "/proc/interrupts" or die $!;
    my %local_map;
    while(<$fh>) {
        chomp;
        my @raw = split;
        my $irq = $raw[0];
        $irq =~ s/://;
        my $name = $raw[-1];
	if(exists $local_map{$name}) {
	    # r68s 有 2条 eth0 和 2条 eth1，只保留第1条
	    next;
	} else {
	    $local_map{$name} = 1;
	}

        if(exists $cpu_map{$name}) {
            $irq_map{$name} = $irq;
            if($name =~ m/\Axhci-hcd:usb[1-9]\Z/) {
		if (not (exists $cpu_map{eth1} || exists $cpu_map{'eth1-0'}) ) {
                    # 对于单网口的设备，USB外接网卡视为eth1
		    $usb_as_eth1 = 1;
                    $uniq_eth_cpu_map{eth1} =  1 << ($min_cpu_map{$name} - 1);
	        } else {
                    $uniq_eth_cpu_map{$name} =  1 << ($min_cpu_map{$name} - 1);
		}
            } else {
                $uniq_eth_cpu_map{$name} = 1 << ($min_cpu_map{$name} - 1);
            }
        }
    }
    close $fh;
}

sub update_smp_affinity {
    for my $key (sort keys %irq_map) {
        my $fh;
        my $irq = $irq_map{$key};
        my $cpus_ref = $cpu_map{$key};
        my $mask = 0;
        foreach my $cpu (@$cpus_ref) {
            $mask += 1 << ($cpu-1);
        }
        my $smp_affinity = sprintf("%0x", $mask);
        open $fh, ">", "/proc/irq/$irq/smp_affinity" or die $!;
        print "irq name:$key, irq:$irq, affinity: $smp_affinity\n";
        print $fh "$smp_affinity\n";
        close $fh;
    }
}

sub tunning_eth_ring {
    my ($eth, $target_rx_ring, $target_tx_ring) = @_;
    my $buf = `/usr/sbin/ethtool -g ${eth} 2>/dev/null`;
    if($buf) {
        $buf =~ s/\r|\n/\t/g;
        if( $buf =~ m/.+?Pre-set maximums:\s+RX:\s+(\d+|n\/a).+?TX:\s+(\d+|n\/a).+?Current hardware settings:\s+RX:\s+(\d+|n\/a).+?TX:\s+(\d+|n\/a)/) {
            my $max_rx_ring  = $1;
            my $max_tx_ring  = $2;
            my $cur_rx_ring  = $3;
            my $cur_tx_ring  = $4;

	    $max_rx_ring = 0 if ($max_rx_ring eq 'n/a');
	    $max_tx_ring = 0 if ($max_tx_ring eq 'n/a');
	    $cur_rx_ring = 0 if ($cur_rx_ring eq 'n/a');
	    $cur_tx_ring = 0 if ($cur_tx_ring eq 'n/a');

            if( ($max_rx_ring > 0) && ($target_rx_ring>0) && ($max_rx_ring > $target_rx_ring) && ($cur_rx_ring != $target_rx_ring) ) {
                system "/usr/sbin/ethtool -G ${eth} rx ${target_rx_ring} >/dev/null 2>&1";
                print "Set the rx ring of ${eth} to ${target_rx_ring}\n";
            }
            if( ($max_tx_ring > 0) && ($target_tx_ring>0) && ($max_tx_ring > $target_tx_ring) && ($cur_tx_ring != $target_tx_ring) ) {
                system "/usr/sbin/ethtool -G ${eth} tx ${target_tx_ring} >/dev/null 2>&1";
                print "Set the tx ring of ${eth} to ${target_tx_ring}\n";
            }
        }
    }
}

sub enable_eth_rps_rfs {
    my $rps_sock_flow_entries = 0;
    for my $eth ("eth0","eth1","eth2","eth3","eth4","eth5","eth6") {
	# rps优化只针对单队列网卡，
	# 如果存在 rx-1,则表示该网卡支持 rss 多队列，不需要优化
        if((-d "/sys/class/net/${eth}/queues/rx-0") && (! -d "/sys/class/net/${eth}/queues/rx-1")) {
            my $value = 32768;
            $rps_sock_flow_entries += $value;
            my $eth_cpu_mask_hex;
	    my $cpu_mask = $all_cpu_mask;
            if($rpscpu_exclude_eth0_core == 1) {
	        $cpu_mask -= $uniq_eth_cpu_map{eth0};
	    }
            if($rpscpu_exclude_eth1_core == 1) {
	        $cpu_mask -= $uniq_eth_cpu_map{eth1};
	    }

            $eth_cpu_mask_hex = sprintf("%0x", $cpu_mask);
            print "Set the rps cpu mask of $eth to 0x$eth_cpu_mask_hex\n";
            open my $fh, ">", "/sys/class/net/${eth}/queues/rx-0/rps_cpus" or die;
            print $fh $eth_cpu_mask_hex;
            close $fh;

            open $fh, ">", "/sys/class/net/${eth}/queues/rx-0/rps_flow_cnt" or die;
            print $fh $value;
            close $fh;

            open my $fh, ">", "/sys/class/net/${eth}/queues/tx-0/xps_cpus" or die;
            print $fh $eth_cpu_mask_hex;
            close $fh;

	    if( ($eth eq "eth1") && ($usb_as_eth1 == 1) ) {
                # USB外接网卡：eth1(RTL8153)，经实测最佳的rx_ring在 100-500范围, 默认值是100，超过500之后， 多CPU负载会失衡
	        &tunning_eth_ring($eth, 192, 0);
            }
        }
    }
    open my $fh, ">", "/proc/sys/net/core/rps_sock_flow_entries" or die;
    print $fh $rps_sock_flow_entries;
    close $fh;
}

sub get_boardinfo() {
    my $ret="unknown";
    if(-f "/proc/device-tree/model") {
         open my $fh, "<", "/proc/device-tree/model" or warn $!;
	 read $fh, $ret, 100;
	 close $fh;
	 $ret =~ s/\0//;
    }
    return $ret;
}

sub board_special_config() {
    my $board = &get_boardinfo();
    if($board eq "FastRhino R66S") {
        &optimize_eth_parameters("eth0","eth1");
    } elsif($board eq "FastRhino R68S") {
        &optimize_eth_parameters("eth0","eth1","eth2","eth3");
    } elsif($board eq "Radxa E25") {
        &optimize_eth_parameters("eth0","eth1");
    } elsif($board eq "FriendlyElec NanoPi R5") {
        &optimize_eth_parameters("eth0","eth1","eth2");
    } elsif($board eq "Radxa ROCK 5B") {
        &optimize_eth_parameters("eth0","eth1");
    } elsif($board eq "Beelink IPC-R Board") {
        &optimize_eth_parameters("eth0","eth1","eth2","eth3","eth4","eth5");
    }
}

sub optimize_eth_parameters {
    while (my $eth = shift) {
        print "optimizing $eth ... ";
        system "ethtool -K $eth scatter-gather on >/dev/null 2>&1";
        system "ethtool -K $eth tcp-segmentation-offload on >/dev/null 2>&1";
        system "ethtool -K $eth rx-udp-gro-forwarding on >/dev/null 2>&1";
        print "done\n";
    }
}
