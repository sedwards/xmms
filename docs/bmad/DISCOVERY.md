# Discovery - XMMS Darwin Port

## Project Overview
This project is a port of the classic XMMS (X Multimedia System) to macOS (Darwin). 
The goal is to modernize the old GTK 1/2 codebase to build against GTK 3, replace obsolete drawing primitives with Cairo/Pango, and integrate native macOS functionality (CoreAudio, Metal, native directory browsing).

## Build System
The project uses `autoconf`, `automake`, and `libtool`.
The standard configuration for this port is:
```bash
./configure --disable-opengl --enable-metal --enable-macos-audio --prefix=$(pwd)/build
```

## Compilation Failures & Analysis
Initial build attempts reveal systemic compilation errors across the GTK/GDK surface area:
1. **Opaque Structures:** Direct struct access (e.g., `mainwin->window`) fails because GTK 3 makes widget structures opaque. Accessors like `gtk_widget_get_window()` must be used.
2. **Removed GDK Drawing Primitives:** Types like `GdkBitmap`, `GdkPixmap`, `GdkColormap`, and `GdkGC` no longer exist in GTK 3. Functions like `gdk_draw_rectangle`, `gdk_gc_new`, and `gdk_pixmap_create_from_xpm_d` are gone.
3. **Cairo Requirement:** All custom widget drawing must be ported to use `cairo_t`, `cairo_surface_t`, and the `draw` signal (or `GtkDrawingArea` conventions in GTK 3).
4. **Shaped Windows:** `gtk_widget_shape_combine_mask` is removed. Window shaping in GTK 3 requires using Cairo regions or `gtk_widget_shape_combine_region`.
5. **Deprecated Layouts:** `GtkTable` is deprecated and heavily warns; it should technically be replaced by `GtkGrid`, though warnings do not halt the build immediately.

## macOS Integrations Required
1. **Objective-C Directory Browser:** Implement `darwin_dirbrowser.m` to replace the standard GTK file chooser with native macOS open panels.
2. **CoreAudio Output:** Ensure the CoreAudio plugin (`libxmms/audio_coreaudio.m` and `Output` plugins) works.
3. **Metal Acceleration:** Wire up Metal visualizer support instead of the legacy OpenGL spectrum analyzer.

## Immediate Next Steps
- Draft the Technical Design for the GTK 3 + Cairo migration.
- Define a strict file-touch list for the Builder.
