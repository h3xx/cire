/*
 * ***************************************
 * Cire: An Open-Source Diary Program in C
 *
 * Copyright (C) 2004, Andrew Morritt, <andymorritt@rogers.com>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://cire.sourceforge.net/license.php
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * File is Managed by Ian Menard (deadalien). Get Permission Before Commiting.
 *
 * $Id: prefs.c,v 1.19 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#include "main.h"

/* store command line preferences */
static int pref_argc;
static char ** pref_argv;

/* integer array to keep track of whether or not a preference should be */
/* written to file */
static int should_write[NPREFS];  

/* supported preferences, defauts set by pref_load_defaults */
/* see there for more details */
static char * editor;
static char * cire_dir;
static char * timef;

static char * prefs_file;

int
prefs_init(int argc, char ** argv)
{
    int i;

    /* store argv */
    pref_argc = argc;
    pref_argv = (char **) malloc( pref_argc * sizeof(char *));

    for(i = 0; i < pref_argc; ++i)
    {
        pref_argv[i] = (char *)  malloc( strlen(argv[i]) +1 );
        strcpy(pref_argv[i], argv[i]);
    }

    prefs_file = malloc( strlen(getenv("HOME")) + strlen("/.cirerc") + 5);

    /* set the prefs file to ~/.cirerc */
    if(prefs_file != NULL)
    {
	strcpy(prefs_file, getenv("HOME"));
	strcat(prefs_file, "/.cirerc");
    }

    /* nothing should be written by default */
    for(i = 0; i < NPREFS; ++i)
	should_write[i] = 0;

    /* load prefs in proper order */
    prefs_load_defaults();
    prefs_load_env();
    prefs_load_from_file(prefs_file);

    return 0;
}

int
prefs_free()
{
    int i;
    
    if(editor != NULL)
        free(editor);
    if(cire_dir != NULL)
        free(cire_dir);
    if(timef != NULL)
        free(timef);

    if(prefs_file != NULL)
	free(prefs_file);

    if(pref_argv != NULL)
    {
	for(i = 0; i < pref_argc; ++i)
	{
	    if(pref_argv[i] != NULL)
		free(pref_argv[i]);
        }
        
        free(pref_argv);
    }

    return 0;
}


int
prefs_load_defaults()
{
    char * tmp;

    if(prefs_set_editor("/usr/bin/vi"))
        return 1;
    should_write[EDITOR] = 0;
    
    tmp = malloc(strlen(getenv("HOME")) + 2);
    strcpy(tmp, getenv("HOME"));
    strcat(tmp, "/");
    
    if(prefs_set_cire_dir(tmp))
    {
	free(tmp);
        return 1;
    }
    free(tmp);
    should_write[CIRE_DIR] = 0;
    
    if(prefs_set_timef("%Y/%m/%d"))
        return 1;
    should_write[TIMEF] = 0;
    
    return 0;
}

int
prefs_load_from_file(char * file)
{
    char * data;
    int file_len, i, j;
    char * tmp_s;
    
    if( misc_load_file(&data, &file_len, file) )
    	return 1;

    for(i = 0; i < file_len; ++i)
    {
    	for(j = 0; *(data + i + j) != '\n' && i + j < file_len; ++j)
	    ;

        tmp_s = malloc(j + 1);
        strncpy(tmp_s, data + i, j);
	tmp_s[j] = '\0';
	
        if(!strncmp("editor = ", tmp_s, strlen("editor = ")))
	{
            prefs_set_editor(tmp_s + strlen("editor = "));
	    should_write[EDITOR] = 1;
	}
        else if(!strncmp("cire_dir = ", tmp_s, strlen("cire_dir = ")))
	{
            prefs_set_cire_dir(tmp_s + strlen("cire_dir = "));
	    should_write[CIRE_DIR] = 1;
	}
        else if(!strncmp("timef = ", tmp_s, strlen("timef = ")))
	{
            prefs_set_timef(tmp_s + strlen("timef = "));
	    should_write[TIMEF] = 1;
	}

        i = i + j;
        free(tmp_s);
    }


    free(data);
    return 0;
}

int
prefs_load_env()
{
    if(getenv("EDITOR") != NULL)
    {
        prefs_set_editor(getenv("EDITOR"));
	should_write[EDITOR] = 0;
    }

    if(getenv("CIREDIR") != NULL)
    {
        prefs_set_cire_dir(getenv("CIREDIR"));
	should_write[CIRE_DIR] = 0;
    }

    return 0;
}

int
prefs_save()
{
    /* idea for saving file without losing comments, formatting, etc.
     *
     * scratched for now, print message describing that the file will
     * be overwritten instead.
     */
    
    FILE * fd;

    fd = fopen(prefs_file, "w+");

    if(fd == NULL)
    {
        fprintf(stderr, "Error opening '%s' for writing\n", prefs_file);
        return 1;
    }

    /* put a warning in the file about it being overwritten */
    fprintf(fd, "# Cire Preferences File\n\n");
    
    fprintf(fd, "# This file has been automatically generated by Cire.\n");
    fprintf(fd, "# You can certainly edit your preferences here.\n");
    fprintf(fd, "# You will lose any comments of your own, however.\n\n");

    fprintf(fd, "# Both the ncurses and gtk interfaces provide a method\n");
    fprintf(fd, "# for editing your preferences.  You are advised to use\n");
    fprintf(fd, "# those means instead of directly editing this file.\n\n");

    fprintf(fd, "# Preferences:\n\n");
    
    /* write the prefs */
    if(should_write[EDITOR])
    	fprintf(fd, "editor = %s\n", editor);
    	
    if(should_write[CIRE_DIR])
	fprintf(fd, "cire_dir = %s\n", cire_dir);
		
    if(should_write[TIMEF])
	fprintf(fd, "timef = %s\n", timef);


    fclose(fd);
    return 0;
}

int
prefs_from_arg(char * shortf, char * longf)
{
    int i;
	
    for(i = 1; i < pref_argc; ++i)
    {
	if( !strcmp(pref_argv[i], shortf) || !strcmp(pref_argv[i], longf) )
	    return 1;
    }
	
    return 0;
}

const char *
prefs_get_argv_file()
{
    int i;
    const char * filename = NULL;

    for(i = 1; i < pref_argc && filename == NULL; ++i)
    {
	if( strcmp(pref_argv[i], "-n") &&
	    strcmp(pref_argv[i], "--ncurses") &&
	    strcmp(pref_argv[i], "-g") &&
	    strcmp(pref_argv[i], "--gtk") &&
	    strcmp(pref_argv[i], "-t") &&
	    strcmp(pref_argv[i], "--text") )
	{
	    filename = pref_argv[i];
	}   
    }

    return filename;
}



/* set/get functions ----------------------------- */

/* editor */
char * prefs_get_editor()
{
    return editor;
}

int
prefs_set_editor(const char * ed)
{
    const char * str;

    str = misc_interpolate_filename(ed);
    if(str == NULL)
	str = ed;
    
    misc_str_assign(&editor, str);
    should_write[EDITOR] = 1;

    return 0;
}


/* cire_dir */
char *
prefs_get_cire_dir()
{
    return cire_dir;
}

int
prefs_set_cire_dir(const char * new_dir)
{
    const char * interp_new_dir;
    char * with_slash;
    
    interp_new_dir = misc_interpolate_filename(new_dir);
    if(interp_new_dir == NULL)
	interp_new_dir = new_dir;

    with_slash = NULL;

    if(interp_new_dir[strlen(interp_new_dir)-1] != '/')
    {
	with_slash = malloc(strlen(interp_new_dir) + 2);
	strcpy(with_slash, interp_new_dir);
	strcat(with_slash, "/");
    }
    else
	misc_str_assign(&with_slash, interp_new_dir);

    misc_str_assign(&cire_dir, with_slash);
    should_write[CIRE_DIR] = 1;

    return 0;
}


/* timef */
char *
prefs_get_timef()
{
    return timef;
}

int
prefs_set_timef(const char * new_timef)
{
    misc_str_assign(&timef, new_timef);
    should_write[TIMEF] = 1;

    return 0;
}


