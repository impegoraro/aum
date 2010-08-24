/************************************************************************
 * Name: config.h                                                       *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 0.3                                                          *
 *                                                                      *
 * Description: Configuration functions and structures                  *
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

#ifndef _AUM_CONF_H_
#define _AUM_CONF_H_

#include <alpm_list.h>

#include <glib/gerror.h>

#define AUM_CONFIG_ERROR aum_config_error()

typedef struct {
	char* name;
	alpm_list_t* servers;  /* List of strings (URLs) */
}Repository;

typedef struct {
	char* root;
	char* dbpath;
	char* logfile;
	char* architecture;
	
	alpm_list_t* caches;   /* List of strings */
	
	alpm_list_t* repos;   /* List of Repositories*/
	
	alpm_list_t* hold;    /* List of strings */
	alpm_list_t* sync;    /* List of strings */
	alpm_list_t* ignorePkg;    /* List of strings */
	alpm_list_t* ignoreGrp;    /* List of strings */
	alpm_list_t* noUpgrade;    /* List of strings */
	alpm_list_t* noExtract;    /* List of strings */
}AumConfig;	

int aum_parse_config(AumConfig*, GError**);
unsigned short aum_initialize_alpm(AumConfig*, GError**);
unsigned short aum_release_alpm(GError **);
void aum_config_free_inner(AumConfig*);

GQuark aum_config_error();

#endif