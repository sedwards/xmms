#include <config.h>
#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include <stdlib.h>
#include <string.h>
#include "xmms/plugin.h"
#include "xmms/i18n.h"
#include "libxmms/configfile.h"

static GtkWidget *configure_win, *options_colorpicker;

static struct
{
	guint32 color;
} bscope_cfg;

void bscope_load_config(void)
{
	ConfigFile *cfg;

	bscope_cfg.color = 0x000000;
	if ((cfg = xmms_cfg_open_default_file()) != NULL)
	{
		xmms_cfg_read_int(cfg, "BlurScope", "color", (gint *)&bscope_cfg.color);
		xmms_cfg_free(cfg);
	}
}

static void bscope_save_config(void)
{
	ConfigFile *cfg;

	cfg = xmms_cfg_open_default_file();
	xmms_cfg_write_int(cfg, "BlurScope", "color", (gint)bscope_cfg.color);
	xmms_cfg_write_default_file(cfg);
	xmms_cfg_free(cfg);
}

static void config_ok_cb(GtkWidget * widget, gpointer data)
{
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(options_colorpicker), &color);
    
    bscope_cfg.color = ((guint32)(color.red * 255) << 16) | 
                       ((guint32)(color.green * 255) << 8) | 
                       ((guint32)(color.blue * 255));
                       
	bscope_save_config();
	gtk_widget_destroy(configure_win);
    configure_win = NULL;
}

void bscope_configure(void)
{
	GtkWidget *vbox, *bbox, *ok, *cancel;
    GdkRGBA rgba;

	if (configure_win)
	{
		gdk_window_raise(gtk_widget_get_window(configure_win));
		return;
	}

	configure_win = gtk_dialog_new_with_buttons(_("Blur Scope Configuration"),
                                               NULL, GTK_DIALOG_MODAL,
                                               _("Ok"), GTK_RESPONSE_OK,
                                               _("Cancel"), GTK_RESPONSE_CANCEL,
                                               NULL);

	g_signal_connect(configure_win, "destroy", G_CALLBACK(gtk_widget_destroyed), &configure_win);

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(configure_win));
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

	options_colorpicker = gtk_color_chooser_widget_new();
    
    rgba.red = ((bscope_cfg.color >> 16) & 0xff) / 255.0;
    rgba.green = ((bscope_cfg.color >> 8) & 0xff) / 255.0;
    rgba.blue = (bscope_cfg.color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(options_colorpicker), &rgba);
    
	gtk_box_pack_start(GTK_BOX(vbox), options_colorpicker, TRUE, TRUE, 0);

	gtk_widget_show_all(configure_win);
    
    if (gtk_dialog_run(GTK_DIALOG(configure_win)) == GTK_RESPONSE_OK) {
        config_ok_cb(NULL, NULL);
    } else {
        gtk_widget_destroy(configure_win);
        configure_win = NULL;
    }
}
