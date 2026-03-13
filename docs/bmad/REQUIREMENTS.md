# Requirements - XMMS Darwin Port

## Objective
Port the legacy GTK 1.2 / GTK 2 XMMS codebase to build and run natively on macOS (Darwin) utilizing GTK 3, Cairo, Pango, CoreAudio, and Metal, **while strictly preserving the classic skin-based UI.**

## Functional Requirements
1. **Skin Engine Preservation (CRITICAL):** The legacy WinAmp 2.x skin engine must remain fully functional. The default UI, pixel-perfect window layouts, and transparency masks must look and behave exactly as they did in the original release.
2. **GTK 3 Migration:** The application must successfully compile and link against GTK 3 (`/opt/homebrew/Cellar/gtk+3/...`).
3. **Modern Drawing Pipeline:** All legacy GDK drawing types and functions (`GdkPixmap`, `GdkBitmap`, `GdkGC`, `gdk_draw_*`) must be completely replaced with modern Cairo operations (`cairo_t`, `cairo_surface_t`, `cairo_region_t`) in a way that perfectly mimics the old bitmapped rendering.
4. **CoreAudio Integration:** Implement native audio output for macOS via `libxmms/audio_coreaudio.m` and any necessary Objective-C bridging.
5. **Native File Browser:** Implement a macOS-native directory browser utilizing `NSSavePanel` / `NSOpenPanel` inside `libxmms/darwin_dirbrowser.m` to replace or supplement GTK file dialogs on macOS.
6. **Metal Visualizations:** Enable and verify the Metal-based visualization plugin (`libxmms/metal_visualizer.m`) to replace the deprecated OpenGL spectrum analyzer.

## Non-Goals
1. We are **not** rewriting the core audio logic or decoding loops.
2. We are **not** modernizing the UI design to look like a native macOS app; the goal is to keep the custom skinning system.
3. We are **not** porting to GTK 4. GTK 3 is the explicit target.

## Technical Constraints
- The project must build using the existing GNU Autotools system (`configure`, `make`), explicitly utilizing the provided configuration flags.
- Objective-C code must bridge smoothly with the existing C codebase (compiling as `.m` files).
- Cairo rendering must be optimized to handle the frequent pixel updates required by the spectrum analyzer and skin animations without excessive CPU overhead.
- **LEGACY X11 REMOVAL:** All legacy X11-specific code and dependencies must be completely removed. We are only targeting macOS (Darwin) with GTK 3 / Cairo.
