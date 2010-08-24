/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * aum.h
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

#ifndef _AUM_H_
#define _AUM_H_

#include "util.h"
#include "error.h"
#include "main-window.h"
#include "conf.h"
#include "sync.h"

/* For testing propose use the local (not installed) ui file */
/* #define AUM_UI_FILE PACKAGE_DATA_DIR"/aum/ui/aum.ui" */
#define AUM_UI_FILE "src/aum.ui"
#define PACKAGE_ICON "/usr/share/icons/gnome/48x48/status/software-update-available.png"

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


#endif 