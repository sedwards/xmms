/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Zinx Verituse
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _XMMS_FULLSCREEN_H
#define _XMMS_FULLSCREEN_H 1

#include <gtk/gtk.h>

typedef struct {
	gint width, height;
} xmms_fullscreen_mode_t;

#ifdef __cplusplus
extern "C" {
#endif

gboolean xmms_fullscreen_available(GdkDisplay *dpy);
gboolean xmms_fullscreen_init(GtkWidget *win);

gboolean xmms_fullscreen_enter(GtkWidget *win, gint *w, gint *h);
void xmms_fullscreen_leave(GtkWidget *win);

gboolean xmms_fullscreen_in(GtkWidget *win);
gboolean xmms_fullscreen_mark(GtkWidget *win);
void xmms_fullscreen_unmark(GtkWidget *win);

void xmms_fullscreen_cleanup(GtkWidget *win);

GSList *xmms_fullscreen_modelist(GtkWidget *win);
void xmms_fullscreen_modelist_free(GSList *modes);

#ifdef __cplusplus
}
#endif


#endif

