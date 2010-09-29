/************************************************************************
 * Name: main-window.c                                                  *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 0.3                                                          *
 *                                                                      *
 * Description: User Interface                                          *
 ***********************************************************************/
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

#ifndef _AUM_MAIN_WINDOW_H_
#define _AUM_MAIN_WINDOW_H_

#include <gtk/gtkwindow.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkprogressbar.h>

#define AUM_MAIN_WINDOW_ERROR aum_main_window_error_quark()


enum {
	AUM_LU_INSTALL = 0,
	AUM_LU_NAME,
	AUM_LU_VERSION,
	AUM_LU_SIZE,
	AUM_LU_TOTAL
};

typedef struct {
	GtkWindow *window;
	GtkLabel *lblTitle;

	GtkTreeView *treeUpdates;
	
	GtkButton *btnRefresh;
	GtkButton *btnClose;
	GtkButton *btnInstall;
	
	GtkBox *boxAction;
	GtkLabel *lblAction;
	GtkProgressBar *pbarTransaction;
} AumInterface;


AumInterface* aum_interface_new(GError**);
void aum_listupgrades_fill(GtkTreeView*, alpm_list_t*);

GQuark aum_main_window_error_quark();


#endif
