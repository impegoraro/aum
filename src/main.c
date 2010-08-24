/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Ilan Moreira Pegoraro 2010 <iemoreirap@gmail.com>
 * 
 * alupdate is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * alupdate is distributed in the hope that it will be useful, but
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

#include "conf.h"
#include "util.h"
#include "error.h"

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

int
main(int argc, char *argv[]) 
{
	int exit_code = EXIT_SUCCESS;
	AumConfig conf;
	GError *err = NULL;

	memset(&conf, 0, sizeof(conf));
	if(!aum_parse_config(&conf, &err)) {
		printf("Impossible to parse the configuration file: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	if(!aum_initialize_alpm(&conf, &err)){
		printf("Impossible to initialize the library: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
		goto cleanup;	
	}



	
cleanup:
	if(!aum_release_alpm(&err)) {
		printf("Impossible to initialize the library: %s\n",err->message);
		g_clear_error(&err);
		exit_code = EXIT_FAILURE;
	}	
	aum_config_free_inner(&conf);
	
	exit(exit_code);
}