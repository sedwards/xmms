#ifndef SKIN_H
#define SKIN_H

#include <gtk/gtk.h>
#include <cairo.h>

/* Enums remain the same as they are logic-based */

typedef enum
{
        SKIN_MAIN, SKIN_CBUTTONS, SKIN_TITLEBAR, SKIN_SHUFREP, SKIN_TEXT, SKIN_VOLUME,
        SKIN_BALANCE, SKIN_MONOSTEREO, SKIN_PLAYPAUSE, SKIN_NUMBERS, SKIN_POSBAR,
        SKIN_PLEDIT, SKIN_EQMAIN, SKIN_EQ_EX,
} SkinIndex;

typedef enum
{
        SKIN_MASK_MAIN, SKIN_MASK_EQ
} MaskIndex;

typedef enum
{
        SKIN_PLEDIT_NORMAL, SKIN_PLEDIT_CURRENT, SKIN_PLEDIT_NORMALBG,
        SKIN_PLEDIT_SELECTEDBG, SKIN_TEXTBG, SKIN_TEXTFG
} SkinColorIndex;

typedef struct
{
    /* GTK 3: Use Cairo surfaces instead of cairo_surface_t */
    cairo_surface_t *surface;
    cairo_surface_t *def_surface;
    
    gint width, height;
    gint current_width, current_height;
} SkinPixmap;

typedef struct
{
    gchar *path;
    /* ... SkinPixmap fields (main, cbuttons, etc) ... */
    SkinPixmap main;
    // ... (rest of your SkinPixmap members)

    /* GTK 3: GdkColor is deprecated, use GdkRGBA */
    GdkRGBA textbg[6], def_textbg[6];
    GdkRGBA textfg[6], def_textfg[6];
    
    /* For visualizer colors, you can keep guchar or move to GdkRGBA */
    guchar vis_color[24][3];

    /* GTK 3: Bitmaps/Masks are now cairo_region_t or alpha-channeled surfaces */
    cairo_region_t *mask_main;
    cairo_region_t *mask_eq;
    // ... etc
} Skin;

extern Skin *skin;

/* Lifecycle functions */
void init_skins(void);
void load_skin(const gchar *path);
void cleanup_skins(void);

/* Updated Utility Functions */
cairo_region_t *skin_get_mask(MaskIndex mi, gboolean doublesize, gboolean shaded);
GdkRGBA *get_skin_color(SkinColorIndex si);

/**
 * GTK 3 Drawing: 
 * Instead of GdkDrawable/cairo_t, we pass the cairo_t context 
 * usually provided by the "draw" signal.
 */
void skin_draw_pixmap(cairo_t *cr, SkinIndex si,
                      gint xsrc, gint ysrc, 
                      gint xdest, gint ydest,
                      gint width, gint height);

#endif

