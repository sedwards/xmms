#ifndef GTK3_COMPAT_H
#define GTK3_COMPAT_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* Suppress deprecated declarations globally for this port */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

/* GTK 3 Compatibility Macros */
#ifndef GtkObject
#define GtkObject GObject
#endif
#ifndef GTK_OBJECT
#define GTK_OBJECT(x) G_OBJECT(x)
#endif
#ifndef GTK_SIGNAL_FUNC
#define GTK_SIGNAL_FUNC(x) G_CALLBACK(x)
#endif
#define gtk_signal_connect(obj, sig, func, data) g_signal_connect(obj, sig, func, data)
#define gtk_signal_connect_object g_signal_connect_swapped
#define gtk_signal_disconnect_by_func(obj, func, data) g_signal_handlers_disconnect_by_func(obj, func, data)
#define gtk_object_set_data g_object_set_data
#define gtk_object_get_data g_object_get_data
#define gtk_object_set_data_full g_object_set_data_full
#define gtk_widget_set_usize(w, width, height) gtk_widget_set_size_request(w, width, height)
#define gdk_window_get_size(win, w, h) do { *(w) = gdk_window_get_width(win); *(h) = gdk_window_get_height(win); } while(0)
#define gdk_screen_width() gdk_screen_get_width(gdk_screen_get_default())
#define gdk_screen_height() gdk_screen_get_height(gdk_screen_get_default())
#define GDK_THREADS_ENTER() gdk_threads_enter()
#define GDK_THREADS_LEAVE() gdk_threads_leave()

#define gtk_object_remove_data(obj, key) g_object_set_data(G_OBJECT(obj), key, NULL)
#define GTK_WIDGET_VISIBLE(w) gtk_widget_get_visible(w)
#define gtk_widget_set_uposition(w, x, y) gtk_window_move(GTK_WINDOW(w), x, y)
#define gdk_window_get_deskrelative_origin(win, x, y) gdk_window_get_root_origin(win, x, y)

#define GTK_WINDOW_DIALOG GTK_WINDOW_TOPLEVEL

#define GTK_CAN_DEFAULT (1 << 0)
#define GTK_CAN_FOCUS   (1 << 1)

#define GTK_WIDGET_SET_FLAGS(w, f) do { \
    if ((f) & GTK_CAN_DEFAULT) gtk_widget_set_can_default(w, TRUE); \
    if ((f) & GTK_CAN_FOCUS) gtk_widget_set_can_focus(w, TRUE); \
} while(0)

#define gtk_vbox_new(homo, spacing) gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing)
#define gtk_hbox_new(homo, spacing) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing)
#define gtk_hseparator_new() gtk_separator_new(GTK_ORIENTATION_HORIZONTAL)
#define gtk_hbutton_box_new() gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL)
#define gtk_button_box_set_spacing(w, s) gtk_box_set_spacing(GTK_BOX(w), s)

#define gtk_timeout_add(i, f, d) g_timeout_add(i, (GSourceFunc)f, d)
#define gtk_timeout_remove(t) g_source_remove(t)

/* Accessors for opaque structs */
#define gtk_adjustment_value(adj) gtk_adjustment_get_value(GTK_ADJUSTMENT(adj))
#define gtk_dialog_vbox(dlg) gtk_dialog_get_content_area(GTK_DIALOG(dlg))
#define gtk_dialog_action_area(dlg) gtk_dialog_get_action_area(GTK_DIALOG(dlg))

void xmms_log(const char *format, ...);

#endif /* GTK3_COMPAT_H */
