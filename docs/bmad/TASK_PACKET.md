# TASK PACKET - Phase 1: GTK 3 & Cairo UI Migration

## Objective
Update the core XMMS GUI logic to replace deprecated GTK/GDK drawing APIs with Cairo equivalents, enabling successful compilation under GTK 3 **without altering the legacy UI layout or skin engine behavior**.

## Non-goals
- Do not refactor core audio pipelines yet. Focus exclusively on GUI compilation and linking.
- Do not rewrite plugins that compile cleanly without changes.
- Do not attempt to "modernize" the look of the UI; keep it pixel-perfect.

## Constraints
- Must use GTK 3 and Cairo.
- Build must be performed with `./configure --disable-opengl --enable-metal --enable-macos-audio --prefix=$(pwd)/build`
- Skin transparency masks must be translated mathematically accurately from bitmaps to `cairo_region_t`.

## Exact File Touch List
**Modify:**
- `xmms/skin.h` / `xmms/skin.c`
- `xmms/main.c`
- `xmms/playlistwin.c`
- `xmms/textbox.c`
- `xmms/titlestring.c`
- `libxmms/util.c`
- `libxmms/dirbrowser.c`
- `gnomexmms/gnomexmms.c`
- `wmxmms/wmxmms.c`
- `Visualization/blur_scope/blur_scope.c`
- `Visualization/sanalyzer/spectrum.c`
- `Input/mikmod/plugin.c`

## Step-by-Step Implementation Checklist
- [ ] Phase 1.1: Fix opaque widget pointer access (`widget->window` -> `gtk_widget_get_window(widget)`).
- [ ] Phase 1.2: Refactor `skin.c` to use `cairo_surface_t` instead of `GdkPixmap`/`GdkBitmap` for loaded skin elements.
- [ ] Phase 1.3: Rewrite `skin_get_mask()` to generate and return `cairo_region_t*` by parsing the loaded surface data, preserving precise UI shapes.
- [ ] Phase 1.4: Replace `gtk_widget_shape_combine_mask` with `gtk_widget_shape_combine_region`.
- [ ] Phase 1.5: Replace `gdk_pixmap_create_from_xpm_d` with `gdk_pixbuf_new_from_xpm_data` and Cairo surface conversion across the codebase.
- [ ] Phase 1.6: Migrate legacy drawing loops (`gdk_draw_rectangle`, `GdkGC`) in UI elements to use `cairo_t` inside a `draw` signal handler (or equivalent rendering path).
- [ ] Phase 1.7: Migrate deprecated `GtkTable` usages (e.g., in `titlestring.c`) to `GtkGrid`, strictly preserving existing spacing values.

## Acceptance Tests
- **Command:** `make`
- **Outcome:** Successfully completes for the `xmms` and `libxmms` directories without fatal `undeclared function` errors for GDK/GTK primitives.

## Open Risks / Blockers
- The `configure` script may lack necessary `-framework Cocoa` or `-framework Metal` linker flags in specific subdirectories if Automake macros are incomplete. We will diagnose linker errors once compilation succeeds.
