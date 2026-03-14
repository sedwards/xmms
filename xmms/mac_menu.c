#include "xmms.h"
#include <gtk/gtk.h>
#include "prefswin.h"
#include "libxmms/util.h"
#include "log.h"

#ifdef HAVE_GTK_MAC
#include <gtkosxapplication.h>

static void on_aot_toggled(GtkMenuItem *item, gpointer data)
{
    cfg.always_on_top = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
    mainwin_set_always_on_top(cfg.always_on_top);
    save_config();
}

static void on_doublesize_toggled(GtkMenuItem *item, gpointer data)
{
    cfg.doublesize = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
    mainwin_recreate();
    save_config();
}

static void on_snap_toggled(GtkMenuItem *item, gpointer data)
{
    cfg.snap_windows = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
    save_config();
}

static void on_save_pos_toggled(GtkMenuItem *item, gpointer data)
{
    cfg.save_window_position = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
    save_config();
}

static void on_save_on_quit_toggled(GtkMenuItem *item, gpointer data)
{
    cfg.save_config_on_quit = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
    save_config();
}

static void on_output_plugin_selected(GtkMenuItem *item, gpointer data)
{
    int index = GPOINTER_TO_INT(data);
    set_current_output_plugin(index);
    save_config();
}

void setup_mac_main_menu(void)
{
    GtkosxApplication *theApp = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
    GtkWidget *menubar = gtk_menu_bar_new();

    /* Options Menu */
    GtkWidget *options_menu = gtk_menu_new();
    GtkWidget *options_item = gtk_menu_item_new_with_label("Options");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(options_item), options_menu);

    GtkWidget *item;

    item = gtk_check_menu_item_new_with_label("Always on top");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.always_on_top);
    g_signal_connect(item, "activate", G_CALLBACK(on_aot_toggled), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    item = gtk_check_menu_item_new_with_label("Double size");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.doublesize);
    g_signal_connect(item, "activate", G_CALLBACK(on_doublesize_toggled), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    item = gtk_check_menu_item_new_with_label("Snap windows");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.snap_windows);
    g_signal_connect(item, "activate", G_CALLBACK(on_snap_toggled), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    item = gtk_check_menu_item_new_with_label("Save window positions");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.save_window_position);
    g_signal_connect(item, "activate", G_CALLBACK(on_save_pos_toggled), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    item = gtk_check_menu_item_new_with_label("Save configuration on quit");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.save_config_on_quit);
    g_signal_connect(item, "activate", G_CALLBACK(on_save_on_quit_toggled), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(options_menu), item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), options_item);

    /* Audio Menu */
    GtkWidget *audio_menu = gtk_menu_new();
    GtkWidget *audio_item = gtk_menu_item_new_with_label("Audio");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(audio_item), audio_menu);

    /* Output Plugins Submenu */
    GtkWidget *output_menu = gtk_menu_new();
    GtkWidget *output_item = gtk_menu_item_new_with_label("Output Plugin");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(output_item), output_menu);

    GList *olist = get_output_list();
    OutputPlugin *cp = get_current_output_plugin();
    GSList *group = NULL;
    int i = 0;
    while (olist) {
        OutputPlugin *op = (OutputPlugin *)olist->data;
        GtkWidget *r_item = gtk_radio_menu_item_new_with_label(group, op->description);
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(r_item));
        if (op == cp) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(r_item), TRUE);
        g_signal_connect(r_item, "activate", G_CALLBACK(on_output_plugin_selected), GINT_TO_POINTER(i));
        gtk_menu_shell_append(GTK_MENU_SHELL(output_menu), r_item);
        olist = olist->next;
        i++;
    }
    gtk_menu_shell_append(GTK_MENU_SHELL(audio_menu), output_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), audio_item);

    /* Classic Preferences Entry */
    GtkWidget *prefs_item = gtk_menu_item_new_with_label("Preferences...");
    g_signal_connect(prefs_item, "activate", G_CALLBACK(show_prefs_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), prefs_item);

    gtk_widget_show_all(menubar);
    gtkosx_application_set_menu_bar(theApp, GTK_MENU_SHELL(menubar));
    
    /* Move Preferences to App Menu */
    GtkWidget *osx_prefs = gtk_menu_item_new_with_label("Preferences");
    g_signal_connect(osx_prefs, "activate", G_CALLBACK(show_prefs_window), NULL);
    gtkosx_application_insert_app_menu_item(theApp, osx_prefs, 1);
    
    gtkosx_application_ready(theApp);
}
#endif
