# カーネルのコンパイルと使用ガイド

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

本プロジェクトでコンパイルしたカーネルは `Armbian` と `OpenWrt` システムに対応しており、[amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian)、[amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt)、[flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions)、[unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) などのプロジェクトで使用できます。ファームウェアのコンパイル時に統合することも、既存のシステムにインストールして使用することもできます。

必要に応じてカーネル設定を調整し、ドライバやパッチを追加できます。また、`5.10.95-happy-new-year`、`5.10.96-beijing-winter-olympics`、`5.10.99-valentines-day` など、個性的な署名付きカーネルをコンパイルすることもできます。

## カーネルリポジトリ

[ophub/kernel](https://github.com/ophub/kernel) の [Releases](https://github.com/ophub/kernel/releases) にコンパイル済みのカーネルファイルが提供されています。

## ローカルコンパイル

- ### Ubuntu システムでの実行

1. リポジトリをローカルにクローン：`git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. 必要なパッケージのインストール（Ubuntu 24.04 を例として）：

`~/amlogic-s9xxx-armbian` ルートディレクトリに移動し、インストールコマンドを実行します：

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-24.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. `~/amlogic-s9xxx-armbian` ルートディレクトリに移動し、`sudo ./recompile -k 5.15.100` などの指定パラメータコマンドを実行してカーネルをコンパイルします。スクリプトはコンパイル環境とカーネルソースコードを自動的にダウンロード・インストールし、すべての設定を完了します。コンパイル済みのカーネルファイルは現在のソースツリーの `compile-kernel/output` ディレクトリ、または `-h` パラメータで指定したディレクトリに保存されます。

- ### Armbian システムでの実行

[Armbian](https://github.com/ophub/amlogic-s9xxx-armbian/releases) システム上で直接カーネルをコンパイルすることも、Ubuntu/Debian などのシステム上で [Docker](https://hub.docker.com/u/ophub) コンテナを介して Armbian システムを実行してカーネルをコンパイルすることもできます。Armbian システムの Docker イメージの作成方法は [armbian_docker](./tools/script/docker) ビルドスクリプトを参照してください。

1. ローカルコンパイル環境と設定ファイルの更新：`armbian-kernel -u`

2. カーネルのコンパイル：`armbian-kernel -k 5.15.100` などの指定パラメータコマンドを実行してカーネルをコンパイルします。スクリプトはコンパイル環境とカーネルソースコードを自動的にダウンロード・インストールし、すべての設定を完了します。コンパイル済みのカーネルファイルは `/opt/kernel/compile-kernel/output` ディレクトリに保存されます。

- ### ローカルコンパイルパラメータの説明

| パラメータ | 意味          | 説明                             |
| --------- | ------------ | ------------------------------- |
| -r     | Repository  | カーネルソースコードリポジトリを指定します。`github.com` 上のカーネルソースコードリポジトリを選択でき、例えば `-r unifreq`。パラメータ形式は `owner/repo@branch` の3部分の組み合わせをサポートし、オーナー名 `owner` は必須、リポジトリ名 `/repo` とブランチ名 `@branch` は任意です。オーナー名のみ指定した場合、そのオーナー配下で `linux-5.x.y` 形式の名前かつ `main` ブランチのカーネルソースコードリポジトリを自動的にマッチします。リポジトリ名やブランチ名が異なる場合は、`owner@branch`、`owner/repo`、`owner/repo@branch` のように組み合わせて指定してください。デフォルト値：`unifreq` |
| -k     | Kernel      | カーネルバージョンを指定します。例：`-k 5.15.100`。複数のカーネルは `_` で区切ります。例：`-k 5.15.100_5.15.50`。`-k all` を使用するとすべてのメインラインカーネルをコンパイルし、現在は `-k 5.10.y_5.15.y_6.1.y_6.6.y_6.12.y_6.18.y` と同等です。カーネルリストは上流カーネルソースリポジトリ [unifreq](https://github.com/unifreq) のメンテナンス状況に応じて動的に調整されます。 |
| -a     | AutoKernel  | 同系列の最新バージョンカーネルを自動的に採用するかどうかを設定します。`true` に設定すると、`-k` で指定したカーネル（例：`5.15.100`）と同じ系列にさらに新しいバージョンがあるか自動的にチェックし、存在する場合は最新版に自動切り替えします。`false` に設定すると指定バージョンをコンパイルします。デフォルト値：`true` |
| -m     | MakePackage | カーネルビルドのパッケージリストを設定します。`all` に設定すると `Image、modules、dtbs` のすべてのファイルを生成し、`dtbs` に設定すると DTB ファイル3つのみ生成します。デフォルト値：`all` |
| -f     | configFlavor | カーネルリポジトリ [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) から設定ファイル `config-*` をローカルの [tools/config](tools/config) ディレクトリにダウンロードする際のフレーバーを指定します。対応するカーネルバージョンの設定ファイルがローカルに既に存在し、このパラメータが未設定の場合はダウンロードをスキップします。選択肢はカーネルリポジトリに存在する[ディレクトリ名](https://github.com/ophub/kernel/tree/main/kernel-config/release)で、例：`stable` / `rk3588` / `rk35xx` / `h6`。デフォルト値：`stable` |
| -p     | AutoPatch   | カスタムカーネルパッチを適用するかどうかを設定します。`true` に設定すると [tools/patch](tools/patch) ディレクトリのカーネルパッチを使用します。詳細は[カーネルパッチの追加方法](../documents/README.ja.md#9-armbian-カーネルのコンパイル)を参照してください。デフォルト値：`false` |
| -n     | CustomName  | カーネルのカスタム署名を設定します。例えば `-ophub` に設定すると、生成されるカーネル名は `5.15.100-ophub` になります。署名にスペースを含めないでください。デフォルト値：`-ophub` |
| -t     | Toolchain   | カーネルコンパイルのツールチェーンを設定します。選択肢：`clang / gcc / gcc-<version>`。デフォルト値：`gcc` |
| -z     | CompressFormat | カーネル内の initrd で使用する圧縮フォーマットを設定します。選択肢：`xz / gzip / zstd / lzma`。デフォルト値：`xz` |
| -d     | DeleteSource | カーネルのコンパイル完了後にソースコードを削除するかどうかを設定します。選択肢：`true / false`。デフォルト値：`false` |
| -s     | SilentLog   | コンパイル時にサイレントモードを有効にしてログ出力を削減するかどうかを設定します。選択肢：`true / false`。デフォルト値：`false` |
| -l     | EnableLog   | コンパイルプロセスをログファイルに記録するかどうかを設定します：`/var/log/kernel_compile_*.log`。選択肢：`true / false`。デフォルト値：`false` |
| -c     | CcacheClear | コンパイル前に ccache キャッシュをクリアするかどうかを設定します。選択肢：`true / false`。デフォルト値：`false` |
| -h     | DockerHostpath | カーネルコンパイル時の Docker コンテナのホストマシン上のマウントパスを設定します。デフォルトは現在のディレクトリを使用します。 |
| -i     | DockerImage | カーネルコンパイル用の Docker コンテナイメージを設定します。デフォルト値：`ophub/armbian-trixie:arm64` |

- `sudo ./recompile` ：デフォルト設定でカーネルをコンパイルします。
- `sudo ./recompile -k 5.15.100` ：デフォルト設定を使用し、`-k` でコンパイルするカーネルバージョンを指定します。複数バージョンを同時にコンパイルする場合は `_` で連結します。
- `sudo ./recompile -k 5.15.100 -f stable` ：デフォルト設定を使用し、`-k 5.15.100` でカーネルバージョンを指定、`-f stable` でカーネルリポジトリの `stable` ディレクトリから設定ファイルをダウンロードします。
- `sudo ./recompile -k 5.15.100 -a true` ：デフォルト設定を使用し、`-a` パラメータでカーネルコンパイル時に同系列の最新カーネルへ自動アップグレードするかどうかを設定します。
- `sudo ./recompile -k 5.15.100 -n -ophub` ：デフォルト設定を使用し、`-n` パラメータでカーネルのカスタム署名を設定します。
- `sudo ./recompile -k 5.15.100 -m dtbs` ：デフォルト設定を使用し、`-m` パラメータで dtbs ファイルのみを生成するよう指定します。
- `sudo ./recompile -k 5.15.100_6.1.10 -a true -n -ophub` ：デフォルト設定を使用し、複数のパラメータで設定を行います。

💡 ヒント：`unifreq` の [linux-6.1.y](https://github.com/unifreq/linux-6.1.y)、[linux-5.15.y](https://github.com/unifreq/linux-5.15.y)、[linux-5.10.y](https://github.com/unifreq/linux-5.10.y) などのリポジトリのカーネルソースコードを使用してコンパイルすることをお勧めします。これらには対象デバイス向けのドライバとパッチが追加されています。設定ファイルは [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) のテンプレートの使用を推奨します。サポートデバイス向けに事前設定されており、これをベースにカスタマイズできます。

## GitHub Actions でのカーネルコンパイル

1. [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) ページで **_`Compile the kernel`_** を選択し、**_`Run workflow`_** ボタンをクリックするとコンパイルを開始できます。

2. テンプレート [compile-kernel-via-docker.yml](../.github/workflows/compile-kernel-via-docker.yml) を参照してください。コードは以下の通りです：

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.12.y_6.18.y
    kernel_auto: true
    kernel_sign: -yourname
```

💡 注意：リポジトリを `fork` して変更を加えた場合、使用時に Actions のユーザー名を自分のリポジトリに変更する必要があります。例：

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### GitHub Action 入力パラメータの説明

関連パラメータは`ローカルコンパイルコマンド`に対応しています。上記の説明を参照してください。

| パラメータ           | デフォルト値       | 説明                                                      |
|-------------------|------------------|-----------------------------------------------------------|
| build_target      | kernel           | 固定パラメータ `kernel`。コンパイル対象をカーネルに設定します。     |
| kernel_source     | unifreq          | カーネルソースコードリポジトリを指定します。デフォルト値：`unifreq`。機能は `-r` を参照 |
| kernel_version    | 6.12.y_6.18.y    | カーネルバージョンを指定します。例：`5.15.100`。機能は `-k` を参照 |
| kernel_auto       | true             | 同系列の最新バージョンカーネルを自動採用するかどうかを設定します。デフォルト値：`true`。機能は `-a` を参照 |
| kernel_package    | all              | カーネルビルドのパッケージリストを設定します。デフォルト値：`all`。機能は `-m` を参照 |
| kernel_sign       | -ophub           | カーネルのカスタム署名を設定します。デフォルト値：`-ophub`。機能は `-n` を参照 |
| kernel_toolchain  | gcc              | カーネルコンパイルのツールチェーンを設定します。デフォルト値：`gcc`。機能は `-t` を参照 |
| config_flavor     | stable           | カーネルリポジトリ [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) から設定ファイル `config-*` をローカルの [tools/config](tools/config) ディレクトリにダウンロードする際のフレーバーを指定します。対応するカーネルバージョンの設定ファイルがローカルに既に存在し、このパラメータが未設定の場合はダウンロードをスキップします。選択肢はカーネルリポジトリに存在する[ディレクトリ名](https://github.com/ophub/kernel/tree/main/kernel-config/release)で、例：`stable` / `rk3588` / `rk35xx` / `h6`。デフォルト値：`stable`。`kernel_config` パラメータも同時に設定した場合、最終的には `config_flavor` で指定した設定ファイルが優先されます。この2つのパラメータは排他的で、どちらか一方を選択できます：前者で [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) のカーネル設定ファイルを使用するか、後者で独自リポジトリの設定ファイルを使用します。機能は `-f` を参照 |
| kernel_config     | false            | デフォルトでは [tools/config](tools/config) ディレクトリの設定テンプレートを使用します。リポジトリ内のカーネル設定ファイルが保存されているディレクトリを指定できます。例：`kernel/config_path`。このディレクトリの設定テンプレートはカーネルメジャーバージョンで命名する必要があります。例：`config-5.10`、`config-5.15` など。 |
| kernel_patch      | false            | リポジトリ内のカスタムカーネルパッチファイルのディレクトリを指定します。このパラメータを設定すると、コンパイル前に指定ディレクトリからパッチファイルを自動ダウンロードします。未設定の場合はスキップします。 |
| auto_patch        | false            | カスタムカーネルパッチを適用するかどうかを設定します。デフォルト値：`false`。機能は `-p` を参照 |
| compress_format   | xz               | カーネル内の initrd で使用する圧縮フォーマットを設定します。デフォルト値：`xz`。機能は `-z` を参照 |
| delete_source     | false            | コンパイル完了後にカーネルソースコードを削除するかどうかを設定します。デフォルト値：`false`。機能は `-d` を参照 |
| silent_log        | false            | コンパイル時にサイレントモードを有効にしてログ出力を削減するかどうかを設定します。デフォルト値：`false`。機能は `-s` を参照 |
| enable_log        | false            | コンパイルプロセスをログファイルに記録するかどうかを設定します：`/var/log/kernel_compile_*.log`。デフォルト値：`false`。機能は `-l` を参照 |
| ccache_clear      | false            | コンパイル前に ccache キャッシュをクリアするかどうかを設定します。デフォルト値：`false`。機能は `-c` を参照 |
| docker_hostpath   | .                | カーネルコンパイル時の Docker コンテナのホストマシン上のマウントパスを設定します。デフォルトは現在のディレクトリ。機能は `-h` を参照 |
| docker_image      | ophub/armbian-trixie:arm64 | カーネルコンパイル用の Docker コンテナイメージを設定します。機能は `-i` を参照 |

- ### GitHub Action 出力変数の説明

`Releases` へのアップロードには、リポジトリに `Workflow の読み書き権限` を設定する必要があります。詳細は[使用説明](../documents/README.ja.md#2-プライバシー変数-github_token-等の設定)を参照してください。

| パラメータ                         | デフォルト値            | 説明                           |
| -------------------------------- | --------------------- | ------------------------------ |
| ${{ env.PACKAGED_OUTPUTTAGS }}   | 6.12.y_6.18.y         | コンパイル済みカーネル名          |
| ${{ env.PACKAGED_OUTPUTPATH }}   | compile-kernel/output | コンパイル済みカーネルファイルのディレクトリ |
| ${{ env.PACKAGED_OUTPUTDATE }}   | 04.13.1058            | コンパイル日付（月.日.時分）       |
| ${{ env.PACKAGED_STATUS }}       | success               | コンパイル状態：success / failure |

## カーネル使用ガイド

本プロジェクトでコンパイルしたカーネルは `Armbian` と `OpenWrt` システムに対応しています。以下では ophub のプロジェクトを例に説明します。

### Armbian システムでの使用

以下では、Armbian ファームウェアのコンパイル時にカーネルを統合する方法と、既存のシステムにカーネルをインストールする方法をそれぞれ紹介します。

- #### カーネルを使用した Armbian ファームウェアのコンパイル

Armbian ファームウェアのコンパイルはローカル操作と github.com の Actions を使用したオンラインコンパイルの両方をサポートしています。ローカルコンパイル方法の詳細は[ローカルパッケージング](../README.ja.md#ローカルパッケージング)を、Actions を使用したオンラインコンパイル方法の詳細は[GitHub Actions でのコンパイル](../README.ja.md#github-actionsを使用したコンパイル)を参照してください。

- #### 既存の Armbian システムへのカーネルインストール

`armbian-update` コマンドを使用して、コンパイル済みのカーネルを既存の Armbian システムにインストールできます。具体的な操作方法の詳細は[Armbian カーネルの更新](../README.ja.md#armbianカーネルの更新)を参照してください。

- #### カスタムドライバモジュールのコンパイル

Linux メインラインカーネルでは一部のドライバがまだサポートされていないため、必要なドライバモジュールを自分でコンパイルできます。具体的な操作方法の詳細は[ドライバモジュールのコンパイル](../documents/README.ja.md#93-カスタムドライバモジュールのコンパイル方法)を参照してください。

### OpenWrt システムでの使用

以下では、OpenWrt システムでファームウェアをコンパイルする際にカーネルを統合する方法と、既存のシステムにカーネルをインストールする方法をそれぞれ紹介します。

- #### カーネルを使用した OpenWrt ファームウェアのコンパイル

OpenWrt ファームウェアのコンパイルはローカル操作と github.com の Actions を使用したオンラインコンパイルの両方をサポートしています。ローカルコンパイル方法の詳細は[ローカルパッケージング](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#本地化打包)を、Actions を使用したオンラインコンパイル方法の詳細は[GitHub Actions でのコンパイル](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#使用-github-actions-进行编译)を参照してください。

- #### 既存の OpenWrt システムへのカーネルインストール

[luci-app-amlogic](https://github.com/ophub/luci-app-amlogic/blob/main/README.cn.md) プラグインを使用して、コンパイル済みのカーネルを既存の OpenWrt システムにインストールできます。具体的な操作方法の詳細は[OpenWrt のアップグレード](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#升级-openwrt)を参照してください。
