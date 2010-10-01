/************************************************************************
 * Name: sync.c                                                         *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 0.1                                                          *
 *                                                                      *
 * Description:                                                         *
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
#include <assert.h>
#include <unistd.h>

#include <glib/gerror.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkmessagedialog.h>

#include <alpm.h>
#include <alpm_list.h>

#include "aum.h"
#include "main-window.h"
#include "callbacks.h"

extern GtkProgressBar *cb_download_pbar;

int
aum_get_upgradablepkgs(alpm_list_t** upkgs, GError** err)
{
	alpm_list_t *uppkgs = NULL, *syncdbs, *i;
	int hmany = 0;
	pmdb_t *localdb = NULL;
	pmpkg_t *spkg;
	
	assert(upkgs != NULL && *upkgs == NULL);

	localdb = alpm_option_get_localdb();
	if(localdb == NULL) {
		localdb = alpm_db_register_local();
		if(localdb == NULL) {
			g_set_error(err, AUM_SYNC_ERROR, AUM_ERROR_NOLOCALDB, "local database not found");
			return -1;
		}
	}
	
	syncdbs = alpm_option_get_syncdbs();
	for(i = alpm_db_get_pkgcache(localdb); i != NULL; i = alpm_list_next(i)) {
		spkg = alpm_sync_newversion((pmpkg_t*) alpm_list_getdata(i), syncdbs);
		if(spkg != NULL)
			/* there is a new version of spkg */
			uppkgs = alpm_list_add(uppkgs, (void *) spkg);
		++hmany;
	}
	*upkgs = uppkgs;
	return hmany;
}

void
aum_refresh_databases(AumInterface *ui)
{
	alpm_list_t *sync_dbs, *i;
	pmdb_t *db;
	uid_t userid;
	int resupdate = 1, noupdated = 0;
	
	userid = getuid();
	if(userid != 0) {
		/*this operations needs root privileges*/
		GtkMessageDialog *msgbox;

		msgbox = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(ui->window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, \
		                                                   GTK_BUTTONS_OK, "Unable to refresh the repositories."));
		gtk_message_dialog_format_secondary_text(msgbox, "You do not have enough privileges");
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(GTK_WIDGET(msgbox));
		return;
	}

	sync_dbs = alpm_option_get_syncdbs();
	if(sync_dbs == NULL) {
		/*there are no repositories registered*/
		GtkMessageDialog *msgbox;

		msgbox = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(ui->window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, \
		                                                   GTK_BUTTONS_OK, "Unable to refresh the repositories."));
		gtk_message_dialog_format_secondary_text(msgbox, "There are no repositories registered. Check your" \
		                                         " configuration file to check that your repositories are configured" \
		                                         " properly.");
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(GTK_WIDGET(msgbox));
		return;
	}
	alpm_option_set_dlcb(aum_cb_download);

	if(alpm_trans_init(0, NULL, NULL, NULL) != 0){
		GtkMessageDialog *msgbox;

		msgbox = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(ui->window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, \
		                                                   GTK_BUTTONS_OK, "Transaction could not be initialize"));
		gtk_message_dialog_format_secondary_text(msgbox, "%s", alpm_strerrorlast());
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(GTK_WIDGET(msgbox));
		return;
	}
	cb_download_pbar = ui->pbarTransaction;
	gtk_widget_set_visible(GTK_WIDGET(ui->boxAction), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(ui->btnInstall), FALSE);
	for( i = sync_dbs; i != NULL; i = alpm_list_next(i)) {
		db = (pmdb_t*) alpm_list_getdata(i);
		resupdate = alpm_db_update(1, db);

		if(resupdate > 0) {
			/*error while updating the database*/
			++noupdated;
		}
	}
	gtk_widget_set_sensitive(GTK_WIDGET(ui->btnInstall), TRUE);
	cb_download_pbar = NULL;
	if(alpm_trans_release() != 0) {
		GtkMessageDialog *msgbox;

		msgbox = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(ui->window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, \
		                                                   GTK_BUTTONS_OK, "Transaction could not be released"));
		gtk_message_dialog_format_secondary_text(msgbox, "%s", alpm_strerrorlast());
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(GTK_WIDGET(msgbox));
	}
	if(noupdated != 0) {
		GtkMessageDialog *msgbox;

		msgbox = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(ui->window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, \
		                                                   GTK_BUTTONS_OK, "Refreshing repositories"));
		gtk_message_dialog_format_secondary_text(msgbox, "Some repositories we're not synchronized...");
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(GTK_WIDGET(msgbox));
	}
	gtk_widget_set_visible(GTK_WIDGET(ui->boxAction), FALSE);
}

GQuark
aum_sync_error_quark()
{
	return g_quark_from_static_string("aum-sync-error-quark");
}