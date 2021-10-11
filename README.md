# Armbian for Amlogic S9xxx STB

View Chinese description  |  [查看中文说明](README.cn.md)

Compile the Armbian for Amlogic S9xxx STB. including install to EMMC and update related functions. Support Amlogic S9xxx STB are ***`s922x, s905x3, s905x2, s912, s905d, s905x, s905w`***, etc. such as ***`Belink GT-King, Belink GT-King Pro, UGOOS AM6 Plus, X96-Max+, HK1-Box, H96-Max-X3, Phicomm-N1, Octopus-Planet, Fiberhome HG680P, ZTE B860H`***, etc.

The latest version of the Armbian firmware can be downloaded in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Welcome to `Fork` and personalize it. If it is useful to you, you can click on the `Star` in the upper right corner of the warehouse to show your support.

## Armbian Firmware instructions

| Model  | STB | [Optional kernel](https://github.com/ophub/flippy-kernel/tree/main/library) | Armbian Firmware |
| ---- | ---- | ---- | ---- |
| s922x | [Belink](https://tokopedia.link/RAgZmOM41db), [Belink-Pro](https://tokopedia.link/sfTHlfS41db), [Ugoos-AM6-Plus](https://tokopedia.link/pHGKXuV41db) | All | armbian_aml_s922x_buster_*.img |
| s905x3 | [X96-Max+](https://tokopedia.link/uMaH09s41db), [HK1-Box](https://tokopedia.link/xhWeQgTuwfb), [H96-Max-X3](https://tokopedia.link/KuWvwoYuwfb), [Ugoos-X3](https://tokopedia.link/duoIXZpdGgb), [X96-Air](https://tokopedia.link/5WHiETbdGgb), [A95XF3-Air](https://tokopedia.link/ByBL45jdGgb) | All | armbian_aml_s905x3_buster_*.img |
| s905x2 | [X96Max-4G](https://tokopedia.link/HcfLaRzjqeb), [X96Max-2G](https://tokopedia.link/HcfLaRzjqeb) | All | armbian_aml_s905x2_buster_*.img |
| s912 | [H96-Pro-Plus](https://tokopedia.link/jb42fsBdGgb), Octopus-Planet | All | armbian_aml_s912_buster_*.img |
| s905d | Phicomm-N1 | All | armbian_aml_s905d_buster_*.img |
| s905x | [HG680P](https://tokopedia.link/HbrIbqQcGgb), [B860H](https://tokopedia.link/LC4DiTXtEib) | 5.4.* | armbian_aml_s905x_buster_*.img |
| s905w | [X96-Mini](https://tokopedia.link/ro207Hsjqeb), [TX3-Mini](https://www.tokopedia.com/beststereo/tanix-tx3-mini-2gb-16gb-android-7-1-kodi-17-3-amlogic-s905w-4k-tv-box) | 5.4.* | armbian_aml_s905w_buster_*.img |

## Install to EMMC and update instructions

Choose the corresponding firmware according to your STB. Then write the IMG file to the USB hard disk through software such as [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/). Insert the USB hard disk into the STB. Common for all `Amlogic S9xxx STB`.

- ### Install Armbian to EMMC

Login in to armbian (default user: root, default password: 1234) → input command:

```yaml
armbian-install
```

- ### Update Armbian Kernel

Query the available [kernel_version](https://github.com/ophub/flippy-kernel/tree/main/library). Login in to armbian → input command:

```yaml
# Run as root user (sudo -i), input command: armbian-update <kernel_version>
armbian-update 5.10.66
```

- ### Install Docker Service

Login in to armbian → input command:

```yaml
armbian-docker
```

- ### Modify Armbian Config

Login in to armbian → input command:

```yaml
armbian-config
```

## Armbian firmware make method

- Different amlogic armbian firmware, use the corresponding soc code to generate. Please choose according to your box soc model. Supported soc are `s922x`, `s905x3`, `s905x2`, `s912`, `s905d`, `s905x`, `s905w`.

- compile `a single soc` can be directly input `sudo ./make s905x3`. When `multiple soc` is compiled at the same time, please use `_` to connect multiple soc, such as `sudo ./make s922x_s905x3`

- Optionality: Replace the kernel. Run Eg: `sudo ./make s905x 5.4.150`. When multiple kernel versions are generated at one time, the kernel version number is connected with `_` . Run Eg: `sudo ./make s922x_s905x3 5.10.70_5.4.150`.  When there is an latest version of the same series of the specified kernel version, the `latest version` will be download from [kernel library](https://github.com/ophub/flippy-kernel/tree/main/library) and used automatically. When you want to compile a `fixed kernel`, Run `sudo ./make s905x 5.4.150 false`.

💡Tips: The ***`s905x`*** and ***`s905w`*** boxs currently only support `5.4.*` kernels, Cannot use kernel version 5.10 and above. Please add kernel substitution variables when compiling these two models of devices. Other devices can be freely selected.

- ### Use GitHub Action to make instructions

1. Workflows configuration in [.yml](.github/workflows) files. Set the armbian `soc` you want to make in `Make Armbian for amlogic s9xxx`.

2. New compilation: Select ***`Build Armbian For Amlogic`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

3. Compile again: If there is an `Armbian_.*-trunk_.*.img.gz` file in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), you do not need to compile it completely, you can directly use this file to `build armbian` of different soc. Select ***`Use Releases file to build`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

- ### Local make instructions

1. Install the necessary packages (E.g Ubuntu 20.04 LTS user)
```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/ubuntu-2004-server)
```
2. Clone the repository to the local. `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`
3. Create the `build/output/images` folder, and upload the Armbian image of the `lepotato` board ( Eg: `Armbian_21.11.0-trunk_Lepotato_buster_current_5.10.67.img` ) to this `~/amlogic-s9xxx-armbian/build/output/images` directory.
4. Enter the `~/amlogic-s9xxx-armbian` root directory. And run Eg: `sudo ./make s905x3` to make armbian for `amlogic s9xxx`. The generated Armbian image is in the `build/output/images` directory under the root directory.

## Acknowledgments

The [armbian](https://github.com/armbian/build) directly calls the official source code for compilation, The `u-boot` uses related resources developed by [flippy](https://github.com/unifreq/openwrt_packit) for `amlogic s9xxx openwrt`. The compilation ideas refer to the tutorial of [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box), thanks.

## License

[LICENSE](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/LICENSE) © OPHUB

