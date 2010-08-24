/************************************************************************
 * Name: config.c                                                       *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/utsname.h>

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <glib/gquark.h>

/* archlinux package manager */
#include <alpm.h>
#include <alpm_list.h>

#include "conf.h"
#include "error.h"
#include "util.h"

#define MAXBUFFER 1024

static unsigned short isInitialized = 0;

static int _parseconfig(const char* const, const char*, AumConfig*, GError**);
static int _init(AumConfig*, GError **);

/*
 * Returns 0 on failure, 1 on success
 */
unsigned short
aum_initialize_alpm(AumConfig* conf, GError** err) 
{
	GError *error = NULL;

	assert(((err != NULL) && (*err == NULL) && (conf != NULL)));
	
	if(isInitialized) return isInitialized;

	if(alpm_initialize() != 0) {
		g_set_error(err, AUM_CONFIG_ERROR, AUM_ERROR_EINIT, "The library was not initilized, Cause: %s", \
					alpm_strerrorlast());
		return 0;
	}

	isInitialized = 1;
	if((!_init(conf,&error))) {
		isInitialized = 0;
		g_propagate_error(err,error);
		return 0;
	}

	return isInitialized;
}

/*
 * Returns 0 on failure, 1 on success
 */
unsigned short 
aum_release_alpm(GError **err) 
{
	
	if(!isInitialized) {
		g_set_error(err,AUM_CONFIG_ERROR, AUM_ERROR_EISINIT,"library is not intilialized");
		return 0;
	}
	
	if(alpm_release() != 0){
		g_set_error(err,AUM_CONFIG_ERROR, AUM_ERROR_ERELEASE, "%s",alpm_strerrorlast());
		return 0;
	}
	return 1;
}

void 
aum_config_free_inner(AumConfig* c)
{
	alpm_list_t *i = NULL, *j = NULL;
	void* data;
	assert(c != NULL);
	
	if(c != NULL) {
		if(c->root != NULL) free(c->root);
		if(c->dbpath != NULL) free(c->dbpath);
		if(c->logfile != NULL) free(c->logfile);
		if(c->architecture != NULL) free(c->architecture);
		
		for(i = c->repos; i != NULL; i = alpm_list_next(i)) {
			data = alpm_list_getdata(i);
			free(((Repository*)data)->name);
			for(j = ((Repository*)data)->servers; j != NULL; j = j->next) 
				free((char*)j->data);
			alpm_list_free(((Repository*)data)->servers);
		}
		alpm_list_free(c->repos);
		
		for(i = c->hold; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->hold);
		for(i = c->sync; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->sync);
		for(i = c->ignorePkg; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->ignorePkg);
		for(i = c->ignoreGrp; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->ignoreGrp);
		for(i = c->noUpgrade; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->noUpgrade);
		for(i = c->noExtract; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->noExtract);
		for(i = c->caches; i != NULL; i = alpm_list_next(i))
			free(alpm_list_getdata(i));
		alpm_list_free(c->caches);

	}
}

int 
aum_parse_config(AumConfig *conf, GError **err)
{
	return (_parseconfig(CONFFILE,NULL, conf, err));
}

static int
_parseconfig(const char* const filename, const char* sec, AumConfig* conf, GError **err) 
{
	FILE *file2parse = NULL;
	char buffer[MAXBUFFER + 1];
	char *section = NULL, *ptr = NULL;
	size_t len = 0;
	unsigned int line = 0;
	int exit_code = 1;
	Repository* repo = NULL;
	GError *error = NULL;
	
	assert(conf != NULL);

	if( (file2parse = fopen(filename,"r")) == NULL) {
		/*Error open the file*/
		g_set_error(&error,AUM_CONFIG_ERROR, AUM_ERROR_NOFILE,"file '%s' could not be opened", filename);
		g_propagate_error(err, error);
		exit_code = 0;
		goto cleanup;
	}

	if(sec != NULL) {
		alpm_list_t *i;
		Repository *data;
		section = strdup(sec);
		for(i = conf->repos; i != NULL; i = alpm_list_next(i)) {
			data = alpm_list_getdata(i);
			if(!strcmp(data->name,section)) {
				repo = data;
				break;
			}
		}
	}
	
	while(fgets(buffer, MAXBUFFER, file2parse) != NULL) {
		++line;
		ptr = strchr(buffer,'#');
		if(ptr != NULL) *ptr = '\0';        /* ignore the comments */
		strtrim(buffer);

		if(*buffer == '\0') continue;       /* ignore blank line */
		len = strlen(buffer);  
		
		if(buffer[0] == '[' && buffer[len-1] == ']') {
			/* new section */
			if(section != NULL)
				free(section);
			if((section = substring(buffer,1, len - 2, &error)) == NULL) {
				g_propagate_error(err, error);
				exit_code = 0;
				goto cleanup;
			}
			strtrim(section); /* in case there're space's inside the square brackets*/
			if(strcmp(section,"options")){ /* if section is a repository */
				if(((repo = calloc(sizeof(Repository),1)) == NULL)){
					g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_NOMEM,"could not allocate enough memory");
					g_propagate_error(err, error);
					exit_code = 0;
					goto cleanup;
				}
				memset(repo,0,sizeof(Repository));
				repo->name = strdup(section);
				conf->repos = alpm_list_add(conf->repos, repo);
			}
		}else {
			if(section == NULL) {
				/* syntax error, every key must belong to a section.*/
				g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_ESYNTAX, \
							"syntax error at line %d, every key must belong to a section",line);
				g_propagate_error(err, error);
				exit_code = 0;
				goto cleanup;
			}
			
			ptr = strchr(buffer,'=');
			if(ptr != NULL) {
				/* Directive with value */
				*ptr = '\0'; 
				strtrim(buffer);
				strtrim(++ptr);
				if(ptr == NULL || strlen(ptr) <= 0){
					g_set_error(&error,AUM_CONFIG_ERROR,AUM_ERROR_EWRONGVAL, \
								"at line %d, directive %s with an invalid value",line,buffer);
					g_propagate_error(err, error);
					exit_code = 0;
					goto cleanup;
				}
			}else 
				strtrim(buffer);
			if(!strcmp(section,"options")) {
				/* General Options */
				if(!strcmp(buffer,"RootDir")){
					if(conf->root == NULL)
						conf->root = strdup(ptr);
				}else if(!strcmp(buffer,"DBPath")) {
					if(conf->dbpath == NULL)
						conf->dbpath = strdup(ptr);
				}else if(!strcmp(buffer,"CacheDir")){
					if(conf->caches == NULL)
						conf->caches = strsplit(ptr,' ');
				}else if(!strcmp(buffer,"LogFile")) {
					if(conf->logfile == NULL)
						conf->logfile = strdup(ptr);
				}else if(!strcmp(buffer,"HoldPkg")) {					
					if(conf->hold == NULL)
						conf->hold = strsplit(ptr,' ');
				}else if(!strcmp(buffer,"SyncFirst")) {
					if(conf->sync == NULL)
						conf->sync = strsplit(ptr,' ');
				}else if(!strcmp(buffer,"Architecture")) {
					if(conf->architecture == NULL) {
						if(!strcmp(ptr,"auto")){
							struct utsname host;
							memset(&host, 0, sizeof(host));
							uname(&host);
							conf->architecture = strdup(host.machine);
						}else if(!strcmp(ptr,"i686") || !strcmp(ptr,"x86_64"))
							conf->architecture = strdup(ptr);
						else{
							g_set_error(&error,AUM_CONFIG_ERROR, AUM_ERROR_ESYNTAX, \
										"at line %d, wrong value for architecture",line);
							g_propagate_error(err, error);
							exit_code = 0;
							goto cleanup;
						}
					}
				}else if(!strcmp(buffer,"IgnorePkg")) {
					if(conf->ignorePkg == NULL)
						conf->ignorePkg = strsplit(ptr,' ');
				}else if(!strcmp(buffer,"IgnoreGroup")) {
					if(conf->ignoreGrp == NULL)
						conf->ignoreGrp = strsplit(ptr,' ');
				}else if(!strcmp(buffer,"NoUpgrade")) {
					if(conf->noUpgrade == NULL)	
						conf->noUpgrade = strsplit(ptr,' ');
				}
				else if(!strcmp(buffer,"NoExtract")) {
					if(conf->noExtract == NULL)
						conf->noExtract = strsplit(ptr,' ');
				}/*else{
					g_set_error(error,IPM_CONFIG_ERROR, IPM_CONFIG_EWRONGDIRECTIVE, \
								"Error at line %d, unknown directive %s",line,buffer);
					exit_code = 0;
					goto cleanup;
				}*/
				
			}else{
				/* Keys for a repository */
				if(!strcmp(buffer,"Server")){
					char *ptmp,*ptmp2;
					if((ptmp = strreplace(ptr,"$repo",section,NULL)) != NULL ){
						if((ptmp2 = strreplace(ptmp,"$arch",conf->architecture, NULL)) != NULL){
							repo->servers = alpm_list_add(repo->servers, strdup(ptmp2));
							free(ptmp2);
							ptmp2 = NULL;
						}else{
							repo->servers = alpm_list_add(repo->servers, strdup(ptmp));
						}
						free(ptmp);
						ptmp = NULL;
					}else if((ptmp = strreplace(ptr,"$arch",conf->architecture,NULL)) != NULL ){
						if((ptmp2 = strreplace(ptmp,"$repo",section,NULL)) != NULL){
							repo->servers = alpm_list_add(repo->servers, strdup(ptmp2));
							free(ptmp2);
							ptmp2 = NULL;
						}else{
							repo->servers = alpm_list_add(repo->servers, strdup(ptmp));
						}
						free(ptmp);	
						ptmp = NULL;
					}else{
						repo->servers = alpm_list_add(repo->servers, strdup(ptr));
					}
				}else if(!strcmp(buffer,"Include")) {
					if(!_parseconfig(ptr, section, conf, &error)) {
						g_propagate_error(err, error);
						exit_code = 0;
						goto cleanup;
					}
				}else{
					g_set_error(&error,AUM_CONFIG_ERROR, AUM_ERROR_EWRONGDIRECTIVE, \
								"at line %d, unknown directive %s", line, buffer);
					g_propagate_error(err, error);
					exit_code = 0;
					goto cleanup;
				}
				
			}
		}
	}

cleanup:
	if(file2parse != NULL) 
		fclose(file2parse);
	if(section != NULL) 
		free(section);
	/* undo conf if error occurs*/
	if(!exit_code) 
		aum_config_free_inner(conf);
	
	return exit_code;
}

/*
 * Returns 0 on error, if error is not NULL then will report the cause, 1 on success.
 */ 
static int
_init(AumConfig *conf, GError **err) 
{
	int exit_code = 1;
	GError *error = NULL;

	assert(conf != NULL);

	/*g_return_if_fail((err!=NULL && *err == NULL));*/
	
	if(!isInitialized) {
		g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_EINIT, "library is not initialized");
		g_propagate_error(err, error);
		exit_code = 0;
		goto cleanup;
	}
	
	if(conf->root == NULL)
		conf->root = strdup(ROOTDIR);

	if(conf->dbpath == NULL){
		conf->dbpath = (char*) calloc(strlen(conf->root) + strlen(DBPATH) ,sizeof(char));
		sprintf(conf->dbpath,"%s%s",conf->root, DBPATH);
	}
	
	if(conf->logfile == NULL){
		conf->logfile = (char*) calloc(strlen(conf->root) + strlen(LOGFILE) ,sizeof(char));
		sprintf(conf->logfile,"%s%s",conf->root, LOGFILE);
	}

	if(alpm_option_set_root(conf->root)) {
		g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_EROOT, \
					"unable to set the root directory, Cause: %s", alpm_strerrorlast());
		g_propagate_error(err, error);
		exit_code = 0;
		goto cleanup;
	}
	if(alpm_option_set_dbpath(conf->dbpath)) {
		g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_EDB, \
					"unable to set the database directory, Cause: %s", alpm_strerrorlast());
		g_propagate_error(err, error);
		exit_code = 0;
		goto cleanup;	
	} 
	if(alpm_option_set_logfile(conf->logfile)) {
		g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_ELOG, \
					"unable to set the log file, Cause: %s", alpm_strerrorlast());
		g_propagate_error(err, error);
		exit_code = 0;
		goto cleanup;
	}

	alpm_option_set_arch(conf->architecture);
	
	if(conf->caches == NULL) {
		/* Use the default cache dir (CACHEDIR) */
		if(alpm_option_add_cachedir(CACHEDIR)) {
			g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_ECACHE, \
						"impossible to add the cache directory, Cause %s",alpm_strerrorlast());
			g_propagate_error(err, error);
			exit_code = 0;
			goto cleanup;
		}
	}else{
		/* Using the ones defined in the structure */ 
		alpm_list_t *i;
		for(i = conf->caches; i != NULL; i = alpm_list_next(i)) {
			if(alpm_option_add_cachedir((char*) alpm_list_getdata(i))) {
				g_set_error(&error, AUM_CONFIG_ERROR, AUM_ERROR_ECACHE, \
							"unable to add the cache directory, Cause %s",alpm_strerrorlast());
				g_propagate_error(err, error);
				exit_code = 0;
				goto cleanup;
			}
		}
	}
	if(conf->ignorePkg != NULL) {
		alpm_list_t *i;
		for(i = conf->ignorePkg; i != NULL; i = alpm_list_next(i))
			alpm_option_add_ignorepkg((char*) alpm_list_getdata(i));
	}
	if(conf->ignoreGrp != NULL) {
		alpm_list_t *i;
		for(i = conf->ignoreGrp; i != NULL; i = alpm_list_next(i)) 
			alpm_option_add_ignoregrp((char*) alpm_list_getdata(i));
	}
	if(conf->noUpgrade != NULL) {
		alpm_list_t *i;
		for(i = conf->noUpgrade; i != NULL; i = alpm_list_next(i)) 
			alpm_option_add_noupgrade((char*) alpm_list_getdata(i));
	}
	if(conf->noExtract != NULL) {
		alpm_list_t *i;
		for(i = conf->noExtract; i != NULL; i = alpm_list_next(i))
			alpm_option_add_noextract((char*) alpm_list_getdata(i));
	}

	if(conf->repos != NULL){
		alpm_list_t *i, *j;
		pmdb_t* db; 
		Repository *data;
		
		for(i = conf->repos; i != NULL; i = alpm_list_next(i)) {
			data = (Repository*)alpm_list_getdata(i);
			db = alpm_db_register_sync(data->name);
			for(j = data->servers; j != NULL; j = alpm_list_next(j))
				alpm_db_setserver(db, (char*) alpm_list_getdata(j));
		}
	}
	
cleanup:	
	return exit_code;
}

/********************** Error Handling functions **********************/

GQuark 
aum_config_error(void)
{
	return g_quark_from_static_string("aum-config-error");
}