/************************************************************************
 * Name: sync.h                                                         *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 1.0                                                          *
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

#ifndef _AUM_SYNC_H_
#define _AUM_SYNC_H_

#define AUM_SYNC_ERROR aum_sync_error_quark()

int aum_get_upgradablepkgs(alpm_list_t**, GError**);
void aum_refresh_databases(AumInterface*);
GQuark aum_sync_error_quark();
#endif