# Technical Design - GTK 3 & Cairo Migration for Legacy Skin Engine

## Overview
The GTK 1/2 to GTK 3 migration replaced deprecated primitives **while absolutely preserving the classic pixel-perfect WinAmp 2.x skin engine.**

## 1. Widget Opaque Pointers (IMPLEMENTED)
- **Problem:** GTK 3 widgets are opaque. Direct struct access like `widget->window` throws a compilation error.
- **Solution:** Refactored to use modern accessors.
- Replaced `widget->window` with `gtk_widget_get_window(widget)`.
- Replaced `widget->allocation.width` with `gtk_widget_get_allocation(widget, &alloc); alloc.width;`.

## 2. Drawing System Overhaul (Gdk -> Cairo) (IMPLEMENTED)
- **Problem:** The skin engine heavily relied on `GdkPixmap`, `GdkBitmap`, `GdkGC`, and `gdk_draw_*` functions, which are completely removed in GTK 3.
- **Solution:**
- **Surfaces over Pixmaps:** Replaced `GdkPixmap` and `GdkBitmap` with `cairo_surface_t *`. Image surfaces (`cairo_image_surface_create`) are used to hold skin elements in memory.
- **Rendering Contexts:** Replaced `GdkGC` with `cairo_t *`. 
- **Drawing Logic:** Legacy drawing loops (e.g., in `playlistwin.c`, `textbox.c`) moved inside the GTK `draw` signal handler. We render the `cairo_surface_t` onto the window's cairo context using `cairo_set_source_surface` and `cairo_paint`.
- **HiDPI Support:** Integrated `CAIRO_FILTER_NEAREST` to maintain pixel sharpness on Retina displays.

## 3. Preserving Skin Transparency (Window Shaping) (IMPLEMENTED)
- **Problem:** XMMS achieves custom shapes using `gtk_widget_shape_combine_mask(widget, bitmap, x, y)`. This relies on `GdkBitmap` which no longer exists.
- **Solution:** GTK 3 uses Cairo regions for window shaping: `gtk_widget_shape_combine_region(widget, cairo_region_t *region)`.
- **Refactoring Masks:** Functions like `skin_get_mask()` in `skin.c` rewritten. Construct a `cairo_region_t` by mathematically unioning the opaque rectangles and return that region.
- This ensures the irregular shapes of the legacy UI are preserved without artifacts.

## 4. macOS Integration (IMPLEMENTED)
- **darwin_dirbrowser.m:** Wraps macOS `NSOpenPanel` inside a standard C function interface exposed via header, replacing GTK file dialogs.
- **audio_coreaudio.m:** Implements the CoreAudio callbacks conforming to the legacy XMMS Output plugin struct.
- **GtkosxApplication:** Moves XMMS menus to the native Apple system menu bar.

## 5. Plugin Loading System (IMPLEMENTED)
- Updated `pluginenum.c` to scan subdirectories relative to `PLUGIN_DIR`.
- Fixed symbol resolution on macOS by exporting key symbols (e.g., `xmms_usleep`) and using `-undefined dynamic_lookup`.
- Resolved `g_basename` related crashes in `init_plugins` via NULL checks.

## 7. File System & Configuration (IMPLEMENTED)
- **Problem:** XMMS originally used `~/.xmms` for configuration and plugins, which is non-standard on macOS.
- **Solution:**
- **Centralized Path Logic:** Introduced `xmms_get_config_dir()` in `libxmms/util.c` to return `~/Library/XMMS` on macOS and `~/.xmms` on other platforms.
- **Configuration Management:**
    - `load_config()` and `save_config()` in `main.c` now use `xmms_get_config_dir()`.
    - Directory creation (`g_mkdir_with_parents`) happens automatically at startup.
- **Plugin Migration:** `pluginenum.c` now scans `~/Library/XMMS/Plugins` for user-installed modules.
- **Periodic State Persistence:** Added a 3-minute `periodic_save_timer` to `main.c` to ensure window positions are saved even if the application isn't closed gracefully.

## 8. Skinned Preferences Window (IMPLEMENTED)
- **Problem:** The original Preferences window used standard GTK widgets that didn't match the loaded skin.
- **Solution:**
- **CSS-Based Styling:** Implemented `prefswin_apply_skin()` in `xmms/prefswin.c`. This function dynamically generates a GTK CSS provider using colors extracted from the skin (specifically `SKIN_PLEDIT_NORMALBG` and `SKIN_PLEDIT_NORMAL`).
- **Modern Widgets:** Refactored the window to use `GtkTreeView` and `GtkComboBoxText`, replacing the deprecated `GtkCList` and `GtkOptionMenu`.
- **Integrated Actions:** Added a dedicated "Save" button to commit configuration changes to disk immediately.

