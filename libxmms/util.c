#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <gtk/gtk.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#elif defined HAVE_SYS_SCHED_H
#include <sys/sched.h>
#endif

#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

GtkWidget *xmms_show_message(gchar *title, gchar *text, gchar *button_text, gboolean modal, GCallback button_action, gpointer action_data)
{
    GtkWidget *dialog, *content_area, *vbox, *label, *action_area, *button;

    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_modal(GTK_WINDOW(dialog), modal);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    label = gtk_label_new(text);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    gtk_button_box_set_layout(GTK_BUTTON_BOX(action_area), GTK_BUTTONBOX_SPREAD);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 5);

    button = gtk_button_new_with_label(button_text);
    
    if (button_action)
        g_signal_connect(button, "clicked", button_action, action_data);
    
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);

    gtk_box_pack_start(GTK_BOX(action_area), button, FALSE, FALSE, 0);
    
    gtk_widget_set_can_default(button, TRUE);
    gtk_widget_grab_default(button);

    gtk_widget_show_all(dialog);

    return dialog;
}

gboolean xmms_check_realtime_priority(void)
{
#ifdef HAVE_SCHED_SETSCHEDULER
#ifdef __FreeBSD__
	int val = 0, len;

	len = sizeof(val);
	sysctlbyname("p1003_1b.priority_scheduling", &val, &len, NULL, 0);
	if ( !val )
		return FALSE;
#endif
	if (sched_getscheduler(0) == SCHED_RR)
		return TRUE;
	else
#endif
		return FALSE;
}

void xmms_usleep(gint usec)
{
	struct timespec req;

	req.tv_sec = usec / 1000000;
	usec -= req.tv_sec * 1000000;
	req.tv_nsec = usec * 1000;

	nanosleep(&req, NULL);
}
