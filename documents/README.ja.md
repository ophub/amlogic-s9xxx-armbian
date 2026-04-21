# Armbian の構築と使用方法

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

GitHub Actions は Microsoft が提供するサービスであり、高性能な仮想サーバー環境を利用して、プロジェクトの構築、テスト、パッケージング、デプロイが可能です。パブリックリポジトリでは時間制限なく無料で利用でき、1回のコンパイル時間は最大6時間で、Armbian のコンパイルニーズを十分に満たします（通常3時間程度で完了します）。本プロジェクトは技術交流のみを目的としており、不備がありましたらご容赦ください。ネットワーク攻撃や GitHub Actions の悪用はお控えください。

# 目次

- [Armbian の構築と使用方法](#armbian-の構築と使用方法)
- [目次](#目次)
  - [1. 自分の Github アカウントを登録する](#1-自分の-github-アカウントを登録する)
  - [2. プライバシー変数 GITHUB\_TOKEN 等の設定](#2-プライバシー変数-github_token-等の設定)
  - [3. リポジトリをフォークしてワークフロー権限を設定する](#3-リポジトリをフォークしてワークフロー権限を設定する)
  - [4. Armbian システムのカスタマイズファイルの説明](#4-armbian-システムのカスタマイズファイルの説明)
  - [5. システムのコンパイル](#5-システムのコンパイル)
    - [5.1 手動コンパイル](#51-手動コンパイル)
    - [5.2 定期コンパイル](#52-定期コンパイル)
    - [5.3 デフォルトシステム設定のカスタマイズ](#53-デフォルトシステム設定のカスタマイズ)
    - [5.4 論理ボリュームを使用して Github Actions のコンパイル領域を拡張する](#54-論理ボリュームを使用して-github-actions-のコンパイル領域を拡張する)
    - [5.5 Armbian Docker イメージの作成](#55-armbian-docker-イメージの作成)
  - [6. システムの保存](#6-システムの保存)
  - [7. システムのダウンロード](#7-システムのダウンロード)
  - [8. Armbian を EMMC にインストールする](#8-armbian-を-emmc-にインストールする)
    - [8.1 Amlogic シリーズのインストール方法](#81-amlogic-シリーズのインストール方法)
    - [8.2 Rockchip シリーズのインストール方法](#82-rockchip-シリーズのインストール方法)
      - [8.2.1 Radxa-Rock5B のインストール方法](#821-radxa-rock5b-のインストール方法)
        - [8.2.1.1 microSD へのシステムインストール](#8211-microsd-へのシステムインストール)
        - [8.2.1.2 eMMC へのシステムインストール](#8212-emmc-へのシステムインストール)
        - [8.2.1.3 NVMe へのシステムインストール](#8213-nvme-へのシステムインストール)
      - [8.2.2 電犀牛 R66S のインストール方法](#822-電犀牛-r66s-のインストール方法)
      - [8.2.3 電犀牛 R68S のインストール方法](#823-電犀牛-r68s-のインストール方法)
      - [8.2.4 貝殻クラウドのインストール方法](#824-貝殻クラウドのインストール方法)
      - [8.2.5 我家クラウドのインストール方法](#825-我家クラウドのインストール方法)
      - [8.2.6 泰山派のインストール方法](#826-泰山派のインストール方法)
    - [8.3 Allwinner シリーズのインストール方法](#83-allwinner-シリーズのインストール方法)
    - [8.4 Docker 版 Armbian のインストール方法](#84-docker-版-armbian-のインストール方法)
      - [8.4.1 Docker 実行環境のインストール](#841-docker-実行環境のインストール)
      - [8.4.2 macvlan ネットワークの設定](#842-macvlan-ネットワークの設定)
      - [8.4.3 Armbian Docker コンテナの実行](#843-armbian-docker-コンテナの実行)
  - [9. Armbian カーネルのコンパイル](#9-armbian-カーネルのコンパイル)
    - [9.1 カスタムカーネルパッチの追加方法](#91-カスタムカーネルパッチの追加方法)
    - [9.2 カーネルパッチの作成方法](#92-カーネルパッチの作成方法)
    - [9.3 カスタムドライバモジュールのコンパイル方法](#93-カスタムドライバモジュールのコンパイル方法)
  - [10. Armbian カーネルの更新](#10-armbian-カーネルの更新)
  - [11. よく使うソフトウェアのインストール](#11-よく使うソフトウェアのインストール)
  - [12. よくある質問](#12-よくある質問)
    - [12.1 各デバイスの dtb と u-boot の対応表](#121-各デバイスの-dtb-と-u-boot-の対応表)
    - [12.2 LED ディスプレイ制御の説明](#122-led-ディスプレイ制御の説明)
    - [12.3 元の Android TV システムの復元方法](#123-元の-android-tv-システムの復元方法)
      - [12.3.1 armbian-ddbr を使用したバックアップと復元](#1231-armbian-ddbr-を使用したバックアップと復元)
      - [12.3.2 Amlogic フラッシュツールを使用した復元](#1232-amlogic-フラッシュツールを使用した復元)
    - [12.4 デバイスを USB/TF/SD から起動する設定](#124-デバイスを-usbtfsd-から起動する設定)
      - [12.4.1 Armbian システムの初回インストール](#1241-armbian-システムの初回インストール)
      - [12.4.2 Armbian システムの再インストール](#1242-armbian-システムの再インストール)
    - [12.5 赤外線レシーバーの無効化](#125-赤外線レシーバーの無効化)
    - [12.6 ブートファイルの選択](#126-ブートファイルの選択)
    - [12.7 ネットワーク設定](#127-ネットワーク設定)
      - [12.7.1 interfaces を使用したネットワーク設定](#1271-interfaces-を使用したネットワーク設定)
        - [12.7.1.1 DHCP による動的 IP アドレスの割り当て](#12711-dhcp-による動的-ip-アドレスの割り当て)
        - [12.7.1.2 静的 IP アドレスの手動設定](#12712-静的-ip-アドレスの手動設定)
        - [12.7.1.3 Docker 内で OpenWrt を使用した相互通信ネットワークの構築](#12713-docker-内で-openwrt-を使用した相互通信ネットワークの構築)
      - [12.7.2 NetworkManager を使用したネットワーク管理](#1272-networkmanager-を使用したネットワーク管理)
        - [12.7.2.1 新しいネットワーク接続の作成](#12721-新しいネットワーク接続の作成)
          - [12.7.2.1.1 ネットワークインターフェース名の取得](#127211-ネットワークインターフェース名の取得)
          - [12.7.2.1.2 既存のネットワーク接続名の取得](#127212-既存のネットワーク接続名の取得)
          - [12.7.2.1.3 新しい有線ネットワーク接続の作成](#127213-新しい有線ネットワーク接続の作成)
          - [12.7.2.1.4 新しい無線ネットワーク接続の作成](#127214-新しい無線ネットワーク接続の作成)
        - [12.7.2.2 無線ネットワーク接続の WiFi SSID または PASSWD の変更](#12722-無線ネットワーク接続の-wifi-ssid-または-passwd-の変更)
        - [12.7.2.3 ネットワークアドレス割り当て方式の変更](#12723-ネットワークアドレス割り当て方式の変更)
          - [12.7.2.3.1 静的 IP アドレス - IPv4](#127231-静的-ip-アドレス---ipv4)
          - [12.7.2.3.2 DHCP による動的 IP アドレスの取得 - IPv4 / IPv6](#127232-dhcp-による動的-ip-アドレスの取得---ipv4--ipv6)
        - [12.7.2.4 ネットワーク接続の MAC アドレスの変更](#12724-ネットワーク接続の-mac-アドレスの変更)
          - [12.7.2.4.1 方法1：`nmcli` コマンドで MAC アドレスを変更する](#127241-方法1nmcli-コマンドで-mac-アドレスを変更する)
          - [12.7.2.4.2 方法2：設定ファイルで MAC アドレスを変更する](#127242-方法2設定ファイルで-mac-アドレスを変更する)
        - [12.7.2.5 IPv6 を無効にする方法](#12725-ipv6-を無効にする方法)
      - [12.7.3 ワイヤレスの有効化方法](#1273-ワイヤレスの有効化方法)
      - [12.7.4 Bluetooth の有効化方法](#1274-bluetooth-の有効化方法)
    - [12.8 起動時のタスク追加方法](#128-起動時のタスク追加方法)
    - [12.9 システム内のサービススクリプトの更新方法](#129-システム内のサービススクリプトの更新方法)
    - [12.10 eMMC 上の Android システムのパーティション情報の取得方法](#1210-emmc-上の-android-システムのパーティション情報の取得方法)
      - [12.10.1 パーティション情報の取得](#12101-パーティション情報の取得)
      - [12.10.2 パーティション情報の共有](#12102-パーティション情報の共有)
      - [12.10.3 パーティション情報の解読](#12103-パーティション情報の解読)
      - [12.10.4 eMMC インストールへの使用](#12104-emmc-インストールへの使用)
    - [12.11 u-boot ファイルの作成方法](#1211-u-boot-ファイルの作成方法)
      - [12.11.1 Amlogic デバイスの u-boot ファイルの作成方法](#12111-amlogic-デバイスの-u-boot-ファイルの作成方法)
        - [12.11.1.1 bootloader と dtb ファイルの抽出方法](#121111-bootloader-と-dtb-ファイルの抽出方法)
        - [12.11.1.2 acs.bin ファイルの作成方法](#121112-acsbin-ファイルの作成方法)
        - [12.11.1.3 u-boot ファイルのコンパイル方法](#121113-u-boot-ファイルのコンパイル方法)
      - [12.11.2 Rockchip デバイスの u-boot ファイルの作成方法](#12112-rockchip-デバイスの-u-boot-ファイルの作成方法)
        - [12.11.2.1 Radxa の u-boot 作成スクリプトの使用方法](#121121-radxa-の-u-boot-作成スクリプトの使用方法)
        - [12.11.2.2 cm9vdA の u-boot 作成スクリプトの使用方法](#121122-cm9vda-の-u-boot-作成スクリプトの使用方法)
    - [12.12 メモリサイズの誤認識](#1212-メモリサイズの誤認識)
    - [12.13 dtb ファイルの逆コンパイル方法](#1213-dtb-ファイルの逆コンパイル方法)
    - [12.14 cmdline 設定の変更方法](#1214-cmdline-設定の変更方法)
    - [12.15 新しいサポートデバイスの追加方法](#1215-新しいサポートデバイスの追加方法)
      - [12.15.1 デバイス設定ファイルの追加](#12151-デバイス設定ファイルの追加)
      - [12.15.2 システムファイルの追加](#12152-システムファイルの追加)
      - [12.15.3 u-boot ファイルの追加](#12153-u-boot-ファイルの追加)
        - [12.15.3.1 パーティション情報レイアウトの確認](#121531-パーティション情報レイアウトの確認)
        - [12.15.3.2 重要なパーティションのバックアップ](#121532-重要なパーティションのバックアップ)
        - [12.15.3.3 特殊パーティション書き込みファイルの追加](#121533-特殊パーティション書き込みファイルの追加)
      - [12.15.4 フロー制御ファイルの追加](#12154-フロー制御ファイルの追加)
    - [12.16 eMMC への書き込み時の I/O エラーの解決方法](#1216-emmc-への書き込み時の-io-エラーの解決方法)
    - [12.17 Bullseye バージョンで音が出ない問題の解決方法](#1217-bullseye-バージョンで音が出ない問題の解決方法)
    - [12.18 boot.scr ファイルのコンパイル方法](#1218-bootscr-ファイルのコンパイル方法)
    - [12.19 リモートデスクトップの有効化とデフォルトポートの変更方法](#1219-リモートデスクトップの有効化とデフォルトポートの変更方法)
    - [12.20 TCP 輻輳制御の最適化方法](#1220-tcp-輻輳制御の最適化方法)

## 1. 自分の Github アカウントを登録する

後続のシステムカスタマイズ操作を行うために、アカウントを登録してください。github.com ウェブサイトの右上にある `Sign up` ボタンをクリックし、案内に従って登録を完了してください。

## 2. プライバシー変数 GITHUB_TOKEN 等の設定

[GitHub ドキュメント](https://docs.github.com/ja/actions/security-guides/automatic-token-authentication#about-the-github_token-secret)によると、各ワークフロージョブの開始時に、GitHub は自動的にユニークな `GITHUB_TOKEN` シークレットを作成し、ワークフローで使用します。ワークフロージョブ内で `${{ secrets.GITHUB_TOKEN }}` を使用して認証できます。

Actions で [Armbian Docker](../.github/workflows/build-armbian-arm64-docker-image.yml) イメージを作成して Docker Hub にプッシュする場合、`DOCKERHUB_USERNAME` と `DOCKERHUB_PASSWORD` の2つのプライバシー変数を設定する必要があります。自分のリポジトリページで、右上の `Settings` > `Secrets and variables` > `Actions` > `Repository secrets` > `New repository secret` ボタンをクリックし、以下の2つのプライバシー変数を追加してください：

- 変数名 `DOCKERHUB_USERNAME`：値は Docker Hub のログインユーザー名
- 変数名 `DOCKERHUB_PASSWORD`：値は Docker Hub のログインパスワード

## 3. リポジトリをフォークしてワークフロー権限を設定する

リポジトリ https://github.com/ophub/amlogic-s9xxx-armbian を開き、右上の Fork ボタンをクリックして、リポジトリのコードを自分のアカウントにコピーします。フォーク完了後、自分のアカウントの amlogic-s9xxx-armbian リポジトリにアクセスし、右上の `Settings` > `Actions` > `General` > `Workflow permissions` で `Read and write permissions` を選択して保存します。操作の流れは以下の通りです：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. Armbian システムのカスタマイズファイルの説明

システムのコンパイルプロセスは [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) ファイルによって制御されます。workflows ディレクトリ内の他の .yml ファイルはそれぞれ異なる機能を実現するために使用されます。コンパイル時には Armbian 公式の最新コードを使用してリアルタイムにコンパイルされます。関連パラメータについては公式ドキュメントを参照してください。

```yaml
- name: Compile Armbian [ ${{ inputs.set_release }} ]
  id: compile
  if: ${{ steps.down.outputs.status }} == 'success' && !cancelled()
  run: |
    # Compile method and parameter description: https://docs.armbian.com/Developer-Guide_Build-Options
    cd build/
        ./compile.sh RELEASE=${{ inputs.set_release }} BOARD=odroidn2 BRANCH=current BUILD_MINIMAL=no \
                      BUILD_ONLY=default HOST=armbian BUILD_DESKTOP=no EXPERT=yes KERNEL_CONFIGURE=no \
                      COMPRESS_OUTPUTIMAGE="sha" SHARE_LOG=yes
    echo "status=success" >> ${GITHUB_OUTPUT}
```

`ophub` を使用して Armbian をパッケージングする際、`armbian_files` パラメータにより、ophub の [common-files](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/build-armbian/armbian-files/common-files) ディレクトリにカスタムファイルを追加または上書きできます。ディレクトリ構造は Armbian のルートディレクトリと同じに保つ必要があり、ファイルがファームウェアに正しく上書きされるようにします（例：デフォルト設定ファイルは `etc/default/` サブディレクトリに配置する必要があります）。例は以下の通りです：

```yaml
- name: Rebuild Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img.gz
    armbian_files: files
    ...
```

## 5. システムのコンパイル

システムのコンパイルは、手動コンパイル、定期コンパイル、特定イベントによるトリガーコンパイルなど、複数の方法をサポートしています。

### 5.1 手動コンパイル

リポジトリのナビゲーションバーで Actions をクリックし、Build armbian > Run workflow > Run workflow の順にクリックするとコンパイルが開始されます。全プロセスには約3時間かかります。操作の流れは以下の通りです：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 定期コンパイル

[.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) ファイル内で、Cron を使用して定期コンパイルを設定します。5つの異なる位置はそれぞれ 分 (0 - 59) / 時 (0 - 23) / 日 (1 - 31) / 月 (1 - 12) / 曜日 (0 - 6)（日曜日 - 土曜日）を表します。各位置の数値を変更して時刻を設定してください。システムはデフォルトで UTC 標準時を使用しますので、お住まいの国のタイムゾーンに合わせて換算してください。

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 デフォルトシステム設定のカスタマイズ

デフォルトのシステム設定情報は [model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) ファイルに記録されています。`BOARD` 名は一意でなければなりません。

`BUILD` 値が `yes` のデバイスはデフォルトで構築されるデバイスであり、対応するシステムを直接使用できます。デフォルト値が `no` のデバイスはデフォルトの構築に含まれておらず、使用する際は同じ `FAMILY` の構築済みシステムをダウンロードする必要があります。`USB` に書き込んだ後、パソコンで `USB の boot パーティション` を開き、`/boot/uEnv.txt` ファイル内の `FDT dtb 名` を変更することで、リスト内の他のデバイスに対応させることができます。

ローカルコンパイル時は `-b` パラメータで指定し、GitHub Actions でのコンパイル時は `armbian_board` パラメータで指定します。`-b all` は `BUILD` が `yes` の全デバイスを構築することを意味します。`BOARD` パラメータを指定する場合、`BUILD` 値が `yes` でも `no` でも構築可能です。例：`-b r68s_s905x3-tx3_s905l3a-cm311`

### 5.4 論理ボリュームを使用して Github Actions のコンパイル領域を拡張する

GitHub Actions のデフォルトコンパイル領域は 84G で、システムと必要なパッケージを除くと利用可能な領域は約 50G です。全ファームウェアのコンパイル時に容量不足が発生する場合があり、論理ボリュームを使用してコンパイル領域を約 110G に拡張できます。[.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) ファイルの方法を参照し、以下のコマンドで論理ボリュームを作成し、コンパイル時に論理ボリュームのパスを使用してください。

```yaml
- name: Create simulated physical disk
  run: |
    mnt_size=$(expr $(df -h /mnt | tail -1 | awk '{print $4}' | sed 's/[[:alpha:]]//g' | sed 's/\..*//') - 1)
    root_size=$(expr $(df -h / | tail -1 | awk '{print $4}' | sed 's/[[:alpha:]]//g' | sed 's/\..*//') - 4)
    sudo truncate -s "${mnt_size}"G /mnt/mnt.img
    sudo truncate -s "${root_size}"G /root.img
    sudo losetup /dev/loop6 /mnt/mnt.img
    sudo losetup /dev/loop7 /root.img
    sudo pvcreate /dev/loop6
    sudo pvcreate /dev/loop7
    sudo vgcreate github /dev/loop6 /dev/loop7
    sudo lvcreate -n runner -l 100%FREE github
    sudo mkfs.xfs /dev/github/runner
    sudo mkdir -p /builder
    sudo mount /dev/github/runner /builder
    sudo chown -R runner.runner /builder
    df -Th
```

### 5.5 Armbian Docker イメージの作成

Armbian システムの [Docker](https://hub.docker.com/u/ophub) イメージの作成方法については、[armbian_docker](../compile-kernel/tools/script/docker) 作成スクリプトを参照してください。

## 6. システムの保存

システムの保存設定も [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) ファイルで制御されます。コンパイル完了したシステムはスクリプトにより自動的に GitHub 公式の Releases にアップロードされます。

```yaml
- name: Upload Armbian image to Release
  uses: ncipollo/release-action@main
  if: ${{ env.PACKAGED_STATUS }} == 'success' && !cancelled()
  with:
    tag: Armbian_${{ env.ARMBIAN_RELEASE }}_${{ env.PACKAGED_OUTPUTDATE }}
    artifacts: ${{ env.PACKAGED_OUTPUTPATH }}/*
    allowUpdates: true
    token: ${{ secrets.GITHUB_TOKEN }}
    body: |
      These are the Armbian OS image
      * OS information
      Default username: root
      Default password: 1234
      Install command: armbian-install
      Update command: armbian-update
```

## 7. システムのダウンロード

リポジトリのトップページ右下の Release セクションからアクセスし、自分のデバイスモデルに対応するシステムを選択してください。操作の流れは以下の通りです：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. Armbian を EMMC にインストールする

Amlogic、Rockchip、Allwinner のインストール方法はそれぞれ異なります。各デバイスは外部 microSD カード、eMMC、NVMe など異なるストレージメディアをサポートしており、以下ではデバイスタイプ別にインストール方法を説明します。まず [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) から対応デバイスの Armbian システムをダウンロードし、.img 形式に解凍して準備してから、デバイスタイプに応じて以下の該当セクションを参照してインストールしてください。

インストール完了後、デバイスを`ルーター`に接続し、起動から約 `2 分`後、ルーターでデバイス名が Armbian の `IP` アドレスを確認し、`SSH` ツールで接続して管理設定を行えます。デフォルトユーザー名は `root`、デフォルトパスワードは `1234`、デフォルトポートは `22` です。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972715-addcd695-970a-43d6-8a34-24a9c4bc80a2.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972773-fc9e9ef9-69a7-4279-8329-6fad1cf2f5b9.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972818-b72e18cd-17d1-4f9f-a0fa-b6c22eef041d.png width="600" />
</div>

### 8.1 Amlogic シリーズのインストール方法

Armbian システムにログイン（デフォルトユーザー: root, デフォルトパスワード: 1234）→ コマンドを入力：

```shell
armbian-install
```

| オプションパラメータ  | デフォルト値   | 選択肢     | 説明                |
| -------  | ------- | ------  | -----------------   |
| -m       | no      | yes/no  | Mainline u-boot を使用 |
| -a       | yes     | yes/no  | [ampart](https://github.com/7Ji/ampart) パーティションテーブル調整ツールを使用 |
| -l       | no      | yes/no  | List. 全デバイスリストを表示 |

例: `armbian-install -m yes -a no`

### 8.2 Rockchip シリーズのインストール方法

各デバイスのインストール方法は以下の通りです。

#### 8.2.1 Radxa-Rock5B のインストール方法

Radxa-Rock5B は microSD/eMMC/NVMe など複数のストレージメディアをサポートしており、インストール方法はメディアによって異なります。[rk3588_spl_loader_v1.08.111.bin と spi_image.img](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/rock5b) ファイルをダウンロードしておきます。[RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) ツールとドライバをダウンロードしておきます。[Rufus](https://rufus.ie/) または [balenaEtcher](https://www.balena.io/etcher/) 書き込みツールをダウンロードしておきます。

##### 8.2.1.1 microSD へのシステムインストール

Rufus や balenaEtcher などのツールで Armbian システムイメージを microSD に書き込み、microSD をデバイスに挿入すれば使用できます。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 eMMC へのシステムインストール

microSD カードを使用したインストール：Armbian システムイメージを microSD カードに書き込み、microSD カードをデバイスに挿入して起動し、`armbian.img` イメージファイルを microSD カードにアップロードし、`dd` コマンドで Armbian イメージを NVMe に書き込みます。コマンドは以下の通りです：

```Shell
dd if=armbian.img  of=/dev/mmcblk1  bs=1M status=progress
```

- USB-eMMC リーダーを使用したインストール：eMMC モジュールをパソコンに接続し、Rufus や balenaEtcher などのツールで Armbian システムイメージを eMMC に書き込み、eMMC をデバイスに挿入すれば使用できます。
- Maskrom モードを使用したインストール：開発ボードの電源をオフにします。金色のボタンを押し続けます。USB-A to Type-C ケーブルを ROCK 5B の Type-C ポートに挿し、もう一方を PC に接続します。金色のボタンを離します。USB デバイスで MASKROM デバイスが検出されたことを確認します。リストの空白部分を右クリックし、`rock-5b-emmc.cfg` 設定ファイルを読み込みます（設定ファイルは RKDevTool と同じディレクトリにあります）。`rk3588_spl_loader_v1.08.111.bin` と `Armbian.img` を下図のように設定し、書き込みを選択します。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 NVMe へのシステムインストール

ROCK-5B メインボードには SPI フラッシュが搭載されており、ブートローダーを SPI フラッシュに書き込むことで、SoC の maskrom モードが直接サポートしていない起動メディア（SATA、USB3、NVMe など）からの起動が可能になります。NVMe での起動前に SPI ファイルを書き込む必要があります。方法は以下の通りです：

開発ボードの電源をオフにし、MicroSD カード、eMMC モジュールなどの起動可能なデバイスを取り外します。金色（または一部のリビジョンでは銀色）のボタンを押し続け、USB-A to Type-C ケーブルを ROCK-5B の Type-C ポートに挿し、もう一方を PC に接続します。ボタンを離し、USB デバイスで MASKROM デバイスが検出されたか確認します。リストボックスで右クリックして設定を読み込み、エクスプローラーで設定ファイルを選択します（設定ファイルは RKDevTool と同じディレクトリにあります）。下図に従って `rk3588_spl_loader_v1.08.111.bin` と `spi_image.img` ファイルを選択し、書き込みをクリックします：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- リーダーを使用したインストール：M.2 NVMe SSD を M.2 NVMe SSD-USB3.0 リーダーに挿入してホストに接続します。Rufus や balenaEtcher などのツールで Armbian システムイメージを NVMe に書き込み、NVMe をデバイスに挿入すれば使用できます。
- microSD カードを使用したインストール：Armbian システムイメージを microSD カードに書き込み、microSD カードをデバイスに挿入して起動し、`armbian.img` イメージファイルを microSD カードにアップロードし、`dd` コマンドで Armbian イメージを NVMe に書き込みます。コマンドは以下の通りです：

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 電犀牛 R66S のインストール方法

Rufus や balenaEtcher などのツールで Armbian システムイメージを microSD に書き込み、microSD をデバイスに挿入すれば使用できます。

#### 8.2.3 電犀牛 R68S のインストール方法

- [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) ツールとドライバをダウンロードし、解凍して DriverAssitant ドライバをインストールし、RKDevTool ツールを開いておきます。
- R68s が電源オフの状態で、まず USB デュアルオスケーブルを挿し、Recovery キーを押しながら 12V 電源を接続し、2秒後に Recovery キーを離すと、フラッシュツールが`LOADER デバイスを発見`します。
- RKDevTool ツールの操作画面の空白部分を右クリックし、項目を追加します。
- アドレスは `0x00000000`、名前は `armbian`、パスの右側をクリックして `armbian.img` システムファイルを選択します。
- 追加した armbian の行以外の`他の行の選択を解除`し、`実行`をクリックして書き込みます。
- 補足説明：eMMC に他のシステムが書き込まれている場合は、まず高度な機能で消去してから、Armbian システムを書き込んでください。消去できない場合は、まず `MiniLoaderAll.bin` ブートファイルを再度書き込み、その後 `MASKROM` に入って Armbian システムを書き込んでください。MiniLoaderAll.bin ブートファイルの設定：アドレス `0xCCCCCCCC`、名前 `Loader`、パスは RKDevTool フラッシュツールの Image ディレクトリにある `MiniLoaderAll.bin` ファイルを選択します。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 貝殻クラウドのインストール方法

方法は [milton](https://www.cnblogs.com/milton/p/15391525.html) のチュートリアルから転載しています。フラッシュするには Maskrom モードに入る必要があります。まずすべての接続を切断し、CLK と GND（TTL の GND、または隣の小さいボタンの GND のどちらでも可）の2つの接点をショートさせてから、USB を PC に接続すると MASKROM デバイスが検出されます。ショートポイントの位置は以下の通りです：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

RKDevTool フラッシュツールを開き、右クリックで項目を追加します。

- アドレス `0xCCCCCCCC`、名前 `Boot`、パスは [こちら](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/beikeyun) から `rk3328_loader_v1.14.249.bin` を選択します。
- アドレス `0x00000000`、名前 `system`、パスはフラッシュする `Armbian.img` システムを選択します。
- `強制アドレス書き込み` にチェックを入れ、`実行` をクリックし、右側のダウンロードパネルに進捗が完了と表示されるまで待ちます。

#### 8.2.5 我家クラウドのインストール方法

方法は [cc747](https://post.smzdm.com/p/a4wkdo7l/) のチュートリアルから転載しています。フラッシュするには Maskrom モードに入る必要があります。デバイスが電源オフの状態であることを確認し、すべてのケーブルを抜きます。USB デュアルオスケーブルの一方を我家クラウドの USB2.0 ポートに、もう一方をパソコンに接続します。クリップを Reset 穴に挿し、押し続けます。電源ケーブルを接続し、数秒待ち、RKDevTool の下部に `LOADER デバイスを発見` と表示されたらクリップを離します。RKDevTool を `高度な機能` に切り替え、`Maskrom に入る` ボタンをクリックすると、`MASKROM デバイスを発見` と表示されます。右クリックで項目を追加します。

- アドレス `0xCCCCCCCC`、名前 `Boot`、パスは [こちら](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/chainedbox) から `rk3328_loader_v1.14.249.bin` を選択します。
- アドレス `0x00000000`、名前 `system`、パスはフラッシュする `Armbian.img` システムを選択します。
- `強制アドレス書き込み` にチェックを入れ、`実行` をクリックし、右側のダウンロードパネルに進捗が完了と表示されるまで待ちます。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a6d2d8c0-35c5-44ba-be35-fd2e2758729b width="600" /><br />
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/13aab016-1b93-4ff1-b1ef-c202bd357068 width="600" />
</div>

#### 8.2.6 泰山派のインストール方法
- [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) ツールとドライバをダウンロードし、解凍して DriverAssitant ドライバをインストールし、RKDevTool ツールを開きます。（注意：2.92ではなく2.86バージョンのツールを使用してください。2.92バージョンは書き込み時にクラッシュします）
- 泰山派が電源オフの状態で Recovery キーを押しながら Type-C データケーブルを接続し、RKDevTool に `LOADER デバイスを発見` と表示されたら Recovery キーを離します。右クリックで項目を追加します。
- アドレス `0x00000000`、名前 `system`、パスはフラッシュする `Armbian.img` システムを選択します。
- 実行をクリックし、プログレスバーが完了するまで待ちます。


### 8.3 Allwinner シリーズのインストール方法

Armbian システムにログイン（デフォルトユーザー: root, デフォルトパスワード: 1234）→ コマンドを入力：

```shell
armbian-install
```

### 8.4 Docker 版 Armbian のインストール方法

Ubuntu/Debian/Armbian システムで Docker 版の Armbian イメージを使用できます。これらのイメージは [Docker Hub](https://hub.docker.com/r/ophub) にホストされており、直接ダウンロードして使用できます。

4つの異なるカーネルバージョンの Armbian Docker イメージが提供されています：`armbian-trixie`、`armbian-bookworm`、`armbian-noble`、`armbian-resolute`。各バージョンとも `arm64` と `amd64` アーキテクチャを提供しており、必要に応じて選択できます。

armbian-trixie は debian13 ベース、armbian-bookworm は debian12 ベース、armbian-noble は ubuntu24.04 ベース、armbian-resolute は ubuntu26.04 ベースです。

arm64 バージョンは Amlogic/Rockchip/Allwinner などのプラットフォームアーキテクチャのデバイスに適用され、amd64 バージョンは x86_64 アーキテクチャの PC やサーバーに適用されます。

#### 8.4.1 Docker 実行環境のインストール

```shell
curl -fsSL https://get.docker.com | sh
sudo usermod -aG docker $USER
sudo newgrp docker
```

#### 8.4.2 macvlan ネットワークの設定

```shell
# 既存の docker ネットワークに macvlan ネットワークが含まれているか確認
docker network ls

# macvlan ネットワークがない場合は作成
# サブネット、ゲートウェイ、ネットワークインターフェース名は実際のネットワークに合わせて変更してください
docker network create -d macvlan \
    --subnet=10.1.1.0/24 \
    --gateway=10.1.1.1 \
    -o parent=eth0 \
    macvlan
```

#### 8.4.3 Armbian Docker コンテナの実行

`armbian-trixie:arm64` イメージを例に、Armbian コンテナの実行方法を説明します。

```shell
# バックグラウンドで Armbian コンテナを実行
# コンテナ名、IPアドレス、イメージバージョンなどは実際の状況に合わせて変更してください
docker run -itd --name=armbian-trixie \
    --privileged \
    --network macvlan \
    --ip 10.1.1.15 \
    --hostname=armbian-trixie \
    -e TZ=Asia/Shanghai \
    --restart unless-stopped \
    ophub/armbian-trixie:arm64

# Armbian コンテナのログを確認
docker logs -f armbian-trixie

# Armbian コンテナに入る
docker exec -it armbian-trixie bash

# Armbian コンテナから出る
exit

# Armbian コンテナを停止して削除
docker rm -f armbian-trixie
```

## 9. Armbian カーネルのコンパイル

Ubuntu、Debian、Armbian システムでのカーネルコンパイルをサポートしており、ローカルコンパイルと GitHub Actions クラウドコンパイルの両方に対応しています。詳細な方法については [カーネルコンパイル説明](../../compile-kernel/README.ja.md) を参照してください。

### 9.1 カスタムカーネルパッチの追加方法

カーネルパッチディレクトリ [tools/patch](../../compile-kernel/tools/patch) に共通カーネルパッチディレクトリ（`common-kernel-patches`）、または `カーネルソースリポジトリと同名` のディレクトリがある場合（例：[linux-5.15.y](https://github.com/unifreq/linux-5.15.y)）、`-p true` でカーネルパッチを自動適用できます。パッチディレクトリの命名規則は以下の通りです：

```shell
~/amlogic-s9xxx-armbian
    └── compile-kernel
        └── tools
            └── patch
                ├── common-kernel-patches  # 固定ディレクトリ名：各バージョン共通のカーネルパッチを格納
                ├── linux-5.15.y           # カーネルソースリポジトリと同名：専用パッチを格納
                ├── linux-6.1.y
                ├── linux-5.10.y-rk35xx
                └── more kernel directory...
```

- ローカルでカーネルをコンパイルする場合、対応するディレクトリを手動で作成し、カスタムカーネルパッチを追加できます。
- GitHub Actions クラウドコンパイル時は、`kernel_patch` パラメータでリポジトリ内のカーネルパッチディレクトリを指定できます。例えば [kernel](https://github.com/ophub/kernel) リポジトリの [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) での使用方法：

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.1_6.1.1
    kernel_auto: true
    kernel_patch: kernel-patch/beta
    auto_patch: true
```

`kernel_patch` パラメータでカスタムカーネルパッチを指定する場合、パッチディレクトリは上記の規則に従って命名してください。

### 9.2 カーネルパッチの作成方法

- [Armbian](https://github.com/armbian/build) や [OpenWrt](https://github.com/openwrt/openwrt) などのリポジトリから取得：例えば [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) や [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1)、[lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15) など、メインラインカーネルに基づくリポジトリのパッチは通常そのまま使用できます。
- github.com リポジトリのコミットから取得：対応する `commit` アドレスの末尾に `.patch` を追加するとパッチを生成できます。

カスタムカーネルパッチを追加する前に、上流のカーネルソースリポジトリ [unifreq/linux-k.x.y](https://github.com/unifreq) と照合し、そのパッチがまだマージされていないことを確認して、コンフリクトを避けてください。テスト済みのカーネルパッチは、unifreq が管理するカーネルリポジトリシリーズへの提出をお勧めします。コミュニティのすべての貢献が、Armbian と OpenWrt システムをより安定して信頼できるものにします。

### 9.3 カスタムドライバモジュールのコンパイル方法

Linux メインラインカーネルには一部のドライバがまだ内蔵されていないため、自分でドライバモジュールをコンパイルできます。メインラインカーネルをサポートするドライバを選択してください。Android ドライバは通常メインラインカーネルと互換性がなく、コンパイルできません。例は以下の通りです：

```shell
# ステップ1：最新カーネルに更新
# 初期の header ファイルが不完全なため、最新のカーネルに更新する必要があります。
# 各カーネルバージョンの要件：5.10.222, 5.15.163, 6.1.100, 6.6.41 以上
armbian-sync
armbian-update -k 6.1


# ステップ2：コンパイルツールのインストール
mkdir -p /usr/local/toolchain
cd /usr/local/toolchain
# コンパイルツールのダウンロード
wget https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# 解凍
tar -Jxf arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# その他のコンパイル依存パッケージのインストール（オプション、エラーメッセージに応じて不足分を手動インストール可能）
armbian-kernel -u


# ステップ3：ドライバのダウンロードとコンパイル
# ドライバソースコードのダウンロード
cd ~/
git clone https://github.com/jwrdegoede/rtl8189ES_linux
cd rtl8189ES_linux
# コンパイル環境の設定
gun_file="arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
toolchain_path="/usr/local/toolchain"
toolchain_name="gcc"
export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-linux-gnu-"
export CC="${CROSS_COMPILE}gcc"
export LD="${CROSS_COMPILE}ld.bfd"
export ARCH="arm64"
export KSRC=/usr/lib/modules/$(uname -r)/build
# ソースコードの実際のパスに基づいて M 変数を設定
export M="/root/rtl8189ES_linux"
# ドライバのコンパイル
make


# ステップ4：ドライバのインストール
sudo cp -f 8189es.ko /lib/modules/$(uname -r)/kernel/drivers/net/wireless/
# モジュール依存関係の更新
sudo depmod -a
# ドライバモジュールのロード
sudo modprobe 8189es
# ドライバがロードされたか確認
lsmod | grep 8189es
# 正常にドライバがロードされたことを確認できます
8189es               1843200  0
cfg80211              917504  2 8189es,brcmfmac
```

図は以下の通りです：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a89cbe6-df38-4862-8d11-9d977e0f4191">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a1d0bb9-44d4-4de5-9907-47e5f20747a7">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/d1bd2eff-4c57-4e91-a870-08b0f8b1fe16">
</div>

## 10. Armbian カーネルの更新

Armbian システムにログイン → コマンドを入力：

```shell
# root ユーザーで実行 (sudo -i)
# パラメータを指定しない場合、最新バージョンに更新されます。
armbian-update
```

| オプションパラメータ  | デフォルト値        | 選択肢           | 説明                              |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | github.com からカーネルをダウンロードするリポジトリを設定  |
| -u       | 自動        | stable/flippy/beta/rk3588/rk35xx/h6 | 使用するカーネルの [tags サフィックス](https://github.com/ophub/kernel/releases) を設定 |
| -k       | 最新版        | カーネルバージョン       | [カーネルバージョン](https://github.com/ophub/kernel/releases/tag/kernel_stable) を設定  |
| -b       | yes          | yes/no        | カーネル更新時に現在使用中のカーネルを自動バックアップ    |
| -d       | deb          | tar/deb       | 優先使用するカーネルパッケージ形式を設定。指定形式が存在しない場合、スクリプトは自動的に別の形式を試行します。カスタムドライバをコンパイルする場合は `deb` 形式を推奨。 |
| -m       | no           | yes/no        | メインライン u-boot を使用                    |
| -s       | なし           | なし/ディスク名称     | [SOS] eMMC/NVMe/sdX などのディスク内のシステムカーネルを復旧 |
| -h       | なし           | なし             | 使用方法のヘルプを表示                       |

例: `armbian-update -k 5.15 -u stable -d deb`

`-k` パラメータでカーネルバージョン番号を指定する場合、正確なバージョン番号の指定（例：`armbian-update -k 5.15.50`）と、カーネルシリーズの指定（例：`armbian-update -k 5.15`）の両方が可能です。シリーズを指定した場合、そのシリーズの最新バージョンが自動的に使用されます。

カーネル更新時には、現在使用中のカーネルが自動的に `/ddbr/backup` ディレクトリにバックアップされ、最新の3バージョンが保持されます。新しいカーネルが不安定な場合、いつでもバックアップバージョンに復元できます：
```shell
# バックアップしたカーネルディレクトリに移動（例：6.6.12）
cd /ddbr/backup/6.6.12
# カーネル更新コマンドを実行すると、現在のディレクトリ内のカーネルが自動インストールされます
armbian-update
```

[SOS]：異常によりアップデートが不完全な状態となり、eMMC/NVMe/sdX からシステムが起動できない場合、USB などの別のディスクから任意のバージョンの Armbian システムを起動し、`armbian-update -s` コマンドを実行して USB 内のシステムカーネルを eMMC/NVMe/sdX に復旧させることで、システムレスキューを行えます。ディスクパラメータを指定しない場合、デフォルトで USB デバイスから eMMC/NVMe/sdX 内のカーネルを復旧します。デバイスに複数のディスクがある場合、ターゲットディスク名を正確に指定できます。例は以下の通りです：

```shell
# eMMC 内のカーネルを復旧
armbian-update -s mmcblk1
# NVMe 内のカーネルを復旧
armbian-update -s nvme0n1
# リムーバブルストレージデバイス内のカーネルを復旧
armbian-update -s sda
# ディスク名は mmcblk0/mmcblk1/nvme0n1/nvme1n1/sda/sdb/sdc などの省略形や、/dev/sda のような完全名も使用可能
armbian-update -s /dev/sda
# デバイスに eMMC/NVMe/sdX のうち1つの内蔵ストレージしかない場合、ディスク名パラメータを省略可能
armbian-update -s
```

ネットワークの問題で github.com にアクセスしてオンライン更新ができない場合、カーネルファイルを手動でダウンロードして Armbian システムの任意のディレクトリにアップロードできます。その後、そのディレクトリに移動し、`armbian-update` コマンドを実行するとローカルインストールが行えます。カレントディレクトリに完全なカーネルファイルセットが存在する場合、システムはローカルファイルを優先して更新に使用します。カーネルは `tar` と `deb` の2つの形式をサポートしており、それぞれ必要な4つのコアファイルは以下の通りです：

- `tar` 形式の更新に必要な4ファイル：`header-xxx.tar.gz`、`boot-xxx.tar.gz`、`dtb-xxx.tar.gz`、`modules-xxx.tar.gz`
- `deb` 形式の更新に必要な4ファイル：`linux-image-xxx.deb`、`linux-dtb-xxx.deb`、`linux-headers-xxx.deb`、`linux-libc-dev-xxx.deb`
- 関係のないカーネルファイルを削除する必要はありません。同時に存在しても更新に影響はなく、システムは必要なファイルを正確に識別できます。デバイスがサポートするカーネル範囲内であれば、バージョンを自由に切り替えることができます（例：6.6.12 カーネルから 5.15.50 カーネルへの変更）。

`-r`/`-u`/`-b`/`-d` などのパラメータで設定したカスタムオプションは、設定ファイル `/etc/ophub-release` の関連パラメータに固定化でき、毎回手動入力する必要がなくなります。対応する設定は以下の通りです：

```shell
# カスタムパラメータの値の設定
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-b  :  KERNEL_BACKUP='yes'
-d  :  DOWNLOAD_TYPE='deb'
```

## 11. よく使うソフトウェアのインストール

Armbian システムにログイン → コマンドを入力：

```shell
armbian-software
```

`armbian-software -u` コマンドでローカルのソフトウェアセンターリストを更新できます。ユーザーからの [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) でのフィードバックに基づき、よく使う [ソフトウェア](../build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) を段階的に統合し、ワンクリックでのインストール、更新、アンインストールをサポートしています。`docker イメージ`、`デスクトップソフトウェア`、`アプリケーションサービス` などが含まれます。詳しくは [説明](armbian_software.md) を参照してください。

お住まいの国や地域に応じて、`armbian-apt` コマンドで適切なソフトウェアソースを選択し、ダウンロード速度を向上させてください。例えば、中国の清華大学ソースを選択する場合：

```shell
armbian-apt

[ STEPS ] Welcome to the Armbian source change script.
[ INFO ] Please select a [ bookworm ] mirror site.
  ┌──────┬───────────────────┬────────────────────────────────┐
  │  ID  │  Country/Region   │  Mirror Site                   │
  ├──────┼───────────────────┼────────────────────────────────┤
  │   0  │  -                │  Restore default source        │
  │   1  │  China            │  mirrors.tuna.tsinghua.edu.cn  │
  │   2  │  China            │  mirrors.bfsu.edu.cn           │
  │   3  │  China            │  mirrors.aliyun.com            │
  │   4  │  Hongkong, China  │  mirrors.xtom.hk               │
  │   5  │  Taiwan, China    │  opensource.nchc.org.tw        │
  ├──────┼───────────────────┼────────────────────────────────┤
  │   6  │  United States    │  mirrors.ocf.berkeley.edu      │
  │   7  │  United States    │  mirrors.xtom.com              │
  │   8  │  United States    │  mirrors.mit.edu               │
  │   9  │  Canada           │  mirror.csclub.uwaterloo.ca    │
  │  10  │  Canada           │  muug.ca/mirror                │
  ├──────┼───────────────────┼────────────────────────────────┤
  │  11  │  Finland          │  mirror.kumi.systems           │
  │  12  │  Netherlands      │  mirrors.xtom.nl               │
  │  13  │  Germany          │  mirrors.xtom.de               │
  │  14  │  Russia           │  mirror.yandex.ru              │
  │  15  │  India            │  in.mirror.coganng.com         │
  ├──────┼───────────────────┼────────────────────────────────┤
  │  16  │  Estonia          │  mirrors.xtom.ee               │
  │  17  │  Australia        │  mirrors.xtom.au               │
  │  18  │  South Korea      │  mirror.yuki.net.uk            │
  │  19  │  Singapore        │  mirror.sg.gs                  │
  │  20  │  Japan            │  mirrors.xtom.jp               │
  └──────┴───────────────────┴────────────────────────────────┘
[ OPTIONS ] Please Input ID: 1
[ INFO ] Your selected source ID is: [ 1 ]
[ STEPS ] Start to change the source of the system: [ mirrors.tuna.tsinghua.edu.cn ]
[ INFO ] The system release is: [ bookworm ]
[ SUCCESS ] Change the source of the system successfully.
```

## 12. よくある質問

以下は Armbian 使用時に遭遇する可能性のあるよくある質問をまとめたものです。

### 12.1 各デバイスの dtb と u-boot の対応表

サポートされるデバイスリストは、Armbian システムの設定ファイル [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) にあります。

### 12.2 LED ディスプレイ制御の説明

[説明](led_screen_display_control.md) を参照してください。

### 12.3 元の Android TV システムの復元方法

通常、`armbian-ddbr` を使用してデバイスの Android TV システムのバックアップと復元を行います。

また、ケーブルフラッシュ方式で Android システムを eMMC に書き込むこともできます。Android システムイメージは [Tools](https://github.com/ophub/kernel/releases/tag/tools) から検索できます。

#### 12.3.1 armbian-ddbr を使用したバックアップと復元

Armbian システムをインストールする前に、まずデバイスの元の Android TV システムをバックアップすることをお勧めします。後で必要な場合に復元できます。`TF/SD/USB` から Armbian システムを起動した後、`armbian-ddbr` コマンドを入力し、プロンプトに従って `b` を入力するとバックアップが実行されます。バックアップファイルは `/ddbr/BACKUP-arm-64-emmc.img.gz` に保存されます。大切に保管してください。復元が必要な場合は、バックアップファイルを `TF/SD/USB` デバイスの同じパスにアップロードし、`armbian-ddbr` コマンドを入力して、プロンプトに従って `r` を入力すると復元が実行されます。

#### 12.3.2 Amlogic フラッシュツールを使用した復元

- 通常、電源を再接続後に USB から起動できる場合は、再インストールするだけで済みます。何度か試してみてください。

- ディスプレイを接続後に画面が真っ暗で USB から起動できない場合は、デバイスのショート初期化が必要です。まずデバイスを元の Android システムに復元してから、Armbian を再度フラッシュします。まず [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) システム復旧ツールをダウンロードしてインストールします。[USB デュアルオスケーブル](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png) と [クリップ](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png) を準備します。

- x96max+ を例にすると、メインボード上の [ショートポイント](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) の位置を確認し、デバイスの [Android TV システムパッケージ](https://github.com/ophub/kernel/releases/tag/tools) をダウンロードします。その他の一般的なデバイスの Android TV システムと対応するショートポイントの図も [こちら](https://github.com/ophub/kernel/releases/tag/tools) からダウンロードして確認できます。

```shell
操作方法：

1. フラッシュソフトウェア USB Burning Tool を開く：
   [ ファイル → システムパッケージをインポート ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ 選択 ]：flash を消去
   [ 選択 ]：bootloader を消去
   [ 開始 ] ボタンをクリック
2. [ クリップ ] を使用してデバイスのメインボード上の [ 2つのショートポイントを短絡接続 ] し、
   同時に [ USB デュアルオスケーブル ] で [ デバイス ] と [ パソコン ] を接続する。
3. [ プログレスバーが動き始めた ] のを確認したら、クリップを外し、短絡を解除する。
4. [ プログレスバーが 100% ] になったらフラッシュ完了。デバイスは Android TV システムに復元されています。
   [ 停止 ] ボタンをクリックし、[ デバイス ] と [ パソコン ] の間の [ USB デュアルオスケーブル ] を抜く。
5. 上記のいずれかのステップが失敗した場合は、成功するまで再試行してください。
   プログレスバーが動かない場合は、電源を接続してみてください。通常は電源不要で、USB デュアルオスケーブルの給電のみでフラッシュ要件を満たせます。
```

工場出荷時設定へのリセットが完了すると、デバイスは Android TV システムに復元されています。後続の Armbian のインストール操作は初回インストールと同じで、元の手順に従って操作してください。

### 12.4 デバイスを USB/TF/SD から起動する設定

実際の状況に応じて、Armbian システムの初回インストールまたは再インストールの対応する方法を選択してください。

#### 12.4.1 Armbian システムの初回インストール

- システムを書き込んだ USB/TF/SD をデバイスに挿入します。
- 開発者モードを有効にする：設定 → 本機について → バージョン番号（例：X96max plus...）を素早く5回タップし、システムに `開発者モードを有効にしました` と表示されるまで続けます。
- USB デバッグモードを有効にする：システム → 詳細設定 → 開発者オプション（`USB デバッグを有効にする` を有効に設定）。`ADB` デバッグを有効にします。
- ADB ツールをインストール：[adb](https://github.com/ophub/kernel/releases/tag/tools) をダウンロードして解凍し、`adb.exe`、`AdbWinApi.dll`、`AdbWinUsbApi.dll` の3つのファイルを `c://windows/` ディレクトリ内の `system32` と `syswow64` フォルダにコピーします。`cmd` コマンドパネルを開き、`adb --version` コマンドを実行し、出力があればインストール成功です。
- `cmd` コマンドモードに入り、`adb connect 192.168.1.137` コマンドを入力します（IP はデバイスの実際の状況に合わせて変更し、ルーター管理画面で確認できます）。接続成功後 `connected to 192.168.1.137:5555` と表示されます。
- `adb shell reboot update` コマンドを入力すると、デバイスは再起動して USB/TF/SD から起動します。ブラウザでシステムの IP アドレスにアクセスするか、SSH 接続でシステムに入ることができます。

#### 12.4.2 Armbian システムの再インストール

- 通常の場合、Armbian を書き込んだ USB を USB ポートに挿入するだけで直接起動できます。USB 起動は eMMC より優先されます。
- 一部のデバイスで USB から起動できない場合は、eMMC 内の Armbian システム `/boot` ディレクトリにある `boot.scr` ファイルの名前を変更し（例：`boot.scr.bak`）、USB を挿入すると正常に起動できます。

### 12.5 赤外線レシーバーの無効化

システムはデフォルトで赤外線レシーバーのサポートが有効になっています。デバイスをサーバーとして使用する場合、誤操作によるシャットダウンを防ぐため、IR カーネルモジュールを無効にすることをお勧めします。IR を完全に無効にする方法：以下の内容を追加

```shell
blacklist meson_ir
```

`/etc/modprobe.d/blacklist.conf` に追加して再起動してください。

### 12.6 ブートファイルの選択

- 現在判明しているデバイスの中で、`T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` など少数のデバイスのみ `/bootfs/extlinux/extlinux.conf` ファイルの使用が必要で、システムにはデフォルトで追加されています。その他のデバイスで必要な場合は、システムを USB に書き込んだ後 `boot` パーティションを開き、システム付属の `/boot/extlinux/extlinux.conf.bak` ファイル名から `.bak` を削除すると使用できます。eMMC への書き込み時には、`armbian-install` が自動的にチェックし、`extlinux.conf` ファイルが存在する場合は自動的に作成します。

- その他のデバイスは `/boot/uEnv.txt` のみで起動でき、`extlinux.conf.bak` ファイルを変更しないでください。

### 12.7 ネットワーク設定

#### 12.7.1 interfaces を使用したネットワーク設定

ネットワーク設定ファイル `/etc/network/interfaces` のデフォルト内容は以下の通りです：

```shell
source /etc/network/interfaces.d/*
# Network is managed by Network manager
auto lo
iface lo inet loopback
```

##### 12.7.1.1 DHCP による動的 IP アドレスの割り当て

```shell
source /etc/network/interfaces.d/*

auto eth0
iface eth0 inet dhcp
```

##### 12.7.1.2 静的 IP アドレスの手動設定

IP、ゲートウェイ、DNS は自分のネットワーク状況に合わせて変更してください。

```shell
source /etc/network/interfaces.d/*

auto eth0
allow-hotplug eth0
iface eth0 inet static
hwaddress ether 12:34:56:78:9A:DA
address 192.168.1.100
netmask 255.255.255.0
gateway 192.168.1.1
dns-nameservers 192.168.1.1
```

##### 12.7.1.3 Docker 内で OpenWrt を使用した相互通信ネットワークの構築

MAC アドレスは必要に応じて変更してください。

```shell
source /etc/network/interfaces.d/*

allow-hotplug eth0
no-auto-down eth0
auto eth0
iface eth0 inet manual

auto macvlan
iface macvlan inet dhcp
        pre-up ip link add macvlan link eth0 type macvlan mode bridge
        post-down ip link del macvlan link eth0 type macvlan mode bridge

auto lo
iface lo inet loopback
```

#### 12.7.2 NetworkManager を使用したネットワーク管理

##### 12.7.2.1 新しいネットワーク接続の作成

新しいネットワーク接続の作成または変更前の準備作業

###### 12.7.2.1.1 ネットワークインターフェース名の取得

デバイスで利用可能なネットワークインターフェースを確認します。

```shell
nmcli device | grep -E "^[e].*|^[w].*|^[D].*|^[T].*" | awk '{printf "%-19s%-19s\n",$1,$2}'
```

コマンド実行後の返却内容：`DEVICE` 列はネットワークインターフェース名、`TYPE` 列はネットワークインターフェースタイプを表示します。

`eth0` = 1番目の有線ネットワークカード名、`eth1` = 2番目の有線ネットワークカード名、以降同様。無線ネットワークカードも同様です。

```shell
DEVICE             TYPE
eth0               ethernet
eth1               ethernet
eth2               ethernet
eth3               ethernet
wlan0              wifi
wlan1              wifi
```

###### 12.7.2.1.2 既存のネットワーク接続名の取得

デバイスの既存のネットワーク接続（有効・無効を含む）を確認します。新しい接続を作成する際は、既存の接続と異なる名前を使用することをお勧めします。

```shell
nmcli connection show | grep -E ".*|^[N].*" | awk '{printf "%-19s%-19s\n", $1,$3}'
```

コマンド実行後の返却内容：`NAME` 列は既存のネットワーク接続名、`TYPE` 列はネットワークインターフェースタイプを表示します。

`ethernet` = 有線ネットワークカード、`wifi` = 無線ネットワークカード、`bridge` = ネットワークブリッジ

```shell
NAME               TYPE
cnc                ethernet
lan                ethernet
lte                ethernet
tel                ethernet
docker0            bridge
titanium           wifi
cpe                wifi
```

###### 12.7.2.1.3 新しい有線ネットワーク接続の作成

ネットワークインターフェース `eth0` に新しいネットワーク接続を作成し、即座に有効化（`動的 IP アドレス` - `IPv4 / IPv6`）。

```shell
# Set ENV
MYCON=ether1                  # 新しいネットワーク接続名
MYETH=eth0                    # ネットワークインターフェース名 = eth0 / eth1 / eht2 / eth3
IPV6AGM=stable-privacy        # IPv6 アドレス状態モード = stable-privacy / eui64

# Add ETH
nmcli connection add \
con-name $MYCON \
type ethernet \
ifname $MYETH \
autoconnect yes \
ipv6.addr-gen-mode $IPV6AGM
nmcli connection up $MYCON
ip -c -br address
```

ネットワークインターフェース `eth0` に新しいネットワーク接続を作成し、即座に有効化（`静的 IP アドレス` - `IPv4`）。

```shell
# Set ENV
MYCON=ether1                  # ネットワーク接続名
MYETH=eth0                    # ネットワークインターフェース名 = eth0 / eth1 / eht2 / eth3
IP=192.168.67.167/24          # ホスト IP アドレス。24 はサブネットマスク（255.255.255.0 に対応）
GW=192.168.67.1               # ゲートウェイ
DNS=119.29.29.29,223.5.5.5    # DNS サーバーアドレス

# Chg CON
nmcli connection add \
con-name $MYCON \
type ethernet \
ifname $MYETH \
autoconnect yes \
ipv4.method manual \
ipv4.addresses $IP \
ipv4.gateway $GW \
ipv4.dns $DNS
nmcli connection up $MYCON
ip -c -br address
```

###### 12.7.2.1.4 新しい無線ネットワーク接続の作成

ネットワークインターフェース `wlan0` に新しいネットワーク接続を作成し、即座に有効化（`動的 IP アドレス` - `IPv4 / IPv6`）。

```shell
# Set ENV
MYCON=ssid                    # 新しいネットワーク接続名。WiFi SSID を使用して接続名を指定することを推奨
MYSSID=ssid                   # WiFi SSID（大文字小文字を区別）
MYPSWD=passwd                 # WiFi パスワード
MYWSKM=wpa-psk                # セキュリティ選択：WPA-WPA2 = wpa-psk または WPA3 = sae（無線ルーターまたは AP で暗号化方式を確認）
MYWLAN=wlan0                  # ネットワークインターフェース名 = wlan0 / wlan1
IPV6AGM=stable-privacy        # IPv6 アドレス状態モード = stable-privacy / eui64

# Add WLAN
nmcli connection add \
con-name $MYCON \
type wifi \
ifname $MYWLAN \
autoconnect yes \
ipv6.addr-gen-mode $IPV6AGM \
wifi.ssid $MYSSID \
wifi-sec.key-mgmt $MYWSKM \
wifi-sec.psk $MYPSWD
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.2 無線ネットワーク接続の WiFi SSID または PASSWD の変更

無線ネットワーク接続 `ssid` の `WiFi SSID or PASSWD` を変更し、即座に有効化。

```shell
# Set ENV
MYCON=ssid                    # 無線ネットワーク接続名
MYSSID=ssid                   # WiFi SSID（大文字小文字を区別）
MYPSWD=passwd                 # WiFi パスワード
MYWSKM=wpa-psk                # セキュリティ選択：WPA-WPA2 = wpa-psk or WPA3 = sae

# Add WLAN
nmcli connection modify $MYCON \
wifi.ssid $MYSSID \
wifi-sec.key-mgmt $MYWSKM \
wifi-sec.psk $MYPSWD
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.3 ネットワークアドレス割り当て方式の変更

###### 12.7.2.3.1 静的 IP アドレス - IPv4

ネットワーク接続 `ether1` で IP アドレスの割り当て方式を `静的 IP アドレス` に変更し、即座に有効化。

*有線接続 / 無線接続に適用

```shell
# Set ENV
MYCON=ether1                  # ネットワーク接続名
IP=192.168.67.167/24          # ホスト IP アドレス。24 はサブネットマスク（255.255.255.0 に対応）
GW=192.168.67.1               # ゲートウェイ
DNS=119.29.29.29,223.5.5.5    # DNS サーバーアドレス

# Chg CON
nmcli connection modify $MYCON \
ipv4.method manual \
ipv4.addresses $IP \
ipv4.gateway $GW \
ipv4.dns $DNS
nmcli connection up $MYCON
ip -c -br address
```

###### 12.7.2.3.2 DHCP による動的 IP アドレスの取得 - IPv4 / IPv6

ネットワーク接続 `ether1` で IP アドレスの割り当て方式を `DHCP による動的 IP アドレスの取得` に変更し、即座に有効化。

*有線接続 / 無線接続に適用

```shell
# Set ENV
MYCON=ether1                  # ネットワーク接続名

# Chg CON
nmcli connection modify $MYCON \
ipv4.method auto \
ipv6.method auto
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.4 ネットワーク接続の MAC アドレスの変更

ネットワーク接続で `MAC アドレス` を変更して、LAN 内の MAC アドレスの競合問題を解決します。

###### 12.7.2.4.1 方法1：`nmcli` コマンドで MAC アドレスを変更する

```shell
# nmcli connection show コマンドでネットワーク接続名を確認
nmcli connection show
# 返却結果にはネットワーク名が含まれます（例：'Wired connection 1'）
NAME                UUID                                  TYPE      DEVICE
Wired connection 1  24d63dc7-c46f-3bf1-912f-1c33eb94338b  ethernet  eth0
lo                  35ca24e5-bdc0-4658-8ac8-435ee22e07f3  loopback  lo
Wired connection 2  59660b21-b460-30e0-8cb3-89b886556955  ethernet  --

# 変数を設定
MYCON='Wired connection 1'    # ネットワーク接続名（ネットワークインターフェースタイプと一致させてください）
MYTYPE='802-3-ethernet'       # ネットワークインターフェースタイプ = 有線ネットワークカード / 無線ネットワークカード
                              #                              = 802-3-ethernet / 802-11-wireless
MYMAC='12:34:56:78:9A:BC'     # 新しい MAC アドレスを設定

# 変更を実行
nmcli connection modify "${MYCON}" ${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up "${MYCON}"
ip -c a show "${MYCON}"
```

* 一部のネットワークパラメータを新規作成または変更すると、ネットワーク接続が切断され、再接続される場合があります。
* ソフトウェアとハードウェアの環境の違い（デバイス、システム、ネットワーク機器等）により、有効になるまでに `1〜15` 秒程度かかります。それ以上かかる場合は、ソフトウェアとハードウェアの環境を確認することをお勧めします。

###### 12.7.2.4.2 方法2：設定ファイルで MAC アドレスを変更する

MAC アドレスの上書き設定ファイルを追加します。

```shell
sudo mkdir -p /etc/systemd/network/
sudo nano /etc/systemd/network/10-mac-override.link
```

以下の内容を追加します：

```shell
[Match]
# MAC アドレスを変更するネットワークインターフェース名を設定
OriginalName=eth0

[Link]
# 規則に準拠した MAC アドレスを設定
MACAddress=02:55:66:77:88:99
```

再起動後に有効になります。

##### 12.7.2.5 IPv6 を無効にする方法

`nmcli` ユーティリティを使用してコマンドラインで `IPv6` プロトコルを無効にできます。参照元: [disable-ipv6](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-networkmanager-to-disable-ipv6-for-a-specific-connection_configuring-and-managing-networking)。

ステップ1：`nmcli connection show` コマンドでネットワーク接続リストを確認します。返却結果は以下の通りです：

```shell
NAME                 UUID                                   TYPE       DEVICE
Wired connection 1   8a7e0151-9c66-4e6f-89ee-65bb2d64d366   ethernet   eth0
...
```

ステップ2：接続の ipv6.method パラメータを disabled に設定します：

```shell
nmcli connection modify "Wired connection 1" ipv6.method "disabled"
```

ステップ3：ネットワークに再接続します：

```shell
nmcli connection up "Wired connection 1"
```

ステップ4：ネットワーク接続状態を確認し、inet6 エントリが表示されなければ、そのデバイスで IPv6 が無効になっています：

```shell
ip address show eth0
```

ステップ5：`/proc/sys/net/ipv6/conf/eth0/disable_ipv6` ファイルに値 `1` が含まれているか確認します：

```shell
# cat /proc/sys/net/ipv6/conf/eth0/disable_ipv6
1
```

`/etc/sysctl.conf` ファイルを変更して IPv6 を無効にすることもできます：

```shell
# Disable IPv6 by default
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
```

#### 12.7.3 ワイヤレスの有効化方法

一部のデバイスは無線ネットワークをサポートしています。有効化方法は以下の通りです：

```shell
# 管理ツールのインストール
sudo apt-get install network-manager

# ネットワークデバイスの確認
sudo nmcli dev

# ワイヤレスの有効化
sudo nmcli r wifi on

# ワイヤレスのスキャン
sudo nmcli dev wifi

# ワイヤレスへの接続
sudo nmcli dev wifi connect "wifi名" password "wifiパスワード"

# 保存済みのネットワーク接続リストを表示
sudo nmcli connection show

# 接続を切断
sudo nmcli connection down "wifi名"

# 接続を削除して自動接続を解除
sudo nmcli connection delete "wifi名"
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/230541872-565a655e-2781-4170-8898-0ae096725506.png">
</div>

#### 12.7.4 Bluetooth の有効化方法

一部のデバイスは Bluetooth をサポートしています。有効化方法は以下の通りです：

```shell
# Bluetooth サポートのインストール
armbian-config >> Network >> BT: Install Bluetooth support

# システムの再起動
reboot
```

システム再起動後、Bluetooth ドライバが正常にロードされているか確認します。デスクトップシステムではメニューから Bluetooth デバイスに接続でき、ターミナルコマンドでも操作可能です。

```shell
dmesg | grep Bluetooth
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/230545883-755a137d-f574-4b32-a26b-bea9cfbf6384.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/230544120-5a63dcd4-9716-40d2-ba59-c27f7b9937f8.png">
</div>

```shell
# Bluetooth デバイスの接続
armbian-config >> Network >> BT: Discover and connect Bluetooth devices
```

ターミナルでコマンドを使用してインストールすることもできます：
```shell
# Bluetooth サービスの実行状態を確認
sudo systemctl status bluetooth

# 起動していない場合は、まず Bluetooth サービスを有効化
sudo systemctl enable bluetooth
sudo systemctl start bluetooth

# 近くの Bluetooth デバイスをスキャン
bluetoothctl scan on

# Bluetooth 発見を有効化
bluetoothctl discoverable on

# Bluetooth MAC アドレスでペアリング
bluetoothctl pair 12:34:56:78:90:AB

# ペアリング済みの Bluetooth デバイスを確認
bluetoothctl paired-devices

# Bluetooth デバイスに接続
bluetoothctl connect 12:34:56:78:90:AB

# デバイスを信頼し、以降の接続を簡単にする
bluetoothctl trust 12:34:56:78:90:AB

# Bluetooth デバイスを切断
bluetoothctl disconnect 12:34:56:78:90:AB

# Bluetooth ペアリングを解除
bluetoothctl remove 12:34:56:78:90:AB

# デバイスの接続をブロック
bluetoothctl block 12:34:56:78:90:AB
```

### 12.8 起動時のタスク追加方法

システムにはカスタム起動タスクスクリプトが組み込まれており、パスは [/etc/custom_service/start_service.sh](../build-armbian/armbian-files/common-files/etc/custom_service/start_service.sh) です。個人のニーズに応じて、このスクリプトにカスタムタスクを追加できます。

### 12.9 システム内のサービススクリプトの更新方法

`armbian-sync` コマンドを使用して、ローカルシステム内のすべてのサービススクリプトをワンクリックで最新版に更新できます。

`armbian-sync` の更新が失敗した場合、コマンドバージョンが古い可能性があります。以下の方法で手動更新できます：

```shell
wget https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-sync -O /usr/sbin/armbian-sync

chmod +x /usr/sbin/armbian-sync

armbian-sync
```

### 12.10 eMMC 上の Android システムのパーティション情報の取得方法

Armbian システムを eMMC に書き込む際、まずデバイスの Android システムのパーティションテーブルを確認し、安全な領域にデータが書き込まれることを確認して、元のパーティションテーブルを破壊してシステムが起動できなくなることを防ぐ必要があります。安全でない領域に書き込んだ場合、起動できなくなったり、以下のようなエラーが発生する可能性があります：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 パーティション情報の取得

2022.11 以降に本リポジトリからリリースされた Armbian を使用している場合、以下のコマンドをコピー＆ペーストして、完全なパーティション情報を含む URL を取得できます（デバイス自体のインターネット接続は不要です）。

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*ampart の webreport モードは 2023.02.03 にリリースされた v1.2 で導入されたものです。上記のコマンドで出力がない場合は、URL の直接出力をサポートしていない旧バージョンの可能性があります。代わりに以下のコマンドを使用してください：*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

生成される URL のフォーマットは以下のようになります：

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

この URL をブラウザで開くと、フォーマットが整った DTB パーティション情報と eMMC パーティション情報を確認できます：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 パーティション情報の共有

他の方にパーティション情報を共有する必要がある場合（本リポジトリへの新デバイスの報告やヘルプの依頼など）、スクリーンショットではなく URL を優先的に共有してください。URL が長すぎる場合は、無料の短縮 URL サービスを利用できます。

- 一方で、ウェブページ上のパーティション情報はアクセスのたびに動的に生成され、パーティションの書き込み可能マークやテーブル形式などが随時更新される可能性があります。
- 他方で、スクリーンショット内のパーティションパラメータは計算に使用するために便利にコピーできません。

また、パラメータを別途テーブルファイルに整理する必要はありません。ウェブページのテーブルレイアウトは、Excel や LibreOffice Calc に直接コピー＆ペーストでインポートできるように特別に設計されています。

#### 12.10.3 パーティション情報の解読

DTB テーブルには、Android DTB 内の各デバイス**システム**が期待するパーティションレイアウトが記録されており、通常、サイズが自動填充の `data` パーティションで終わります。したがって、同じシステム（必然的に同じモデルを含む）のデバイスの DTB パーティションレイアウトは必ず同じです。デバイス上の実際のパーティションレイアウトは eMMC の容量によって異なりますが、常に DTB パーティションレイアウトによって決定されます（つまり、DTB パーティションレイアウト + eMMC の正確なサイズがわかれば、eMMC のパーティション状況を必ず推定できます。*上記の DTB パーティション情報と eMMC パーティション情報は同じデバイスからのものではありません。お気づきでしたか？*）。

eMMC テーブルはデバイス上の実際の eMMC パーティションレイアウトです。各行は1つのストレージ領域を表し、その領域はパーティションまたはパーティション間のギャップの場合があります（Amlogic の設計決定により、各パーティション間に少なくとも 8M のギャップがあり、将来の使用を想定していましたが、S905X4 に至っても使用されておらず、スペースの無駄になっています）。パーティションに対応する行は黒い文字で、オフセットとマスク列に値があります。ギャップに対応する行は灰色の文字で、オフセットとマスク列に値がなく、パーティション名は `gap` です。

eMMC テーブルでは、各ストレージ領域の最後の列が書き込み可能性を示します。緑色で `yes` はその領域に書き込み可能、赤色で `no` はその領域に絶対に書き込み不可、黄色でマークがある場合は特定の前提条件下で書き込み可能、または一部のみ書き込み可能を意味します。

上記の表を例にすると、`bootloader` パーティションに対応する `0+4M` (`0M~4M`) は絶対に書き込み不可、その後の `32M` のギャップ（`4M~36M`）は書き込み可能、`reserved` パーティションに対応する `36M+64M` (`36M~100M`) は絶対に書き込み不可、その後のギャップから `env` 前のギャップ（`100M~836M`）まで書き込み可能、`env` の 1M 以降（`837M から末尾まで`）は Android 起動ロゴが不要な場合はすべて書き込み可能です。したがって、eMMC 上のすべての書き込み可能範囲は：

- 4M~36M
- 100M~836M
- 837M~末尾

Android 起動ロゴが必要な場合、追加で `logo` パーティションに対応する 852M + 32M (`852M~884M`) に書き込みできず、eMMC 上のすべての書き込み可能範囲は：

- 4M~36M
- 100M~836M
- 837M~852M
- 884M~末尾

#### 12.10.4 eMMC インストールへの使用

デバイスが `armbian-install` の使用時に `-a` パラメータ（[ampart](https://github.com/7Ji/ampart) を使用して eMMC パーティションレイアウトを調整）が `yes`（デフォルト値）で失敗した場合、そのデバイスは最適なパーティションレイアウトを使用できません（つまり、DTB パーティション情報を `data` のみに調整し、そこから eMMC パーティション情報を生成し、残存するすべてのパーティションを前方に移動させ、117M 以降のスペースがすべて使用可能になります）。[armbian-install](../build-armbian/armbian-files/common-files/usr/sbin/armbian-install) 内の対応するパーティション情報を変更する必要があります。

このファイルでは、パーティションレイアウトを宣言する重要なパラメータが3つあります：`BLANK1`、`BOOT`、`BLANK2`。`BLANK1` は eMMC の先頭から数えて使用不可能なサイズ、`BOOT` は `BLANK1` の後にカーネルや DTB などのファイルを格納するパーティションサイズ（256M 以上を推奨）、`BLANK2` は `BOOT` の後の使用不可能なサイズを表します。その後ろのスペースはすべて `ROOT` パーティションの作成に使用され、システム内の `/boot` マウントポイント以外のデータを格納します。3つとも整数で、単位は MiB (1 MiB = 1024 KiB = 1024^2 Byte) です。

上記の `logo` パーティションが不要な場合を例にすると、すべての利用可能なスペースを最大限活用するため、`4M~36M` の領域は小さすぎて `BOOT` として使用できないため、`BLANK1` に含めます。`100M~836M` の領域は `BOOT` として十分なので、736M すべてを `BOOT` に割り当てます。その後の `836M~837M` の使用不可領域を `BLANK2` に含め、対応するパラメータ設定は以下の通りです（`s905x3` を例として、他の SoC は対応するコードブロックを変更してください）：

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 u-boot ファイルの作成方法

u-boot はシステム起動を誘導する重要なファイルです。Amlogic、Allwinner、Rockchip デバイスではソースコードの取得とコンパイルプロセスに若干の違いがあります。

#### 12.11.1 Amlogic デバイスの u-boot ファイルの作成方法

Amlogic シリーズのデバイスメーカーの多くはオープンソースではないため、まずデバイスから u-boot 関連ファイルを抽出してからコンパイルする必要があります。以下の方法は [unifreq](https://github.com/unifreq) が共有した制作チュートリアルに基づいています。

##### 12.11.1.1 bootloader と dtb ファイルの抽出方法

抽出には HxD ソフトウェアが必要です。[公式ダウンロードリンク](https://mh-nexus.de/en/downloads.php?product=HxD20) または [バックアップダウンロードリンク](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip) から取得してインストールしてください。

`cmd` パネルで以下のコマンドを順に実行して、関連ファイルを抽出しローカルにダウンロードします。

```shell
# adb ツールでデバイスに接続
adb connect 192.168.1.111
adb shell

# bootloader のエクスポートコマンド
dd if=/dev/block/bootloader of=/data/local/bootloader.bin

# dtb のエクスポートコマンド
cat /dev/dtb >/data/local/mybox.dtb

# gpio のエクスポートコマンド
cat /sys/kernel/debug/gpio >/data/local/mybox_gpio.txt

# bootloader、dtb、gpio ファイルをローカル PC の C ドライブ直下の mybox フォルダにダウンロード
adb pull /data/local/bootloader.bin C:\mybox
adb pull /data/local/mybox.dtb C:\mybox
adb pull /data/local/mybox_gpio.txt C:\mybox
```

##### 12.11.1.2 acs.bin ファイルの作成方法

メインライン u-boot で最も重要なのは acs.bin ファイルで、メモリ初期化に使用されます。メーカー u-boot はシステム先頭の 4MB 領域にあります。先ほど取得した `bootloader.bin` から `acs.bin` ファイルを抽出します。

HxD ソフトウェアを開き、先ほどエクスポートした `bootloader.bin` ファイルを開きます。`右クリック - 範囲を選択`、開始位置 `F200`、長さ `1000`、`十六進数` を選択します。

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

選択した結果をコピーし、新しいファイルを作成、挿入式で貼り付け、警告を無視し、acs.bin として保存します。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

bootloader がロックされている場合、この領域のコードは文字化けして表示され、使用できません。正常な場合、この領域には多数の `0` が含まれ、`cfg` が数回連続して出現し、`ddr` 関連の文字が混在します。このようなコードは有効な使用可能データです。

##### 12.11.1.3 u-boot ファイルのコンパイル方法

u-boot の作成には https://github.com/unifreq/amlogic-boot-fip と https://github.com/unifreq/u-boot の2つのソースコードリポジトリが必要で、デバイスに必要な u-boot ファイルをコンパイルするために使用します。

amlogic-boot-fip ソースコードでは、各デバイスモデルで異なるのは acs.bin ファイルのみで、その他のファイルは共通です。

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

制作方法の詳細は https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic の説明を参照し、対応するデバイスモデルを選択してコンパイルとテストを行ってください。

[unifreq](https://github.com/unifreq) の方法で u-boot を作成するには、デバイスの acs.bin、dts、config ファイルが必要です。Android システムからエクスポートした dts は直接 Armbian 形式に変換できないため、対応する dts ファイルを自分で作成する必要があります。デバイスの具体的なハードウェアの違い（スイッチ、LED、電源制御、TF カード、SDIO WiFi モジュールなど）に応じて、カーネルソースリポジトリ内の類似の [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) ファイルを参考に修正してください。

X96Max Plus の u-boot 作成を例に説明します：

```shell
~/make-uboot
    ├── amlogic-boot-fip
    │   ├── x96max-plus                                     # 自分でディレクトリを作成
    │   │   ├── asc.bin                                     # 自分でソースファイルを作成
    │   │   └── other-copy-files...                         # 他のディレクトリからファイルをコピー
    │   │
    │   ├── other-source-directories...
    │   └── other-source-files...
    │
    └── u-boot
        ├── configs
        │   └── x96max-plus_defconfig                       # 自分でソースファイルを作成
        ├── arch
        │   └── arm
        │       └── dts
        │           ├── meson-sm1-x96-max-plus-u-boot.dtsi  # 自分でソースファイルを作成
        │           ├── meson-sm1-x96-max-plus.dts          # 自分でソースファイルを作成
        │           └── Makefile                            # 編集
        ├── fip
        │   ├── u-boot.bin                                  # 生成ファイル
        │   └── u-boot.bin.sd.bin                           # 生成ファイル
        ├── u-boot.bin                                      # 生成ファイル
        │
        ├── other-source-directories...
        └── other-source-files...
```

- [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) ソースコードをダウンロード。ルートディレクトリに [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) ディレクトリを作成し、自作の `asc.bin` ファイル以外は他のディレクトリからコピーできます。
- [u-boot](https://github.com/unifreq/u-boot) ソースコードをダウンロード。対応する [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) ファイルを作成して [configs](https://github.com/unifreq/u-boot/tree/master/configs) ディレクトリに配置。対応する [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) と [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) ファイルを作成して [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) ディレクトリに配置し、このディレクトリ内の [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) ファイルを編集して `meson-sm1-x96-max-plus.dtb` ファイルのインデックスを追加します。
- u-boot ソースコードのルートディレクトリに入り、ドキュメント https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst の手順に従って操作します。

最終的に2種類のファイルが生成されます：u-boot ルートディレクトリの `u-boot.bin` は `/boot` ディレクトリで使用する簡易版 u-boot（リポジトリの [overload](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) ディレクトリに対応）。`fip` ディレクトリの `u-boot.bin` と `u-boot.bin.sd.bin` は `/usr/lib/u-boot/` ディレクトリで使用する完全版 u-boot ファイル（リポジトリの [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) ディレクトリに対応）で、完全版の2つのファイルは 512 バイトの差があり、大きい方のファイルはヘッダーに 512 バイトの 0 が埋められています。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡ヒント：eMMC への書き込みテスト前に、まず 12.3 節のブリック復旧方法をお読みください。ショートポイントの位置を確認し、メーカー純正の .img 形式の Android システムファイルを準備し、ショートフラッシュの手順を成功テストしてから、書き込みテストを行ってください。

#### 12.11.2 Rockchip デバイスの u-boot ファイルの作成方法

Rockchip デバイスの大部分のメーカーは u-boot ソースコードをオープンソース化しており、メーカーのソースコードリポジトリから取得してコンパイルできます。また、一部のオープンソース貢献者が便利な u-boot コンパイルスクリプトを共有しており、以下で実例を通じていくつかのコンパイル方法を紹介します。

##### 12.11.2.1 Radxa の u-boot 作成スクリプトの使用方法

[Rock5b(rk3588)](https://wiki.radxa.com/Rock5/guide/build-u-boot-on-5b) のコンパイルを例に説明します。

```shell
# 01. 依存関係のインストール
sudo apt-get update
sudo apt-get install -y git device-tree-compiler libncurses5 libncurses5-dev build-essential libssl-dev mtools bc python dosfstools flex bison

# 02. ソースコードのクローン
mkdir ~/rk3588-sdk && cd ~/rk3588-sdk
git clone -b stable-5.10-rock5 https://github.com/radxa/u-boot.git
git clone -b master https://github.com/radxa/rkbin.git
git clone -b debian https://github.com/radxa/build.git

# ソースコードの説明：
# ~/rk3588-sdk/build/：Radxa ヘルパースクリプトファイルと、U-Boot、Linux カーネル、ルートファイルシステムの構築に使用する設定ファイル。
# ~/rk3588-sdk/rkbin/：ビルド済みの Rockchip バイナリ（第1ステージローダーと ATF（Arm Trustzone ファームウェア）を含む）。
# ~/rk3588-sdk/u-boot/：OS（Linux や Android など）を起動するための第2ステージブートローダー。

# 03. u-boot のコンパイル（For ROCK 5B）
cd ~/rk3588-sdk
./build/mk-uboot.sh rk3588-rock-5b

# 04. ビルド成功後、~/rk3588-sdk/out/u-boot ディレクトリに配置されます
~/rk3588-sdk/out/u-boot
├── idbloader.img
├── rk3588_spl_loader_v1.08.111.bin
├── spi
│   └── spi_image.img
└── u-boot.itb
```

[radxa/build](https://github.com/radxa/build) ソースコードの `board_configs.sh` と `mk-uboot.sh` にオプションを追加することで、他のデバイスの u-boot ファイルをコンパイルできます。例えば [Beelink-IPC-R(rk3588)](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415#issuecomment-1508234307) デバイスのコンパイル方法。

##### 12.11.2.2 cm9vdA の u-boot 作成スクリプトの使用方法

cm9vdA のオープンソースプロジェクト [cm9vdA/build-linux](https://github.com/cm9vdA/build-linux) では、u-boot とカーネルのコンパイルスクリプトと使用方法が提供されています。以下は一部の Rockchip デバイスの u-boot コンパイルのプロセス記録を参考として抜粋したものです。

- Lenovo-Leez-P710(rk3399) デバイスの u-boot コンパイル：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609#issuecomment-1681494735)
- DLFR100(rk3399) デバイスの u-boot コンパイル：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522#issuecomment-1622919423)
- ZYSJ(rk3399) デバイスの u-boot コンパイル：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380#issuecomment-1539325464)


### 12.12 メモリサイズの誤認識

メモリサイズが正しく認識されない場合（4G メモリが 1-2G として認識されるのは異常、3.7G として認識されるのは正常）、`/boot/UBOOT_OVERLOAD` ファイルを手動でコピーして試してみてください（注意：`コピー`であり`リネーム`ではありません。リネームするとインストールや更新操作時に起動できなくなります）：USB で使用する場合は `/boot/u-boot.ext` として保存、eMMC で使用する場合は `/boot/u-boot.emmc` として保存します。

メモリ認識問題のトラブルシューティング以外では、u-boot ファイルを手動でコピーしないでください。誤った操作は起動不能やその他の問題を引き起こす可能性があります。

### 12.13 dtb ファイルの逆コンパイル方法

一部の新しいデバイスは現在のサポートリストに含まれていない場合（またはハードウェアの違いがある場合）があり、dtb ファイルを逆コンパイルして関連パラメータを調整し、適合を試みることができます。

```shell
# 依存関係のインストール
sudo apt-get update
sudo apt-get install -y device-tree-compiler

# 1. 逆コンパイルコマンド（dtb ファイルから dts ソースコードを生成）
dtc -I dtb -O dts -o xxx.dts xxx.dtb

# 2. コンパイルコマンド（dts からコンパイルして dtb ファイルを生成）
dtc -I dts -O dtb -o xxx.dtb xxx.dts

# 3. データを保存して再起動
sync && reboot

# 4. [任意アクション] 必要に応じてテスト
# 例えば 12.16 で紹介した問題の解決時に、再インストールしてテスト
armbian-install
```

### 12.14 cmdline 設定の変更方法

Amlogic デバイスは `/boot/uEnv.txt` ファイルで設定します。Rockchip と Allwinner デバイスは `/boot/armbianEnv.txt` ファイルで設定します（`extraargs` または `extraboardargs` パラメータに追加）。`/boot/extlinux/extlinux.conf` を使用するデバイスはそのファイルで設定します。変更のたびに再起動が必要です。

- 例えば `Home Assistant Supervisor` は `docker cgroup v1` のみをサポートしていますが、現在の docker はデフォルトで v2 バージョンをインストールします。v1 に切り替える場合は、cmdline に `systemd.unified_cgroup_hierarchy=0` パラメータを追加し、再起動すると `docker cgroup v1` に切り替わります。

- cmdline に `mmc_core.max_freq=50000000` 設定を追加することで、eMMC の最大周波数を `50MHz` に制限できます。一部の S905L2 ボックスでは、高周波（HS200/HS400、100MHz以上）において eMMC が不安定になることがありますが、周波数を下げることで、起動失敗やランダムなクラッシュ、読み書きエラー、不安定性などの問題を解決できます。

- cmdline に `max_loop=128` 設定を追加することで、許可される loop マウント数を調整できます。

- cmdline に `usbcore.usbfs_memory_mb=1024` 設定を追加することで、USBFS メモリバッファをデフォルトの `16 mb` からより大きなサイズに永続的に変更できます（`cat /sys/module/usbcore/parameters/usbfs_memory_mb`）。USB での大容量ファイル転送能力を向上させます。

- cmdline に `usbcore.usb3_disable=1` 設定を追加することで、すべての USB 3.0 デバイスを無効にできます。

- cmdline に `extraargs=video=HDMI-A-1:1920x1080@60` 設定を追加することで、ビデオ表示モードを 1080p に強制できます。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a600dcad-d817-47eb-b529-4014019915b3">
</div>

### 12.15 新しいサポートデバイスの追加方法

デバイス用の Armbian システムを構築するには、`デバイス設定ファイル`、`システムファイル`、`u-boot ファイル`、`フロー制御ファイル` の4つの部分が必要です。具体的な追加方法は以下の通りです：

#### 12.15.1 デバイス設定ファイルの追加

設定ファイル [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) に、デバイスのテスト結果に基づいて対応する設定情報を追加します。`BUILD` 値が `yes` のデバイスはデフォルトで構築されるデバイスで、対応する `BOARD` 値は`一意でなければなりません`。これらのデバイスはデフォルト構築の Armbian システムを直接使用できます。

デフォルト値が `no` のデバイスはデフォルトの構築に含まれておらず、使用する際は同じ `FAMILY` の Armbian システムをダウンロードする必要があります。`USB` に書き込んだ後、パソコンで `USB の boot パーティション` を開き、`/boot/uEnv.txt` ファイル内の `FDT dtb 名` を変更することで、リスト内の他のデバイスに対応させることができます。

#### 12.15.2 システムファイルの追加

共通ファイルは `build-armbian/armbian-files/common-files` ディレクトリに配置し、各プラットフォーム共通です。

プラットフォームファイルはそれぞれ `build-armbian/armbian-files/platform-files/<platform>` ディレクトリに配置します。[Amlogic](../build-armbian/armbian-files/platform-files/amlogic)、[Rockchip](../build-armbian/armbian-files/platform-files/rockchip)、[Allwinner](../build-armbian/armbian-files/platform-files/allwinner) はそれぞれプラットフォームファイルを共有します。`bootfs` ディレクトリには /boot パーティションファイルを格納し、`rootfs` ディレクトリには Armbian システムファイルを格納します。

個別のデバイスに特殊な設定ニーズがある場合、`build-armbian/armbian-files/different-files` ディレクトリに `BOARD` 名の独立ディレクトリを作成し、必要に応じて `bootfs` ディレクトリを作成して `/boot` パーティションファイルを追加するか、`rootfs` ディレクトリを作成してシステムファイルを追加します。ディレクトリ構造は Armbian システム内の実際のパスに準じ、新しいファイルの追加や共通ファイルおよびプラットフォームファイル内の同名ファイルの上書きに使用できます。

#### 12.15.3 u-boot ファイルの追加

`Amlogic` シリーズのデバイスは [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) ファイルと [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) ファイルを共有し、新しいファイルはそれぞれ対応するディレクトリに配置します。`bootloader` ファイルはシステム構築時に自動的に Armbian システムの `/usr/lib/u-boot` ディレクトリに追加され、`u-boot` ファイルは自動的に `/boot` ディレクトリに追加されます。

`Rockchip` と `Allwinner` シリーズのデバイスは、各デバイスに `BOARD` 名の独立した [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot) ファイルディレクトリを追加し、対応するシリーズファイルをこのディレクトリに配置します。

Armbian イメージの構築時に、これらの u-boot ファイルは [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) の設定に基づいて、rebuild スクリプトにより対応する Armbian イメージファイルに書き込まれます。

標準の U-Boot ファイルが使用できるデバイスは、まず直接使用することをお勧めします。一部のデバイスは適切な U-Boot をコンパイルまたは取得できない場合がありますが、そのようなデバイスが Ubuntu などの Linux システムを正常に実行できる場合は、ブート関連の重要なパーティションを保持して Armbian または OpenWrt をインストールすることを試みることができます。通常、保持が必要な重要なパーティションには `bootloader`、`reserved`、`env` が含まれます。

これらのパーティションはバックアップした後、新しい Armbian または OpenWrt イメージを作成する際に対応する位置に書き戻すことができます。元のシステムのブートパーティションを含む作成済みイメージは、`dd` コマンドで eMMC に直接書き込むか、対応するシステムの内蔵ツールを使用してインストールできます。例えば Armbian の `armbian-install` コマンドや、OpenWrt の `晶晨宝盒` プラグインなどです。

現在この方法を採用しているデバイスには [oes(a311d)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666)、[oes-plus(s922x)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029)、[oec-turbo(rk3566)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736) があります。以下では `oes(a311d)` デバイスを例に具体的な操作手順を説明します。

##### 12.15.3.1 パーティション情報レイアウトの確認

```shell
ampart /dev/mmcblk2

# 取得したパーティション情報は以下の通りです：
# ===================================================================================
# IO seek EPT: Seeking to 37748736
# EPT report: 20 partitions in the table:
# ===================================================================================
# ID| name            |          offset|(   human)|            size|(   human)| masks
# -----------------------------------------------------------------------------------
#  0: bootloader                      0 (   0.00B)           400000 (   4.00M)      0
#     (GAP)                                                 2000000 (  32.00M)
#  1: reserved                  2400000 (  36.00M)          4000000 (  64.00M)      0
#     (GAP)                                                  800000 (   8.00M)
#  2: cache                     6c00000 ( 108.00M)         20000000 ( 512.00M)      2
#     (GAP)                                                  800000 (   8.00M)
#  3: env                      27400000 ( 628.00M)           800000 (   8.00M)      0
#     (GAP)                                                  800000 (   8.00M)
#  ... other partitions ...
# ===================================================================================
```

##### 12.15.3.2 重要なパーティションのバックアップ

```shell
dd if=/dev/mmcblk2 of=bootloader.bin bs=1M count=4 skip=0
dd if=/dev/mmcblk2 of=reserved.bin bs=1M count=8 skip=36
dd if=/dev/mmcblk2 of=env.bin bs=1M count=1 skip=628
```

バックアップファイルは [u-boot](https://github.com/ophub/u-boot) リポジトリの対応するディレクトリ [u-boot/amlogic/bootloader/a311d-oes](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader/a311d-oes) に配置します。

reserved パーティションは 64MB ですが、8MB のみバックアップしていることに注意してください。理由は `oes(a311d)` デバイスでは `reserved` パーティションの最初の 8MB が重要なデータで、残りの 56MB は空白でバックアップ不要だからです。具体的な確認方法：

```shell
# まず、reserved パーティションの完全な 64MB サイズのファイルをバックアップ：
dd if=/dev/mmcblk2 of=reserved_64M.bin bs=1M count=64 skip=36

# 次に、バックアップした reserved_64M.bin ファイルの最初の 8MB を抽出：
dd if=reserved_64M.bin of=reserved_first_8M.bin bs=1M count=8

# 十六進数の内容を確認：
hexdump -C reserved_first_8M.bin | less

# 返却結果の最後の数行の内容を確認：
0071ffd0  4c 5e a8 1f fc 5b 5b 98  ae ef b0 97 0c 3b e8 c2  |L^...[[......;..|
0071ffe0  c8 e0 b2 74 3d 67 d5 3d  24 7b 63 b7 c7 73 f5 d8  |...t=g.=${c..s..|
0071fff0  a1 b8 38 a7 57 d6 b4 b5  e8 1c ba c0 07 0f f5 79  |..8.W..........y|
00720000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00800000
```

出力結果を分析すると、非ゼロデータを含む最後の行のアドレスは `0071fff0` です。アドレス `00720000` 以降はすべて `00`（ゼロ）になっています。hexdump ツールは（`*`）を使用して繰り返し行を表示し、これは `00720000` から `00800000`（8MB の末尾）までがすべてゼロであることを意味します。有効データアドレス `0x00720000` を十進数に変換すると `7,471,104` バイト、つまり `7471104 / 1024 / 1024 = 7.125 MB` のサイズです。したがって切り上げて 8MB をバックアップすれば十分です。env パーティションも同様で、最初の 80KB のみが有効データで、残りは空白のため、1MB のみバックアップします。

##### 12.15.3.3 特殊パーティション書き込みファイルの追加

具体的な実装の詳細については、ファイル [/etc/armbian-board-release.conf](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/build-armbian/armbian-files/different-files/a311d-oes/rootfs/etc/armbian-board-release.conf) で定義されている `write_board_bootloader` 関数を参照してください。この関数はイメージの再構築（rebuild）プロセス中に呼び出されます。また、この設定ファイルは強力なデバイスカスタマイズセンターでもあります。`skip_mb="700"` などのパラメータでイメージパーティションのレイアウトとサイズを精密に制御できるだけでなく、カスタムスクリプトを追加してカーネルやその他のシステムファイルの特殊処理を実現することもできます。特定デバイスに対するすべての高度なカスタマイズ操作はこのファイルに集中して統一管理され、設定が明確で効率的になります。

#### 12.15.4 フロー制御ファイルの追加

[yml ワークフロー制御ファイル](../.github/workflows/build-armbian-arm64-server-image.yml) の `armbian_board` に対応する `BOARD` オプションを追加し、GitHub Actions での使用をサポートさせます。

### 12.16 eMMC への書き込み時の I/O エラーの解決方法

一部のデバイスは USB/SD/TF から Armbian を正常に起動できますが、eMMC に書き込む際に I/O 書き込みエラーが発生します。例えば [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989) のケースでは、以下のようなエラーが報告されています：

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

この場合、dtb の動作モード速度と周波数を調整してストレージの読み書きを安定させることができます。SDR モードでは周波数は速度の2倍、DDR モードでは周波数は速度と等しくなります。利用可能なモードは以下の通りです：

```shell
sd-uhs-sdr12
sd-uhs-sdr25
sd-uhs-sdr50
sd-uhs-ddr50
sd-uhs-sdr104

max-frequency = <208000000>;
```

カーネルソースコードの [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) ファイル内のコードスニペットを例に：

```shell
/* SD card */
&sd_emmc_b {
	status = "okay";

	bus-width = <4>;
	cap-sd-highspeed;
	sd-uhs-sdr12;
	sd-uhs-sdr25;
	sd-uhs-sdr50;
	max-frequency = <100000000>;
};

/* eMMC */
&sd_emmc_c {
	status = "okay";

	bus-width = <8>;
	cap-mmc-highspeed;
	max-frequency = <100000000>;
};
```

通常、`&sd_emmc_c` の周波数を `max-frequency = <200000000>;` から `max-frequency = <100000000>;` に下げれば解決します。効果がない場合は `50000000` にさらに下げてテストし、同時に `&sd_emmc_b` の設定で `USB/SD/TF` を調整するか、`sd-uhs-sdr` で速度制限することもできます。dts ファイルを修正して [コンパイル](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) してテストファイルを生成するか、`12.13 節` の方法で既存の dtb ファイルを逆コンパイルして修正できます。dtb ファイルの逆コンパイル時は十六進数値を使用します。十進数の `200000000` に対応する十六進数は `0xbebc200`、十進数の `100000000` に対応する十六進数は `0x5f5e100`、十進数の `50000000` に対応する十六進数は `0x2faf080`、十進数の `25000000` に対応する十六進数は `0x17d7840` です。また、`12.14 節` の cmdline 設定方法を参照し、cmdline に `mmc_core.max_freq=50000000` パラメータを追加して eMMC の最大周波数を制限することで、読み書きエラーや不安定な問題を解決することも可能です。

ソフトウェア面の最適化の他に、[ハードウェアアップグレード](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) や [手動改造](https://www.right.com.cn/forum/thread-901586-1-1.html) で解決することもできます。

### 12.17 Bullseye バージョンで音が出ない問題の解決方法

音の問題のエラーログ情報：

```shell
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: dpcm_fe_dai_prepare() failed (-22)
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: no backend DAIs enabled for fe.dai-link-0
```

[Bullseye NO Sound](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1000) の方法を参照して設定してください。

```shell
curl -fsSOL https://github.com/ophub/kernel/releases/download/tools/bullseye_g12_sound-khadas-utils-4-2-any.tar.gz
tar -xzf bullseye_g12_sound-khadas-utils-4-2-any.tar.gz -C /

systemctl enable sound.service
systemctl restart sound.service
```

Armbian を再起動してテストします。音がまだ出ない場合、デバイスが旧版の conf に対応するオーディオ出力パスを使用している可能性があります。/usr/bin/g12_sound.sh の `L137-L142` の新しい設定をコメントアウトし（この設定は主に G12B/S922X に適用され、旧版の G12A/S905X2 および G12A ベースの SM1/S905X3 の多くは互換性がありません）、`L130-L134` の旧設定のコメントを解除する必要があります。

### 12.18 boot.scr ファイルのコンパイル方法

Armbian システムの `/boot` ディレクトリにある `boot.scr` はシステムブートファイルで、`boot.cmd` ソースファイルからコンパイルして生成されます。`boot.cmd` を修正した後、`mkimage` コマンドで再コンパイルすると新しい `boot.scr` が生成されます。

通常、これら2つのファイルを修正する必要はありませんが、調整が必要な場合は以下の方法を参照してください。

```shell
# 依存関係のインストール
sudo apt-get update
sudo apt-get install -y u-boot-tools

# boot.cmd ファイルの編集
cd /boot
copy /boot/boot.cmd /boot/boot.cmd.bak
copy /boot/boot.scr /boot/boot.scr.bak
nano boot.cmd

# コンパイルコマンド
mkimage -C none -A arm -T script -d boot.cmd boot.scr

# 再起動してテスト
sync
reboot

# 補足説明
# Amlogic デバイスでは、USB で使用するのは /boot/boot.scr ファイルで、eMMC に書き込む際は /boot/boot-emmc.scr ファイルが使用されます。
```

### 12.19 リモートデスクトップの有効化とデフォルトポートの変更方法

ソフトウェアセンター `armbian-software` で `201` を選択してデスクトップをインストールします。インストール中にリモートデスクトップを有効にするか尋ねられるので、`y` と入力して有効にします。リモートデスクトップのデフォルトポートは `3389` で、必要に応じてカスタムポートを設定できます：

```shell
sudo nano /etc/xrdp/xrdp.ini
# カスタムポートに変更（例：5000）
port=5000
```

### 12.20 TCP 輻輳制御の最適化方法

異なる性能のデバイスに対して、最適な体験を得るためにネットワークスタック設定を差別化することをお勧めします。デバイスの実際の状況に応じて、設定ファイル `/etc/sysctl.conf` を編集し、以下の2行を変更してください：
- ギガビットデバイス（高性能/モダンアーキテクチャ）：`fq + bbr` の組み合わせを推奨し、スループットを最大化しパケットロスへの耐性を向上させます。
- 100Mbps デバイス（低性能/レガシーアーキテクチャ）：`fq_codel + cubic` の組み合わせを推奨し、CPU 負荷を低減し低遅延の安定性を確保します。

```shell
# プラン A：ギガビットデバイス/高性能推奨 (Gigabit/High Performance)
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr

# プラン B：100Mbps デバイス/低性能推奨 (100M/Low Performance)
# net.core.default_qdisc = fq_codel
# net.ipv4.tcp_congestion_control = cubic
```
