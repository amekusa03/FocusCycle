# FocusCycle

**FocusCycle** is a modern, lightweight Pomodoro Technique desktop application built for Ubuntu/Linux using **Qt5 (C++)**. It features dynamic idle detection for Wayland/X11, custom company break scheduling, interactive activity logging, system tray integration, and dual-language support (English / Japanese).

---

## Key Features

- **Pomodoro Timer Engine**: Customizable work durations (1–55 mins) and break durations (1–15 mins).
- **Focus Protection (Idle Detection)**:
  - Detects user inactivity threshold (0–30 seconds).
  - Uses Wayland GNOME Mutter D-Bus API (`org.gnome.Mutter.IdleMonitor`) with XScreenSaver fallback on X11.
  - Automatically suppresses popup dialogs while you are actively focusing until idle threshold is reached.
- **Scheduled Company Breaks**:
  - Configure up to 10 daily fixed break times (e.g., 12:00, 15:00).
  - Prompts dedicated options to rest immediately or defer while keeping the timer running.
- **Compact & Draggable UI**:
  - Sleek dark theme with custom circular progress ring animation.
  - Frameless, compact window draggable from anywhere on the card.
- **System Tray Integration**: Quick actions for completion, settings, work logs, and status tooltips.
- **Session History Logging**: Automatically logs work sessions to CSV (`~/.local/share/FocusCycle/history.csv`).
- **Language Support**: Seamlessly switch between **English** (default) and **Japanese**.

---

## Requirements

- **OS**: Linux (tested on Ubuntu 24.04 LTS)
- **Framework**: Qt 5.15+ (Widgets, Core, Gui, DBus)
- **Compiler**: C++17 compatible compiler (`g++` / `clang`)
- **Notification Utility**: `libnotify` (`notify-send`)

---

## Building from Source

Ensure Qt5 development libraries are installed:

```bash
sudo apt update
sudo apt install qtbase5-dev libqt5dbus5 libqt5widgets5 libnotify-bin build-essential
```

Clone the repository and build:

```bash
cd FocusCycle
qmake FocusCycle.pro
make -j$(nproc)
```

Run the application:

```bash
./FocusCycle
```

---

## Configuration

Settings are saved in INI format at `~/.config/FocusCycle/FocusCycle.ini`.

To change settings via the GUI:
1. Right-click the **FocusCycle** icon in your system tray.
2. Select **Open Settings** (or **設定を開く**).
3. Adjust timers, idle threshold, language, or company break schedules.

---

## Language Documentation

- English: [README.md](README.md)
- 日本語: [README.ja.md](README.ja.md)

---

## Trademark Notice & Disclaimer

> Pomodoro® and The Pomodoro Technique® are registered trademarks of Francesco Cirillo. This application is not affiliated with, associated with, or endorsed by Francesco Cirillo.

---

## License

Open-source under the [MIT License](LICENSE).
