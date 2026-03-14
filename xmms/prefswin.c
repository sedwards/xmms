#include "xmms.h"
#include <gtk/gtk.h>
#include "prefswin.h"
#include "skin.h"
#include "libxmms/util.h"
#include "log.h"

#ifdef HAVE_GTK_MAC
#include <gtkosxapplication.h>
#endif

static GtkWidget *prefswin = NULL;
static GtkWidget *prefswin_notebook;
static GtkListStore *in_store, *ef_store, *gen_store, *vis_store;
static GtkWidget *prefswin_audio_olist;

static void prefswin_destroy_cb(GtkWidget *widget, gpointer data)
{
    prefswin = NULL;
}

static void on_save_clicked(GtkWidget *widget, gpointer data)
{
    xmms_log("Preferences: Saving configuration...");
    save_config();
}

static void on_save_on_quit_toggled(GtkToggleButton *widget, gpointer data)
{
    cfg.save_config_on_quit = gtk_toggle_button_get_active(widget);
}

static void add_input_plugins(GtkListStore *store)
{
    GList *ilist = get_input_list();
    GtkTreeIter iter;
    gtk_list_store_clear(store);
    
    while (ilist) {
        InputPlugin *ip = (InputPlugin *)ilist->data;
        char *display_desc = g_strdup(ip->description);
        if (g_list_find(disabled_iplugins, ip)) {
            char *tmp = g_strconcat(display_desc, " (disabled)", NULL);
            g_free(display_desc);
            display_desc = tmp;
        }
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, display_desc, 1, ip, -1);
        g_free(display_desc);
        ilist = ilist->next;
    }
}

static void add_output_plugins(GtkComboBoxText *combo)
{
    GList *olist = get_output_list();
    OutputPlugin *cp = get_current_output_plugin();
    int i = 0, active_index = 0;

    gtk_combo_box_text_remove_all(combo);
    while (olist) {
        OutputPlugin *op = (OutputPlugin *)olist->data;
        gtk_combo_box_text_append_text(combo, op->description);
        if (op == cp) active_index = i;
        olist = olist->next;
        i++;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), active_index);
}

static void on_output_plugin_changed(GtkComboBox *widget, gpointer data)
{
    int index = gtk_combo_box_get_active(widget);
    if (index >= 0) {
        xmms_log("Switching output plugin to index %d", index);
        set_current_output_plugin(index);
    }
}

static GtkWidget* create_plugin_page(const char *title, GtkListStore **store_ptr)
{
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    GtkWidget *frame = gtk_frame_new(title);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
    GtkWidget *ivbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(ivbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), ivbox);

    *store_ptr = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
    GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(*store_ptr));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Plugin", renderer, "text", 0, NULL);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scrolled), view);
    gtk_box_pack_start(GTK_BOX(ivbox), scrolled, TRUE, TRUE, 0);

    return vbox;
}

static void on_always_on_top_toggled(GtkToggleButton *widget, gpointer data)
{
    cfg.always_on_top = gtk_toggle_button_get_active(widget);
    mainwin_set_always_on_top(cfg.always_on_top);
}

static gboolean on_doublesize_idle(gpointer data)
{
    mainwin_recreate();
    return FALSE;
}

static void on_doublesize_toggled(GtkToggleButton *widget, gpointer data)
{
    cfg.doublesize = gtk_toggle_button_get_active(widget);
    g_idle_add(on_doublesize_idle, NULL);
}

static void on_snap_windows_toggled(GtkToggleButton *widget, gpointer data)
{
    cfg.snap_windows = gtk_toggle_button_get_active(widget);
}

static void on_save_window_pos_toggled(GtkToggleButton *widget, gpointer data)
{
    cfg.save_window_position = gtk_toggle_button_get_active(widget);
}

void show_prefs_window(void)
{
    if (prefswin) {
        gtk_window_present(GTK_WINDOW(prefswin));
        return;
    }

    xmms_log("Opening preferences window...");
    prefswin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(prefswin), "XMMS Preferences");
    gtk_window_set_default_size(GTK_WINDOW(prefswin), 600, 500);
    g_signal_connect(prefswin, "destroy", G_CALLBACK(prefswin_destroy_cb), NULL);

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 10);
    gtk_container_add(GTK_CONTAINER(prefswin), main_vbox);

    prefswin_notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), prefswin_notebook, TRUE, TRUE, 0);

    /* 1. Audio I/O Page */
    GtkWidget *audio_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(audio_vbox), 10);

    GtkWidget *iframe = gtk_frame_new("Input Plugins");
    gtk_box_pack_start(GTK_BOX(audio_vbox), iframe, TRUE, TRUE, 0);
    in_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
    GtkWidget *in_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(in_store));
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(in_view), -1, "Plugin", gtk_cell_renderer_text_new(), "text", 0, NULL);
    GtkWidget *in_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(in_scroll), in_view);
    gtk_container_add(GTK_CONTAINER(iframe), in_scroll);

    GtkWidget *oframe = gtk_frame_new("Output Plugin");
    gtk_box_pack_start(GTK_BOX(audio_vbox), oframe, FALSE, FALSE, 0);
    prefswin_audio_olist = gtk_combo_box_text_new();
    g_signal_connect(prefswin_audio_olist, "changed", G_CALLBACK(on_output_plugin_changed), NULL);
    gtk_container_add(GTK_CONTAINER(oframe), prefswin_audio_olist);

    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), audio_vbox, gtk_label_new("Audio I/O"));

    /* 2. Effect/General Plugins */
    GtkWidget *plugins_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), plugins_vbox, gtk_label_new("Plugins"));
    gtk_box_pack_start(GTK_BOX(plugins_vbox), create_plugin_page("Effect Plugins", &ef_store), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(plugins_vbox), create_plugin_page("General Plugins", &gen_store), TRUE, TRUE, 0);

    /* 3. Visualization */
    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), create_plugin_page("Visualization Plugins", &vis_store), gtk_label_new("Visualization"));

    /* 4. Options */
    GtkWidget *opt_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(opt_vbox), 10);
    
    GtkWidget *opt_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(opt_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(opt_grid), 20);
    gtk_box_pack_start(GTK_BOX(opt_vbox), opt_grid, FALSE, FALSE, 0);

    /* Grouped Options */
    GtkWidget *cb_aot = gtk_check_button_new_with_label("Always on top");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_aot), cfg.always_on_top);
    g_signal_connect(cb_aot, "toggled", G_CALLBACK(on_always_on_top_toggled), NULL);
    gtk_grid_attach(GTK_GRID(opt_grid), cb_aot, 0, 0, 1, 1);

    GtkWidget *cb_ds = gtk_check_button_new_with_label("Double size");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_ds), cfg.doublesize);
    g_signal_connect(cb_ds, "toggled", G_CALLBACK(on_doublesize_toggled), NULL);
    gtk_grid_attach(GTK_GRID(opt_grid), cb_ds, 1, 0, 1, 1);

    GtkWidget *cb_sw = gtk_check_button_new_with_label("Snap windows");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_sw), cfg.snap_windows);
    g_signal_connect(cb_sw, "toggled", G_CALLBACK(on_snap_windows_toggled), NULL);
    gtk_grid_attach(GTK_GRID(opt_grid), cb_sw, 0, 1, 1, 1);

    GtkWidget *cb_swp = gtk_check_button_new_with_label("Save window positions");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_swp), cfg.save_window_position);
    g_signal_connect(cb_swp, "toggled", G_CALLBACK(on_save_window_pos_toggled), NULL);
    gtk_grid_attach(GTK_GRID(opt_grid), cb_swp, 1, 1, 1, 1);
    
    GtkWidget *cb_save = gtk_check_button_new_with_label("Save configuration on quit");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_save), cfg.save_config_on_quit);
    g_signal_connect(cb_save, "toggled", G_CALLBACK(on_save_on_quit_toggled), NULL);
    gtk_grid_attach(GTK_GRID(opt_grid), cb_save, 0, 2, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), opt_vbox, gtk_label_new("Options"));

    /* 5. Fonts */
    GtkWidget *fonts_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(fonts_vbox), 10);
    gtk_box_pack_start(GTK_BOX(fonts_vbox), gtk_label_new("Playlist Font:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fonts_vbox), gtk_entry_new(), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fonts_vbox), gtk_label_new("Main Window Font:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fonts_vbox), gtk_entry_new(), FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), fonts_vbox, gtk_label_new("Fonts"));

    /* 6. Title */
    GtkWidget *title_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(title_vbox), 10);
    gtk_box_pack_start(GTK_BOX(title_vbox), gtk_label_new("Title Format:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(title_vbox), gtk_entry_new(), FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(prefswin_notebook), title_vbox, gtk_label_new("Title"));

    /* Bottom Buttons */
    GtkWidget *bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(bbox), 5);
    gtk_box_pack_start(GTK_BOX(main_vbox), bbox, FALSE, FALSE, 0);

    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(bbox), btn_save);

    GtkWidget *btn_close = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(btn_close, "clicked", G_CALLBACK(gtk_widget_destroy), prefswin);
    gtk_container_add(GTK_CONTAINER(bbox), btn_close);

    /* Populate Data */
    add_input_plugins(in_store);
    add_output_plugins(GTK_COMBO_BOX_TEXT(prefswin_audio_olist));

    gtk_widget_show_all(prefswin);
}

void create_prefs_window(void) {}
void prefswin_vplugins_rescan(void) {}
void prefswin_show_vis_plugins_page(void) {}
