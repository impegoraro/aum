/************************************************************************
 * Name: main-window.c                                                  *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 0.1                                                          *
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

#include <gtk/gtkwindow.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtkbuilder.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcellrenderertoggle.h>
#include <gtk/gtkmain.h>


#include <glib.h>
#include <glib/gerror.h>
#include <glib/gquark.h>

#include "aum.h"

static void _on_renderer_toggle(GtkCellRendererToggle*, char*, GtkTreeModel*);
static void _on_window_destroy(GtkWindow *window, gpointer data);
static void _on_btnClose_clicked(GtkButton*, GtkWidget*);
static gboolean _on_window_delete_event(GtkWindow*, GdkEvent*, gpointer);


AumInterface*
aum_interface_new(GError **err)
{
	AumInterface *ui;
	GtkBuilder *builder;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkListStore *model;
	
	g_return_val_if_fail(err == NULL || *err == NULL, NULL);

	if(((ui = g_new(AumInterface, 1)) == NULL) || ((builder = gtk_builder_new()) == NULL)) {
		g_set_error(err, AUM_MAIN_WINDOW_ERROR, AUM_ERROR_NOMEM, "unable to allocate memory");
		if(ui != NULL)
			g_free(ui);
		return NULL;
	}

	if(!gtk_builder_add_from_file(builder, AUM_UI_FILE, err)){
		g_free(ui);
		g_object_unref(G_OBJECT(builder));
		return NULL;
	}

	ui->window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
	ui->lblTitle = GTK_LABEL(gtk_builder_get_object(builder, "lblTitle"));
	ui->treeUpdates = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeUpdates"));
	ui->btnRefresh = GTK_BUTTON(gtk_builder_get_object(builder, "btnRefresh"));
	ui->btnClose = GTK_BUTTON(gtk_builder_get_object(builder, "btnClose"));
	ui->btnInstall = GTK_BUTTON(gtk_builder_get_object(builder, "btnInstall"));

	g_object_unref(G_OBJECT(builder)); /* there's no need to keep the reference to builder */

	/*connecting signal to callback function*/
	g_signal_connect(G_OBJECT(ui->window), "destroy", G_CALLBACK(_on_window_destroy), NULL);
	g_signal_connect(G_OBJECT(ui->window), "delete_event", G_CALLBACK(_on_window_delete_event), NULL);
	g_signal_connect(G_OBJECT(ui->btnClose), "clicked", G_CALLBACK(_on_btnClose_clicked), ui->window);
	
	/*setting properties to treeUpdates*/
	model = gtk_list_store_new(AUM_LU_TOTAL, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(ui->treeUpdates, GTK_TREE_MODEL(model));
	
	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(_on_renderer_toggle), \
	                 gtk_tree_view_get_model(ui->treeUpdates));
	column = gtk_tree_view_column_new_with_attributes("Install", renderer, "active", AUM_LU_INSTALL, NULL);
	gtk_tree_view_append_column(ui->treeUpdates, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", AUM_LU_NAME, NULL);
	g_object_set(G_OBJECT(column), "expand", TRUE, "resizable", TRUE, "clickable", TRUE, NULL);
	gtk_tree_view_append_column(ui->treeUpdates, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Version", renderer, "text", AUM_LU_VERSION, NULL);
	g_object_set(G_OBJECT(column), "resizable", TRUE, NULL);
	gtk_tree_view_append_column(ui->treeUpdates, column);
	
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Size", renderer, "text", AUM_LU_SIZE, NULL);
	g_object_set(G_OBJECT(column), "resizable", TRUE, NULL);
	gtk_tree_view_append_column(ui->treeUpdates, column);

	/*setting properties for window*/
	gtk_window_set_icon_from_file(ui->window, PACKAGE_ICON, NULL);
	
	/*setting properties for btnInstall*/
	gtk_widget_set_sensitive(GTK_WIDGET(ui->btnInstall), FALSE);

	gtk_widget_show_all(GTK_WIDGET(ui->window));	/*showing all widgets*/
	return ui;
}


GQuark 
aum_main_window_error_quark()
{
	return g_quark_from_static_string("aum-main-window-error");
}


/******************************* callback functions *******************************/
static void
_on_renderer_toggle(GtkCellRendererToggle *toggle, char *path, GtkTreeModel *model) 
{
	GtkTreeIter iter;
	gboolean install;
	install = gtk_cell_renderer_toggle_get_active(toggle);

	if(gtk_tree_model_get_iter_from_string(model,&iter, path))
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, AUM_LU_INSTALL, !install, -1);
}

static void
_on_window_destroy(GtkWindow *window, gpointer data)
{
	gtk_main_quit();
}

static gboolean
_on_window_delete_event(GtkWindow *wnd, GdkEvent *event, gpointer data) 
{
	return FALSE;
}

static void
_on_btnClose_clicked(GtkButton *btn, GtkWidget *widget)
{
	gboolean res;

	g_signal_emit_by_name(G_OBJECT(widget), "delete_event", NULL, &res);
	if(!res)
		gtk_widget_destroy(widget);
}