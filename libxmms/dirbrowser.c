#include <gtk/gtk.h>
#include "dirbrowser.h"

extern void open_native_dir_browser(const char *title, const char *current_path, int mode, void (*handler) (gchar *));

GtkWidget *xmms_create_dir_browser(gchar * title, gchar * current_path, GtkSelectionMode mode, void (*handler) (gchar *))
{
    /* On macOS, we use the native NSOpenPanel */
    open_native_dir_browser(title, current_path, (int)mode, handler);
    
    /* Since the native browser is modal and handles its own lifecycle, 
       we don't return a GtkWidget handle to it in the traditional sense. */
    return NULL;
}
