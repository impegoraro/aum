/************************************************************************
 * Name: util.c                                                         *
 * Author: Ilan Moreira Pegoraro <iemoreirap@gmail.com>                 *
 * Version 1.0                                                          *
 *                                                                      *
 * Description: Utility functions                                       *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <alpm_list.h>

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <glib/gquark.h>

#include "util.h"
#include "error.h"

char*
strtrim(char* src) 
{
	unsigned int i, l;
	size_t len;

	if((src == NULL)) return src;

	len = strlen(src);

	if((len == 0)) return src; /* the string is empty */

	/* move the index i until src[i] is not a space */
	for(i=0 ; (src[i] != '\0' ) && (isspace(src[i])); i++); 
	/* same as the loop above, but this time we start at the end of the string*/	
	for(l = len - 1; (l  > i) && (isspace(src[l])); l--);	

	l++;
	
	src[l] = '\0';
	memmove(src,&src[i],(l-i)+1);

	return src;
}

char*
substring(const char *src, unsigned int first, unsigned int hmany, GError** err) 
{
	char *dest = NULL;

	assert(src != NULL);

	/* invalid index */
	if((first >= strlen(src)) && (hmany >= strlen(&src[first]))) {
		GError *tmp_err = NULL;
		g_set_error(&tmp_err, AUM_UTIL_ERROR, AUM_ERROR_NOINDX, "invalid index");
		g_propagate_error(err,tmp_err);
		return NULL;
	}

	if((dest = calloc(sizeof(char), hmany + 1)) == NULL) {
		GError *tmp_err = NULL;

		g_set_error(&tmp_err, AUM_UTIL_ERROR, AUM_ERROR_NOMEM, "unable to allocate memory");
		g_propagate_error(err,tmp_err);
		return NULL;
	}
	memcpy(dest,&src[first], hmany);
	dest[hmany+1] ='\0';
	return dest;
}

char* 
strreplace(const char* src, const char* orig, const char* sub, GError** err)
{
	char *ptr = NULL, *dest = NULL;
	size_t len;
	
	assert(src != NULL && orig != NULL && sub != NULL);

	if((ptr = strstr(src, orig)) == NULL) {
		GError *tmp_error = NULL;
		
		g_set_error(&tmp_error, AUM_UTIL_ERROR, AUM_ERROR_NOMEM, "unable to allocate memory");
		g_propagate_error(err, tmp_error);
		return NULL;
	}

	len = (ptr-src) + ((src + strlen(src)) - (ptr + strlen(orig)) + strlen(sub));
	if((dest = (char*) calloc(sizeof(char), (len + 1))) != NULL) {
		strncpy(dest,src, ptr-src);
		strcat(dest,sub);
		strcat(dest,ptr+strlen(orig));
	}
	return dest;
}

alpm_list_t* 
strsplit(const char* str, char c)
{
	alpm_list_t *list = NULL;
	char *tmp, *ptr = NULL,*ptr2;
	assert(str != NULL);
	
	tmp = strdup(str);
	ptr2 = tmp;
	
	for(ptr = strchr(tmp,c); ptr != NULL; ptr = strchr(ptr,c)){
		*ptr='\0';
		ptr++;
		list = alpm_list_add(list, strdup(ptr2));
		ptr2 = ptr;
	}
	if((ptr2 != NULL) && (strlen(ptr2) >= 0)) /* there is still an element left*/
		list = alpm_list_add(list, strdup(ptr2));
	free(tmp);
		
	return list;
}
/*************** Error Handlig Functions for this module **************/
GQuark 
aum_util_error(void) 
{
	return g_quark_from_static_string("aum-util-error");
}
