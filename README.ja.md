# FocusCycle

**FocusCycle** は、Ubuntu / Linux 環境向けに **Qt5 (C++)** で開発されたモダンで軽量なポモドーロテクニック実践デスクトップアプリケーションです。Wayland / X11 対応の未操作時間検出機能、会社の休憩時間スケジュール機能、作業履歴の自動記録、常駐システムトレイ、および英語・日本語の表示切り替えに対応しています。

---

## 主な機能

- **ポモドーロタイマーエンジン**: 作業時間（1分〜55分）および休憩時間（1分〜15分）を柔軟にカスタマイズ可能。
- **集中保護（操作検出機能）**:
  - 未操作時間（0秒〜30秒）のしきい値検出。
  - Wayland環境（GNOME Mutter D-Bus API: `org.gnome.Mutter.IdleMonitor`）を優先使用し、X11ではXScreenSaverをフォールバックとして使用。
  - ユーザーが集中してPCを操作している間はダイアログの表示を自動抑制し、キーボード・マウスの未操作時間が設定値に達した段階で休憩ダイアログを表示。
- **会社の休憩時間スケジュール**:
  - 定時休憩時刻を最大10個まで登録可能（例: 12:00, 15:00）。
  - 設定時刻に達すると専用ダイアログが表示され、すぐに休憩に入るか作業を継続するかを選択できます。
- **コンパクト ＆ ドラッグ移動可能なUI**:
  - 円形カウントダウンプログレスリングによるシックなダークテーマ。
  - カード上のどこをドラッグしてもデスクトップ上で自由に移動できるコンパクトな設計。
- **システムトレイ常駐機能**: 作業完了/休憩完了の操作、設定ダイアログ起動、ログ確認、ツールチップでのリアルタイム表示。
- **作業ログの自動記録**: ポモドーロの実行履歴を CSV 形式（`~/.local/share/FocusCycle/history.csv`）に自動保存。
- **多言語UI対応**: **英語（デフォルト）** と **日本語** を設定ダイアログからいつでも切替可能。

---

## システム要件

- **OS**: Linux（Ubuntu 24.04 LTS 等）
- **フレームワーク**: Qt 5.15以上（Widgets, Core, Gui, DBus）
- **コンパイラ**: C++17 対応コンパイラ（`g++` / `clang`）
- **通知ユーティリティ**: `libnotify` (`notify-send`)

---

## ビルド方法

必要な開発ライブラリをインストールします：

```bash
sudo apt update
sudo apt install qtbase5-dev libqt5dbus5 libqt5widgets5 libnotify-bin build-essential
```

ソースコードを取得しビルドします：

```bash
cd FocusCycle
qmake FocusCycle.pro
make -j$(nproc)
```

アプリケーションの起動：

```bash
./FocusCycle
```

---

## 設定の変更

設定データは INI 形式で `~/.config/FocusCycle/FocusCycle.ini` に保存されます。

GUIから設定変更する場合：
1. システムトレイの **FocusCycle** アイコンを右クリックします。
2. **Open Settings**（または **設定を開く**）を選択します。
3. タイマー時間、操作検出しきい値、言語設定（English / 日本語）、会社の休憩時間を設定して「Save（保存）」を押します。

---

## 言語ドキュメント

- English: [README.md](README.md)
- 日本語: [README.ja.md](README.ja.md)

---

## 商標・免責事項

> Pomodoro® および Pomodoro Technique® は Francesco Cirillo 氏の登録商標です。本アプリは同氏と提携・公認関係にあるものではありません。
> 
> Pomodoro® and The Pomodoro Technique® are registered trademarks of Francesco Cirillo. This application is not affiliated with, associated with, or endorsed by Francesco Cirillo.

---

## ライセンス

[MIT License](LICENSE) の下で公開されています。
