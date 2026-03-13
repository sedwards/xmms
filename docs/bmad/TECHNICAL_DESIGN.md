# Technical Design - GTK 3 & Cairo Migration for Legacy Skin Engine

## Overview
The GTK 1/2 to GTK 3 migration requires a complete overhaul of the drawing and widget architecture in the XMMS C codebase. This document outlines the technical strategy for replacing deprecated primitives **while absolutely preserving the classic pixel-perfect WinAmp 2.x skin engine.**

## 1. Widget Opaque Pointers
**Problem:** GTK 3 widgets are opaque. Direct struct access like `widget->window` throws a compilation error.
**Solution:** Refactor to use modern accessors.
- Replace `widget->window` with `gtk_widget_get_window(widget)`.
- Replace `widget->allocation.width` with `gtk_widget_get_allocation(widget, &alloc); alloc.width;`.

## 2. Drawing System Overhaul (Gdk -> Cairo)
**Problem:** The skin engine heavily relies on `GdkPixmap`, `GdkBitmap`, `GdkGC`, and `gdk_draw_*` functions, which are completely removed in GTK 3.
**Solution:**
- **Surfaces over Pixmaps:** Replace `GdkPixmap` and `GdkBitmap` with `cairo_surface_t *`. Image surfaces (`cairo_image_surface_create`) will be used to hold skin elements in memory.
- **Rendering Contexts:** Replace `GdkGC` with `cairo_t *`. 
- **Drawing Logic:** Legacy drawing loops (e.g., in `playlistwin.c`, `textbox.c`) must be moved inside the GTK `draw` signal handler. We will render the `cairo_surface_t` onto the window's cairo context using `cairo_set_source_surface` and `cairo_paint`.
- **Image Loading:** Replace `gdk_pixmap_create_from_xpm_d` with `gdk_pixbuf_new_from_xpm_data()`. Convert the resulting `GdkPixbuf` to a `cairo_surface_t` using `gdk_cairo_surface_create_from_pixbuf` for faster hardware-accelerated drawing.

## 3. Preserving Skin Transparency (Window Shaping)
**Problem:** XMMS achieves custom shapes (like rounded corners or non-rectangular elements) using `gtk_widget_shape_combine_mask(widget, bitmap, x, y)`. This relies on `GdkBitmap` which no longer exists.
**Solution:** GTK 3 uses Cairo regions for window shaping: `gtk_widget_shape_combine_region(widget, cairo_region_t *region)`.
- **Refactoring Masks:** Functions like `skin_get_mask()` in `skin.c` must be rewritten. Instead of returning a `GdkBitmap*`, they must read the 1-bit transparency data from the skin files, construct a `cairo_region_t` by mathematically unioning the opaque rectangles, and return that region.
- This ensures the irregular shapes of the legacy UI are preserved without artifacts.

## 4. Exposed Layout Components
**Problem:** `GtkTable` is deprecated and generates many warnings.
**Solution:** To minimize risk to the intricate sizing of the legacy UI, we will initially suppress `GtkTable` deprecation warnings or perform a conservative, 1:1 mapped migration to `GtkGrid` using `gtk_grid_new` and `gtk_grid_attach`.

## 5. Objective-C Subsystems
**darwin_dirbrowser.m:** Will wrap macOS `NSOpenPanel` / `NSSavePanel` inside a standard C function interface exposed via header, replacing GTK file dialogs where appropriate for a native feel.
**audio_coreaudio.m:** Will implement the CoreAudio callbacks conforming to the legacy XMMS Output plugin struct.

## 6. Legacy X11 Removal
All X11-specific logic (e.g., in `skin.c`, `main.c`, `titlestring.c`) will be removed. This includes `XQueryColor` and any other direct Xlib or GDK-X11 bridge calls. The application will rely solely on GTK 3's backend abstractions and Cairo for drawing.
