# cpp-win-gdi-screenshot-tool

## WindowsScreenshotTool

WindowsScreenshotTool is a minimalistic Windows screenshot utility written in C++ using Win32, GDI, and GDI+. 
Supports global hotkeys, full-screen and region captures, and saves PNGs automatically — no UI, no interruptions.

## Features

- **Full-screen & region capture** via customizable global hotkeys.
- **Transparent overlay** with live rubber-band selection (region).
- **Instant PNG save** with timestamped filenames (`Screenshot_YYYYMMDD_HHMMSS_MMM.png`).
- **Continuous operation**: ready for the next capture immediately.
- Zero UI distractions—runs silently in the background.

## Configuration (`config.ini`)

```ini
[General]
FullHotkey=Win+Shift+Q
RegionHotkey=Win+Shift+A
OutputFolder=%USERPROFILE%\Pictures\Screenshots
```

* **FullHotkey**: e.g., `Win+Shift+Q` for full-screen capture.
* **RegionHotkey**: e.g., `Win+Shift+A` for region capture.
* **OutputFolder**: directory where PNGs are saved (auto-created if missing).

You can customize these after the first run.

## Usage

* Press **FullHotkey** full desktop captured, PNG saved.
* Press **RegionHotkey** screen freezes; drag to select region; release to save PNG.
* Press **Esc** or **right-click** during selection cancel capture.
* Can repeat endlessly—no restart needed.

## Design

Outlined modules:

```
AppController
 ├─ ConfigManager       // reads INI and expands env vars
 ├─ HotkeyListener      // registers global hotkeys
 ├─ CaptureEngine       // captures full screen or crops region
 ├─ OverlayWindow       // transparent overlay + rubber-band selection
 └─ FileManager         // saves PNG with timestamped filename
```

## Dependencies

* Windows 10+
* **GDI+** (`gdiplus.lib`) for PNG encoding
* Pure Win32 API — no third-party libraries

## Known limitations

* Single-monitor, 100% DPI only.
* Cursor not captured.
* Silent failure if hotkey registration or file save fails.
* No async file I/O.

