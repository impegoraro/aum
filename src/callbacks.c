/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <string.h>
#include <stdlib.h>
#include <glib/gtypes.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkprogressbar.h>

#include "callbacks.h"

GtkProgressBar *cb_download_pbar = NULL;

void
aum_cb_totaldl(off_t total)
{
	printf("%lu\n",total);
}

void
aum_cb_download(const char *filename, off_t xfered, off_t total)
{
	char *text, *p;
	gdouble res = (gdouble) ((gdouble) xfered / (gdouble) total);
	
	if(cb_download_pbar != NULL) {
		/*gtk_progress_bar_set_fraction(cb_download_pbar, res);*/
		text = strdup(filename);
		if((p = strstr(text, ".db.tar.")))
			*p = '\0';

		gtk_progress_bar_set_text(cb_download_pbar, text);
		free(text);
		while(gtk_events_pending())
			gtk_main_iteration();
	}
}
