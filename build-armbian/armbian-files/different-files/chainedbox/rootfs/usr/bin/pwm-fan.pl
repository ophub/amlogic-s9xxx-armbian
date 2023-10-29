#!/usr/bin/perl

use strict;

################################################################################
# 参数调整区
# 速度最小值(满速是99),如果小于20可能进入死区，风扇不转
my $speed_min = 33;

# 速度最大值(满速是99)
my $speed_max = 99;

# 温度低限(摄氏度): 小于等于此值按最低速率转动
my $temp_low = 40;

# 温度最高限(摄氏度): 大于此值按最高速率转动
my $temp_high = 75;

# 调速间隔(秒)
my $interval = 10;
################################################################################

my $fixed_speed = $ARGV[0];
my $period = 25000;
&init;
if( ($fixed_speed ne "") && 
    ($fixed_speed =~ m/^[0-9]{1,3}$/) ) {
    # 如果命令行参数为 0-100 的整数,则按指定的固定速率调速
    &set_fixed_speed($fixed_speed);
} else { 
    # 否则自动调速
    while(1) {
        &auto_speed;
        sleep($interval);    
    }
}
exit 0;


###############################################################################
sub get_soc_temp {
    my @fnames=(
          '/sys/devices/virtual/thermal/thermal_zone0/temp',
          '/sys/devices/platform/scpi/scpi:sensors/hwmon/hwmon0/temp1_input',
       );
    my $fh;
    my $temp = 50;
    for my $fname (@fnames) {
        if( -f $fname ) {
            open $fh, "<", $fname;
            $temp = <$fh> / 1000.0;
            close $fh;
            return $temp;
        }
    }
    return $temp;
}

sub init {

    $speed_max = 100  if ($speed_max > 100);
    $speed_min = 0  if($speed_min < 0);

    my $fh;
    `rmmod pwm_fan 2>/dev/null`;

    open $fh, ">", "/sys/class/pwm/pwmchip0/export";
    print $fh "0\n";
    close $fh;

    open $fh, ">", "/sys/class/pwm/pwmchip0/pwm0/period";
    print $fh "$period\n";
    close $fh;

    open $fh, ">", "/sys/class/pwm/pwmchip0/pwm0/polarity";
    print $fh "normal\n";
    close $fh;

    open $fh, ">", "/sys/class/pwm/pwmchip0/pwm0/enable";
    print $fh "1\n";
    close $fh;
}

sub set_fixed_speed {
    my $fixed_speed = shift;

    # 最大值0.99
    my $coeff_speed = ($fixed_speed / 100.0) > 1 ?  0.99 : ($fixed_speed / 100.0);

    #0.1-0.19 定义为死区
    $coeff_speed = 0.20 if $coeff_speed > 0 and $coeff_speed < 0.2;

    my $duty_cycle = int($coeff_speed * $period);
    
    open my $fh, ">", "/sys/class/pwm/pwmchip0/pwm0/duty_cycle";
    print $fh "$duty_cycle\n";
    close $fh;
}

sub auto_speed {
    my $temp = &get_soc_temp;
    my $coeff_temp;
    if($temp <= $temp_low) {
	$coeff_temp = 0;
    } elsif($temp > $temp_high) {
        $coeff_temp = 1;
    } else {
        $coeff_temp = ($temp - $temp_low) / ($temp_high - $temp_low);
    }

    my $coeff_speed_min = $speed_min / $speed_max  * ($speed_max / 100);
    my $coeff_speed = $coeff_temp; 
    $coeff_speed = ($coeff_speed_min + $coeff_speed) > 0.99 ? 0.99 : ($coeff_speed_min + $coeff_speed);
   
    my $duty_cycle = int($coeff_speed * $period);
    
    open my $fh, ">", "/sys/class/pwm/pwmchip0/pwm0/duty_cycle";
    print $fh "$duty_cycle\n";
    close $fh;
}
