# Discovery - XMMS Darwin Port

## Project Overview
This project is a port of the classic XMMS (X Multimedia System) to macOS (Darwin). 
The goal is to modernize the old GTK 1/2 codebase to build against GTK 3, replace obsolete drawing primitives with Cairo/Pango, and integrate native macOS functionality (CoreAudio, native directory browsing).

## Build System
The project uses `autoconf`, `automake`, and `libtool`.
The standard configuration for this port is:
```bash
./configure --disable-opengl --enable-metal --enable-macos-audio --prefix=$(pwd)/build
```

## Compilation & Integration Findings (Updated 2026-03-13)
1. **GTK 3 & Cairo Success:** The entire UI (Main, Playlist, EQ, Prefs) has been successfully ported to GTK 3 and Cairo.
2. **Skin Engine Sharpness:** Using `CAIRO_FILTER_NEAREST` and GTK 3 logical coordinates (1 unit = 1 skin pixel) preserves the pixel-perfect look of the original WinAmp 2.x skins on Retina displays.
3. **Plugin Loading Architecture:** 
    - `pluginenum.c` was updated to scan subdirectories (`Input`, `Output`, etc.) relative to `PLUGIN_DIR`.
    - Symbols like `xmms_check_realtime_priority` and `xmms_usleep` must be exported by the main binary for plugins to resolve them via `dlopen`.
    - `PLUGIN_LDFLAGS` must include `-undefined dynamic_lookup` on macOS.
4. **Playback Success:** 
    - **WAV:** Verified perfect playback via `libwav.so` and `libcoreaudio.so`.
    - **MP3:** Verified perfect playback via `libmpg123.so`.
    - **OGG:** Verified perfect playback via `libvorbis.so`.
5. **macOS Integration:** 
    - `GtkosxApplication` (from `gtk-mac-integration-gtk3`) successfully moves XMMS menus to the native Apple system menu bar.
    - `NSOpenPanel` is integrated via `darwin_dirbrowser.m` for native file/directory selection.

## Resolved Issues
- **Crash in init_plugins:** Fixed by adding NULL checks for `cfg.outputplugin` and `ip->filename` in `pluginenum.c`.
- **Syntax Errors:** Resolved corruption in `main.c` caused by placeholder errors in previous edits.
- **UI Interaction:** Refactored `widget.c` to allow window dragging on the background while preserving widget clicks.

## Next Steps
- Implement volume/balance synchronization between the Main and Equalizer windows.
- Finalize HiDPI sharp rendering defaults.
- Investigate potential Audacious plugin wrapper for extended format support.
