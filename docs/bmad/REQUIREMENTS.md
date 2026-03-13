# Requirements - XMMS Darwin Port

## Objective
Port the legacy GTK 1.2 / GTK 2 XMMS codebase to build and run natively on macOS (Darwin) utilizing GTK 3, Cairo, Pango, CoreAudio, and Metal, **while strictly preserving the classic skin-based UI.**

## Functional Requirements
1. **[DONE] Skin Engine Preservation:** The legacy WinAmp 2.x skin engine is fully functional. The default UI, pixel-perfect window layouts, and transparency masks behave exactly as original.
2. **[DONE] GTK 3 Migration:** Application successfully compiles and links against GTK 3 on Darwin/ARM-64.
3. **[DONE] Modern Drawing Pipeline:** Legacy GDK drawing primitives replaced with modern Cairo operations (`cairo_t`, `cairo_surface_t`, `cairo_region_t`).
4. **[DONE] CoreAudio Integration:** Native audio output via `libcoreaudio.so` implemented and verified.
5. **[DONE] Native File Browser:** macOS-native directory browser utilizing `NSOpenPanel` implemented in `libxmms/darwin_dirbrowser.m`.
6. **[DONE] Audio Format Support:** Verified playback for WAV, MP3, and OGG formats via plugin-based architecture.
7. **[DONE] macOS Menu Integration:** Native Apple menu bar support using `GtkosxApplication`.

## Pending Requirements
1. **Volume/Balance Sync:** Synchronize volume and balance sliders between the Main and Equalizer windows.
2. **HiDPI Sharpness:** Ensure consistent sharp rendering using `CAIRO_FILTER_NEAREST` across all skin elements.

## Non-Goals
1. We are **not** rewriting the core audio logic or decoding loops.
2. We are **not** modernizing the UI design to look like a native macOS app; the goal is to keep the custom skinning system.
3. We are **not** porting to GTK 4. GTK 3 is the explicit target.

## Technical Constraints
- The project must build using the existing GNU Autotools system (`configure`, `make`).
- Objective-C code must bridge smoothly with the existing C codebase.
- **LEGACY X11 REMOVAL:** Zero dependencies on `X11/`, `Xlib.h`, or `GdkX11`. All drawing must be Cairo-based.
