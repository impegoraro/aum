/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * aum.c
 * Copyright (C) Ilan Moreira Pegoraro 2010 <iemoreirap@gmail.com>
 * 
 * aum is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * aum is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include <gtk/gtkmain.h>
#include <glib/gerror.h>

#include "aum.h"

int
main(int argc, char *argv[]) 
{
	int exit_code = EXIT_SUCCESS;
	AumConfig conf;
	AumInterface *aumUI;
	alpm_list_t *lupkgs = NULL;
	GError *err = NULL;

	memset(&conf, 0, sizeof(conf));
	if(!aum_parse_config(&conf, &err)) {
		printf("failed to parse the configuration file: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	if(!aum_initialize_alpm(&conf, &err)){
		printf("failed to initialize the library: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	gtk_init(&argc, &argv);
	if((aumUI = aum_interface_new(&err)) == NULL) {
		printf("failed to load the interface: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	if(aum_get_upgradablepkgs(&lupkgs, &err) < 0) {
		printf("Error %s", err->message);
		g_clear_error(&err);
	}else {
		char *text;
		
		if(lupkgs == NULL) {
			text = g_markup_printf_escaped("<b>There are not new updates available for your system.</b>" \
			                               "\n<small>you can check for updates at any given time.</small>");
		}else{
			text = g_markup_printf_escaped("<b>There're %d new available updates.</b>" \
				                           "\n<small>select the packages that you want to install and " \
					                       "then click on the \"install\" button.</small>",alpm_list_count(lupkgs));
			aum_listupgrades_fill(aumUI->treeUpdates, lupkgs);
			alpm_list_free(lupkgs);
		}
		gtk_label_set_markup(aumUI->lblTitle, text);
		g_free(text);
	}
	/*hanging over control to gtk main loop*/ 
	gtk_main();
	
cleanup:
	if(!aum_release_alpm(&err)) {
		printf("failed to release the library: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
	}	

	g_free(aumUI);
	aum_config_free_inner(&conf);
	exit(exit_code);
}