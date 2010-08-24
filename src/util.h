/************************************************************************
 * Name: util.h                                                         *
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
#ifndef _AUM_UTIL_H_
#define _AUM_UTIL_H_

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <glib/gquark.h>

#include <alpm_list.h>

#define AUM_UTIL_ERROR aum_util_error()


/*
 * This function deletes the extra spaces at the begining and at the end of the string. 
 * The operation is performed on the same memory block, no extra memory is allocated.
 * Returns either a pointer to the string or NULL if an error occurs
 * On NULL nothing is done and returns NULL.
 * Errors: No errors are returned.
 *
 * Param:
 *       	char* - string to trim.
 */
char* strtrim(char*);

/*
 * Returns: A new string holding the substring, Note that the new space is allocated with
 * the malloc family functions and must be freed with free.
 *
 * Param:
 *       	const char*      - source string
 *       	unsigned int     - start index of the substring
 *       	unsigned int     - how many characters to copy
 *          GError**         - A location to store the error, pass NULL to ignore the error
 */
char* substring(const char *, unsigned int, unsigned int, GError**);

/*
 * Replaces a token within the string, Note that the memory is allocated dinamically and must free freed.
 *
 * Param:
 *        const char*    - the source string
 *        const char*    - original token
 *        const char*    - 
 *        GError**       - A location to store the error, pass NULL to ignore the error
 */
char* strreplace(const char*, const char*, const char*, GError**);

/*
 * Returns a list of tokens extracted from the string. Note that the tokens and the list are dinamically allocated
 * they must be free.
 * 
 * Param:
 *        const char*  - source string
 *        char         - character to separate
 */
alpm_list_t* strsplit(const char* str, char c);


GQuark aum_util_error(void);

#endif
