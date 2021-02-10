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
 * File is a General Cire File. Anyone may commit changes.
 *
 * $Id: main.h,v 1.22 2004/11/24 02:54:44 andrewm1 Exp $
 *  ***************************************
*/

#ifndef __MAIN_H
#define __MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#ifdef BUILD_NCURSES_UI
	#include <curses.h>
#endif

#ifdef BUILD_GTK_UI
	#include <gtk/gtk.h>
#endif

#include "misc.h"
#include "entry.h"
#include "diary.h"
#include "prefs.h"
#include "ncurses_ui.h"
#include "text_ui.h"
#include "gtk_ui_main.h"
#include "gtk_ui_callbacks.h"
#include "gtk_ui_other.h"

#define CIRE_VERSION "0.14.0"

/* this is an 'about' message, it's shared by the ncurses and gtk interfaces */
#define CIRE_ABOUT_MESSAGE cire_about_message
const char* cire_about_message;

char* errmsg;
/* When an error is encountered in the core code, it puts an error message
 * here for the current user interface to display to the user however it wants.
 */

#endif
