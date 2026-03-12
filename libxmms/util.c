#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <gtk/gtk.h>

/* ... (Keep your existing system includes for sched.h, sysctl, etc.) ... */

GtkWidget *xmms_show_message(gchar *title, gchar *text, gchar *button_text, gboolean modal, GCallback button_action, gpointer action_data)
{
    GtkWidget *dialog, *content_area, *vbox, *label, *action_area, *button;

    // 1. Create dialog (GTK 3 uses accessor functions for internal boxes)
    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_modal(GTK_WINDOW(dialog), modal);

    // 2. Get the internal content area (replaces GTK_DIALOG(dialog)->vbox)
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // 3. GtkVBox/GtkHBox are replaced by GtkBox with orientation
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    label = gtk_label_new(text);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // 4. Handle the action area (replaces HButtonBox and direct struct access)
    action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    gtk_button_box_set_layout(GTK_BUTTON_BOX(action_area), GTK_BUTTONBOX_SPREAD);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 5);

    button = gtk_button_new_with_label(button_text);
    
    // 5. Signal Porting: Use g_signal_connect and G_CALLBACK
    if (button_action)
        g_signal_connect(button, "clicked", button_action, action_data);
    
    // g_signal_connect_swapped replaces gtk_signal_connect_object
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);

    gtk_box_pack_start(GTK_BOX(action_area), button, FALSE, FALSE, 0);
    
    // 6. Replace GTK_WIDGET_SET_FLAGS with specific setter
    gtk_widget_set_can_default(button, TRUE);
    gtk_widget_grab_default(button);

    // In GTK 3, showing the top-level window shows children (if they aren't explicitly hidden)
    gtk_widget_show_all(dialog);

    return dialog;
}

/* xmms_check_realtime_priority and xmms_usleep remain unchanged as they are system/POSIX calls */


