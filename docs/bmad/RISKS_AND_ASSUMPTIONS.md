# Risks and Assumptions

## Risks
1. **Skin Integrity & Shape Accuracy (High Risk):** XMMS relies heavily on complex, pixel-perfect window shaping for skins. Cairo regions (`cairo_region_t`) must be constructed perfectly from old bitmap masks. Any mathematical error in calculating these regions will result in clipped UI elements or unclickable transparent areas, breaking the classic look and feel.
2. **Event Handling & Redrawing (Medium Risk):** GTK 3 altered the way expose events are handled (replacing `expose-event` with the `draw` signal). Some widgets, especially the highly customized animated ones like the spectrum analyzer, might flicker, tear, or fail to redraw correctly if their rendering logic isn't perfectly mapped to the Cairo `draw` context.
3. **CoreAudio Buffer Underruns (Medium Risk):** Interfacing a legacy C polling audio loop with modern asynchronous CoreAudio might introduce buffer underruns, stuttering, or high CPU usage if threading isn't managed carefully.
4. **Performance Overhead:** Simulating old X11 `GdkPixmap` blitting with Cairo surfaces might introduce performance overhead, particularly on the main GTK thread.

## Assumptions
1. We assume the Homebrew-installed GTK 3, Pango, and Cairo toolchain is correctly linked by the current `configure` script.
2. We assume `GtkGrid` migration (if performed) will map 1:1 with the sizing behaviors of the legacy `GtkTable`, preserving the tight spacing of the old UI.
3. We assume the user's primary goal is feature parity and visual fidelity to the original XMMS, even if that means avoiding some "modern" macOS UI paradigms in favor of custom-drawn widgets.
