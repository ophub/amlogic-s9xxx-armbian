# The files description

The list of devices supported by Amlogic TV Boxes, the configuration file in the `Armbian` system is [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf), and the configuration file in the `OpenWrt` system is [/etc/model_database.txt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/amlogic-s9xxx/common-files/rootfs/etc/model_database.txt).

支持的 Amlogic 电视盒子列表，在 `Armbian` 系统中配置文件的位置为 [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf)，在 `OpenWrt` 系统中配置文件的位置为 [/etc/model_database.txt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/amlogic-s9xxx/common-files/rootfs/etc/model_database.txt)。

## The list of devices supported by Amlogic TV Boxes

<table cellpadding="0" cellspacing="0">
<tr><td>ID</td><td>Model Name</td><td>SOC</td><td>FDTFILE</td><td>UBOOT_OVERLOAD</td><td>MAINLINE_UBOOT</td><td>BOOTLOADER_IMG</td><td>Brief Description</td></tr>
<tr><td>101</td><td>Phicomm N1</td><td>s905d</td><td>meson-gxl-s905d-phicomm-n1.dtb</td><td>u-boot-n1.bin</td><td>NA</td><td>u-boot-2015-phicomm-n1.bin</td><td>4C@1512Mhz,2GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>102</td><td>Phicomm N1(DMA thresh)</td><td>s905d</td><td>meson-gxl-s905d-phicomm-n1-thresh.dtb</td><td>u-boot-n1.bin</td><td>NA</td><td>u-boot-2015-phicomm-n1.bin</td><td>Same as above, when ethmac flow control is off</td></tr>
<tr><td>103</td><td>HG680P</td><td>s905x</td><td>meson-gxl-s905x-p212.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic</td></tr>
<tr><td>104</td><td>TX3-Mini</td><td>s905w</td><td>meson-gxl-s905w-tx3-mini.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic</td></tr>
<tr><td>105</td><td>MECOOL KI Pro</td><td>s905d</td><td>meson-gxl-s905d-mecool-ki-pro.dtb</td><td>u-boot-p201.bin</td><td>NA</td><td>NA</td><td>2G/16G,1Gb Nic</td></tr>
<tr><td>106</td><td>T95</td><td>s905x</td><td>meson-gxl-s905x-p212.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>1+8G,100Mb Nic(Use: extlinux/extlinux.conf)</td></tr>
<tr><td>107</td><td>B860H</td><td>s905x</td><td>meson-gxl-s905x-b860h.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic</td></tr>
<tr><td>108</td><td>TBee Box</td><td>s905x</td><td>meson-gxl-s905x-tbee.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic,qca9377 WLAN/Bluetooth</td></tr>
<tr><td>109</td><td>W95</td><td>s905w</td><td>meson-gxl-s905w-p281.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,2GB RAM,16GB ROM,100Mb Nic,USB2.0</td></tr>
<tr><td>110</td><td>X96-Mini</td><td>s905w</td><td>meson-gxl-s905w-x96-mini.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic</td></tr>
<tr><td>111</td><td>TX9</td><td>s905x</td><td>meson-gxl-s905x-tx9.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz,100Mb Nic,rtl8723cs WLAN,rtl8723bs Bluetooth</td></tr>
<tr><td>112</td><td>M302A/M304A</td><td>s905l3b</td><td>meson-gxl-s905l3b-m302a.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L3-b,2+8G,USB2.0x2,LAN 100Mb Nic</td></tr>
<tr><td>113</td><td>MGV2000/MGV3000</td><td>s905l2</td><td>meson-gxl-s905l2-x7-5g.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L2,2+8G,USB2.0x2,LAN 100Mb Nic,2.4/5GHz Wi-Fi + Bluetooth(Cdtech 47822BS/Realtek 8822BS,no work)</td></tr>
<tr><td>114</td><td>Wojia-TV-IPBS9505</td><td>s905l2</td><td>meson-gxl-s905l2-ipbs9505.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L2,2+8G,USB2.0x2,LAN 100Mb Nic</td></tr>
<tr><td>115</td><td>Q96 mini</td><td>s905l-b</td><td>meson-gxl-s905x-p212.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>1+8G,100Mb Nic</td></tr>
<tr><td>116</td><td>CM311-1</td><td>s905l3</td><td>meson-gxl-s905l2-x7-5g.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L2,2+8G,USB2.0x2,LAN 100Mb Nic</td></tr>

<tr><td>201</td><td>Octopus Planet</td><td>s912</td><td>meson-gxm-octopus-planet.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz+4C@1000Mhz,2GB Mem,1Gb Nic</td></tr>
<tr><td>202</td><td>Octopus Planet (FAKE)</td><td>s912</td><td>meson-gxm-octopus-planet.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>bl-fake-octopus-planet.bin</td><td>4C@1512Mhz+4C@1000Mhz,2GB Mem,1Gb Nic</td></tr>
<tr><td>203</td><td>H96 Pro Plus</td><td>s912</td><td>meson-gxm-octopus-planet.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz+4C@1000Mhz,2GB Mem,1Gb Nic</td></tr>
<tr><td>204</td><td>Tanix-TX92</td><td>s912</td><td>meson-gxm-octopus-planet.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>NA</td><td>3GB DDR4 32GB eMMC,1.5GHz,5G WIFI,1Gb Nic</td></tr>
<tr><td>205</td><td>VORKE-Z6-Plus</td><td>s912</td><td>meson-gxm-octopus-planet.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>NA</td><td>3GB DDR3 32GB eMMC5.0,1.5Ghz,TF CARD Support 1~32GB,1Gb Nic</td></tr>
<tr><td>206</td><td>MECOOL-M8S-Pro-L</td><td>s912</td><td>meson-gxm-q201.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>2G RAM,3G RAM,16G ROM,32G ROM,100Mb Nic</td></tr>
<tr><td>207</td><td>T95Z Plus</td><td>s912</td><td>meson-gxm-t95z-plus.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>3G+32G,Octa-Core,2.4/5.8G Dual-Band WiFi,1Gb Nic 64Bit BT4.0(Use: extlinux/extlinux.conf)</td></tr>
<tr><td>208</td><td>TX9 Pro(3G+32g+1Gb)</td><td>s912</td><td>meson-gxm-tx9-pro.dtb</td><td>u-boot-zyxq.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz+4C@1000Mhz,3G RAM,32G ROM,Bluetooth 4.1,1Gb Nic,brcm43455 wifi</td></tr>
<tr><td>209</td><td>TX9 Pro(2G+16g+100Mb)</td><td>s912</td><td>meson-gxm-q201.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>4C@1512Mhz+4C@1000Mhz,2G RAM,16G ROM,Bluetooth 4.1,100Mb Nic,RTL8189ETV wifi</td></tr>
<tr><td>210</td><td>Nexbox A1 & A95X</td><td>s912</td><td>meson-gxm-nexbox-a1.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>2G DDR3 RAM 16G eMMC,1Gb Nic,qca9377 wifi(no work)</td></tr>
<tr><td>211</td><td>Nexbox A95X A2</td><td>s912</td><td>meson-gxm-nexbox-a2.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>2GB RAM,16GB ROM,2.4G/5.0G WiFi,qca9377 WLAN/Bluetooth 4.0,1Gb Nic</td></tr>
<tr><td>212</td><td>Tanix TX8 MAX</td><td>s912</td><td>meson-gxm-tx8-max.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>3GB RAM,16GB/32GB eMMC,2.4G/5.0G WiFi,qca9377 WLAN/Bluetooth 4.1,1Gb Nic</td></tr>
<tr><td>213</td><td>Vontar X92</td><td>s912</td><td>meson-gxm-x92.dtb</td><td>u-boot-p212.bin</td><td>NA</td><td>NA</td><td>3GB RAM,16GB/32GB eMMC,2.4G/5.0G WiFi,AP6255 WLAN/Bluetooth 4.0,1Gb Nic</td></tr>
<tr><td>214</td><td>Phicomm-T1</td><td>s912</td><td>meson-gxm-phicomm-t1.dtb</td><td>u-boot-s905x-s912.bin</td><td>NA</td><td>NA</td><td>2G RAM,16G ROM,100Mb Nic,Wifi,Bluetooth</td></tr>

<tr><td>301</td><td>X96 Max 4GB</td><td>s905x2</td><td>meson-g12a-x96-max.dtb</td><td>u-boot-x96max.bin</td><td>x96max-u-boot.bin.sd.bin</td><td>NA</td><td>4C@1908Mhz,4GB Mem,1Gb Nic</td></tr>
<tr><td>302</td><td>X96-Max-2GB/A95X-F2</td><td>s905x2</td><td>meson-g12a-x96-max-rmii.dtb</td><td>u-boot-x96max.bin</td><td>x96max-u-boot.bin.sd.bin</td><td>NA</td><td>4C@1908Mhz,2GB Mem,100Mb Nic</td></tr>
<tr><td>303</td><td>MECOOL KM3 4G</td><td>s905x2</td><td>meson-g12a-sei510.dtb</td><td>u-boot-x96max.bin</td><td>x96max-u-boot.bin.sd.bin</td><td>NA</td><td>4C@1908Mhz,4+64G/128G,2.4G/5G WiFi,Bluetooth 4.1,100Mb Nic</td></tr>
<tr><td>304</td><td>E900V22C/D</td><td>s905l3a</td><td>meson-g12a-s905l3a-e900v22c.dtb</td><td>u-boot-e900v22c.bin</td><td>e900v22c-u-boot.bin.sd.bin</td><td>NA</td><td>4C@1908Mhz,S905L3A+B,2+8G,USB2.0x2,LAN 100Mb Nic,uwe5621ds wifi(no work)</td></tr>
<tr><td>305</td><td>CM311-1a-YST</td><td>s905l3a</td><td>meson-g12a-s905l3a-cm311.dtb</td><td>u-boot-e900v22c.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L3A,2+16G,USB2.0x2,LAN 100Mb Nic,Bluetooth rtl8761b</td></tr>
<tr><td>306</td><td>M401A/ZTE-B863AV3.2-M</td><td>s905l3a</td><td>meson-g12a-s905l3a-m401a.dtb</td><td>u-boot-e900v22c.bin</td><td>NA</td><td>NA</td><td>4C@1908Mhz,S905L3A,2+16G,USB2.0x2,LAN 100Mb Nic</td></tr>

<tr><td>401</td><td>Beelink GT-King</td><td>s922x</td><td>meson-g12b-gtking.dtb</td><td>u-boot-gtking.bin</td><td>gtking-u-boot.bin.sd.bin</td><td>NA</td><td>2C@1800Mhz(A53)+4C@1908Mhz(A73),4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>402</td><td>Beelink GT-King Pro</td><td>s922x</td><td>meson-g12b-gtking-pro.dtb</td><td>u-boot-gtkingpro.bin</td><td>gtkingpro-u-boot.bin.sd.bin</td><td>NA</td><td>2C@1800Mhz(A53)+4C@1908Mhz(A73),4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>403</td><td>Beelink GT-King Pro H</td><td>s922x</td><td>meson-g12b-gtking-pro-h.dtb</td><td>u-boot-gtkingpro.bin</td><td>gtkingpro-u-boot.bin.sd.bin</td><td>NA</td><td>S922X-H,2C@1800Mhz(A53)+4C@2208Mhz(A73),4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>404</td><td>Beelink GT-King Pro Rev_A</td><td>s922x</td><td>meson-g12b-gtking-pro.dtb</td><td>u-boot-gtkingpro-rev-a.bin</td><td></td><td>NA</td><td>2C@1800Mhz(A53)+4C@1908Mhz(A73),4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>405</td><td>Hardkernel ODroid N2</td><td>s922x</td><td>meson-g12b-odroid-n2.dtb</td><td>u-boot-gtkingpro.bin</td><td>odroid-n2-u-boot.bin.sd.bin</td><td>NA</td><td>2C@1800Mhz(A53)+4C@1908Mhz(A73),4GB Mem,1Gb Nic</td></tr>
<tr><td>406</td><td>UGOOS AM6 Plus</td><td>s922x</td><td>meson-g12b-ugoos-am6.dtb</td><td>u-boot-gtkingpro.bin</td><td>gtkingpro-u-boot.bin.sd.bin</td><td>NA</td><td>2C@1800Mhz(A53)+4C@1908Mhz(A73),4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>407</td><td>Khadas VIM3</td><td>a311d</td><td>meson-g12b-a311d-khadas-vim3.dtb</td><td>u-boot-gtkingpro.bin</td><td>khadas-vim3-u-boot.sd.bin</td><td>NA</td><td>4C@2.2Ghz+2C@1.8Ghz,PCIe+USB 3.0,1Gb Nic,Wifi</td></tr>

<tr><td>501</td><td>X96 Max+ 100Mb</td><td>s905x3</td><td>meson-sm1-x96-max-plus-100m.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>hk1box-bootloader.img</td><td>4C@2100Mhz,4GB Mem,1Gb Nic,AM7256 wifi</td></tr>
<tr><td>502</td><td>X96 Max+ 1Gb</td><td>s905x3</td><td>meson-sm1-x96-max-plus.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>hk1box-bootloader.img</td><td>4C@2100Mhz,4GB Mem,1Gb Nic,AM7256 wifi</td></tr>
<tr><td>503</td><td>X96 Max+ (OverClock)</td><td>s905x3</td><td>meson-sm1-x96-max-plus-oc.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>hk1box-bootloader.img</td><td>4C@2208Mhz,4GB Mem,1Gb Nic,AM7256 wifi</td></tr>
<tr><td>504</td><td>X96 Max+ (IP1001M phy)</td><td>s905x3</td><td>meson-sm1-x96-max-plus-ip1001m.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>hk1box-bootloader.img</td><td>4C@2208Mhz,4GB Mem,1Gb Nic (IP1001M phy),brcm4354 wifi</td></tr>
<tr><td>505</td><td>X96 Max+ Q2</td><td>s905x3</td><td>meson-sm1-x96-max-plus-q2.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>hk1box-bootloader.img</td><td>4C@2208Mhz,4GB Mem,1Gb Nic,qca9377 wifi and Bluetooth</td></tr>
<tr><td>506</td><td>HK1 Box</td><td>s905x3</td><td>meson-sm1-hk1box-vontar-x3.dtb</td><td>u-boot-x96maxplus.bin</td><td>hk1box-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>507</td><td>HK1 Box (OverClock)</td><td>s905x3</td><td>meson-sm1-hk1box-vontar-x3-oc.dtb</td><td>u-boot-x96maxplus.bin</td><td>hk1box-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2208Mhz,4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>508</td><td>H96 Max X3</td><td>s905x3</td><td>meson-sm1-h96-max-x3.dtb</td><td>u-boot-x96maxplus.bin</td><td>h96maxx3-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>509</td><td>H96 Max X3 (OverClock)</td><td>s905x3</td><td>meson-sm1-h96-max-x3-oc.dtb</td><td>u-boot-x96maxplus.bin</td><td>h96maxx3-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2208Mhz,4GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>510</td><td>Ugoos X3</td><td>s905x3</td><td>meson-sm1-ugoos-x3.dtb</td><td>u-boot-ugoos-x3.bin</td><td>ugoos-x3-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,2(Cube)/4(Pro,Plus)GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>511</td><td>Ugoos X3 (OverClock)</td><td>s905x3</td><td>meson-sm1-ugoos-x3.dtb</td><td>u-boot-ugoos-x3.bin</td><td>ugoos-x3-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2208Mhz,2(Cube)/4(Pro,Plus)GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>512</td><td>TX3 1Gb</td><td>s905x3</td><td>meson-sm1-tx3-qz.dtb</td><td>u-boot-tx3-qz.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,4GB Mem,1Gb Nic,bcm4330 wifi</td></tr>
<tr><td>513</td><td>TX3 1Gb (OverClock)</td><td>s905x3</td><td>meson-sm1-tx3-qz-oc.dtb</td><td>u-boot-tx3-qz.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2208Mhz,4GB Mem,1Gb Nic,bcm4330 wifi</td></tr>
<tr><td>514</td><td>TX3 100Mb</td><td>s905x3</td><td>meson-sm1-tx3-bz.dtb</td><td>u-boot-tx3-bz.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,4GB Mem,100Mb Nic,bcm4330 wifi</td></tr>
<tr><td>515</td><td>TX3 100Mb (OverClock)</td><td>s905x3</td><td>meson-sm1-tx3-bz-oc.dtb</td><td>u-boot-tx3-bz.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2208Mhz,4GB Mem,100Mb Nic,bcm4330 wifi</td></tr>
<tr><td>516</td><td>X96 Air 1Gb</td><td>s905x3</td><td>meson-sm1-x96-air-gbit.dtb</td><td>u-boot-x96maxplus.bin</td><td>NA</td><td>NA</td><td>4C@2100Mhz,2GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>517</td><td>X96 Air & X96 Max+100W & Other 100Mb</td><td>s905x3</td><td>meson-sm1-x96-air.dtb</td><td>u-boot-x96maxplus.bin</td><td>NA</td><td>NA</td><td>4C@2100Mhz,2GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>518</td><td>X96 Air Q1000</td><td>s905x3</td><td>meson-sm1-x96-max-plus-q2.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,2GB Mem,1Gb Nic,qca9377 wifi and Bluetooth</td></tr>
<tr><td>519</td><td>A95XF3 Air 1Gb</td><td>s905x3</td><td>meson-sm1-a95xf3-air-gbit.dtb</td><td>u-boot-x96maxplus.bin</td><td>NA</td><td>NA</td><td>4C@2100Mhz,2GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>520</td><td>A95XF3 Air 100Mb</td><td>s905x3</td><td>s905x3:meson-sm1-a95xf3-air.dtb</td><td>u-boot-x96maxplus.bin</td><td>NA</td><td>NA</td><td>4C@2100Mhz,2GB Mem,1Gb Nic,Wifi</td></tr>
<tr><td>521</td><td>Tencent Aurora 3Pro</td><td>s905x3-b</td><td>meson-sm1-skyworth-lb2004-a4091.dtb</td><td>u-boot-skyworth-lb2004.bin</td><td>skyworth-lb2004-u-boot.bin.sd.bin</td><td>skyworth-lb2004-bootloader.img</td><td>4C@2100Mhz,4GB Mem,32G Rom,Model LB2004-A4091,MT7661RSN chip Wifi-5(mt7663s)/Bluetooth-5.0(btmtksdio),JL2121 1Gb Nic</td></tr>
<tr><td>522</td><td>X96 Max+ A100</td><td>s905x3</td><td>meson-sm1-sei610.dtb</td><td>u-boot-x96maxplus.bin</td><td>x96maxplus-u-boot.bin.sd.bin</td><td>NA</td><td>4C@2100Mhz,4GB Mem,32G Rom,Wifi AM7256, 100Mb Nic</td></tr>
<tr><td>523</td><td>X96 Max+ Q1</td><td>s905x3</td><td>meson-sm1-x96-max-plus-q1.dtb</td><td>u-boot-x96maxplus.bin</td><td>NA</td><td>NA</td><td>4C@2100Mhz,4GB Mem,32G Rom,Wifi work,100Mb Nic</td></tr>

<tr><td>a01</td><td>Beelink Mini MX 2G</td><td>s905</td><td>meson-gxbb-beelink-mini-mx.dtb</td><td>u-boot-s905.bin</td><td>NA</td><td>NA</td><td>Mali-450 GPU 2.4GHz / 5.8GHz WiFi 2GB RAM 16GB ROM,1Gb Nic</td></tr>
<tr><td>a02</td><td>Sunvell-T95M</td><td>s905</td><td>meson-gxbb-p201.dtb</td><td>u-boot-s905.bin</td><td>NA</td><td>NA</td><td>2GB DDR3 + 8G eMMC, LAN 100Mb Nic, Realtek 8189ETV wireless (no drive)</td></tr>
<tr><td>a03</td><td>MXQ PRO+ 4K</td><td>s905</td><td>meson-gxbb-mxq-pro-plus.dtb</td><td>u-boot-p201.bin</td><td>NA</td><td>NA</td><td>2GB RAM 16GB ROM 2.4G/5.8G,1Gb Nic</td></tr>
</table>
