#ifndef XMMS_UTIL_H
#define XMMS_UTIL_H
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GtkSignalFunc is replaced with GCallback */
GtkWidget *xmms_show_message(gchar *title, 
                             const gchar *text, 
                             gchar *button_text, 
                             gboolean modal, 
                             GCallback button_action, 
                             gpointer action_data);

gboolean xmms_check_realtime_priority(void);
void xmms_usleep(gint usec);
char *xmms_get_config_dir(void);
char *xmms_color_to_hex(GdkColor *color);
const char *util_basename(const char *name);

#ifdef __cplusplus
}
#endif
    
#endif

