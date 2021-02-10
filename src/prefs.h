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
 * $Id: prefs.h,v 1.12 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#ifndef __PREFS_H
#define __PREFS_H


/* ---------------------------------------------------------------------
 * NOTES:
 * pref_get functions should consider their return value READ ONLY
 *
 * ORDER OF RETURN
 * 1. returns setting from file if set
 * 2. failing that, returns possible environment variables
 * 3. failing that, returns a hardcoded default
 *
 * ARGV
 * right now, argv is not checked for any preference settings,
 * though it will be easy to do so if we decide to.
 *
 * ADDING A PREF: CHECKLIST
 * 1. create the static variable in prefs.c
 *    and add it to the enum
 * 2. add default value in prefs_load_defaults()
 * 3. add any necessary environtment checks in prefs_load_env()
 * 4. add necessary lines in prefs_load_from_file
 * 5. write get/set pair
 *
 * --------------------------------------------------------------------- */


/* keep an enum of preferences... */
/* TODO: find a work around, this might be duplication of information */

enum {
    EDITOR,
    CIRE_DIR,
    TIMEF, 

    NPREFS /* tells me how many preferences i have, add prefs above this */
};

int
prefs_init(int argc, char ** argv);
/* initialize preferences by:
 * 1. store argv (for use in prefs_get_*)
 * 2. load defaults
 * 3. load prefs from file, overwriting any defaults
 */

int
prefs_free();
/* free all preferences
 */

int
prefs_load_defaults();
/* load the default preferences
 */

int
prefs_load_from_file(char * file);
/* load from file, overwriting existing prefs or setting others
 */

int
prefs_load_env();
/* check any relevant environment variables
 */

int
prefs_save();
/* save prefs to file, without saving formatting
 */

int
prefs_from_arg(char * shortf, char * longf);
/* looks up both the short and long format to see if they were
 * specified on the command line
 */

const char *
prefs_get_argv_file();
/* checks for a filename specified on the command line (argv)
 * returns filename if there was one
 *  returns NULL if not
 */

/* set/get functions -------------------------------- */

char * prefs_get_editor();
int prefs_set_editor(const char * ed);
/* you can free 'ed' after calling set_editor
 */

char * prefs_get_cire_dir();
int prefs_set_cire_dir(const char * new_dir);
/* you can free 'new_dir' after calling set_ui
 */

char * prefs_get_timef();
int prefs_set_timef(const char * new_timef);
/* you can free 'timef' after calling set_timef
 */

/* --------- */


#endif
