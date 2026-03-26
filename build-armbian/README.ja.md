# ビルドファイルの説明

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

Armbian システムのコンパイルに必要なファイルは、機能別に各サブディレクトリに分類して格納されています。

## armbian-files

このディレクトリには、Armbian のビルドプロセスで必要なすべてのファイルが格納されています。`common-files` ディレクトリにはプラットフォームに依存しない共通ファイル、`platform-files` ディレクトリには各プラットフォーム専用ファイル、`different-files` ディレクトリには異なるデバイス向けの差分設定ファイルが格納されています。詳細は [documents](../documents/README.ja.md) の `12.15` セクションを参照してください。

必要なファームウェアは [ophub/firmware](https://github.com/ophub/firmware) リポジトリから `common-files/usr/lib/firmware` ディレクトリに自動的にダウンロードされます。

## kernel

`kernel` ディレクトリ内にバージョン番号に対応するフォルダを作成します（例：`stable/5.10.125`）。複数のカーネルをコンパイルする必要がある場合は、各バージョンのディレクトリを順次作成し、対応するカーネルファイルを配置してください。カーネルファイルは [kernel](https://github.com/ophub/kernel) リポジトリからダウンロードするか、[自分でコンパイル](../compile-kernel)することもできます。ローカルにカーネルファイルが事前に配置されていない場合、ビルドスクリプトが kernel リポジトリから自動的にダウンロードします。

## u-boot

システムブートファイルです。使用するカーネルバージョンに応じて、対応する u-boot ファイルはインストールまたは更新スクリプトによって自動的に取得・設定されます。必要な u-boot は [ophub/u-boot](https://github.com/ophub/u-boot) リポジトリから `u-boot` ディレクトリに自動的にダウンロードされます。
