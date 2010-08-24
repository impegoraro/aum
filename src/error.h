/************************************************************************
 * Name: error.c                                                        *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 0.1                                                          *
 *                                                                      *
 * Description: Error codes.                                            *
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
 

#ifndef _AUM_ERROR_H_
#define _AUM_ERROR_H_

enum {
	AUM_ERROR_NOMEM, 
	AUM_ERROR_NOLOCALDB,
	AUM_ERROR_NOINDX,
	AUM_ERROR_NOFILE,
	AUM_ERROR_ESYNTAX,
	AUM_ERROR_EVAL,
	AUM_ERROR_NOARCH,
	AUM_ERROR_EWRONGVAL,
	AUM_ERROR_EWRONGDIRECTIVE,
	AUM_ERROR_EROOT,
	AUM_ERROR_EDB,
	AUM_ERROR_ELOG,
	AUM_ERROR_EARCH,
	AUM_ERROR_ECACHE,
	AUM_ERROR_EIGNOREPKG,
	AUM_ERROR_EIGNOREGRP,
	AUM_ERROR_ENOUPGRADE,
	AUM_ERROR_ENOEXTRACT,
	AUM_ERROR_EISINIT,
	AUM_ERROR_EINIT,
	AUM_ERROR_ERELEASE
};

#endif
