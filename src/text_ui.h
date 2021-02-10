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
 * $Id: text_ui.h,v 1.8 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#ifndef __TEXT_UI
#define __TEXT_UI

#ifdef BUILD_TEXT_UI

struct text_ui_cire
{
    struct diary * the_diary;
    struct entry * the_entry;
    int offset;

    char * current_file;

    int saved;
};

int
text_ui_main(int argc, char * argv[]);
/* main interface loop
 */

/* Diary Menu */

int
text_ui_diary_new();
/* start a new diary with d
 * is not saved until save is called
 */

int
text_ui_diary_open();
/* close the current diary and open the one in 'filename'
 */

int
text_ui_diary_save();
/* call diary_save_to_file on text_core.the_diary
 */

int
text_ui_diary_save_as();
/* save diary to filename instead of current_file
 */

int
text_ui_diary_close();
/* set text_core.the_diary to null, save first
 */

int
text_ui_diary_quit();
/* prompt to save, then exit
 */

int
text_ui_diary_about();
/* Show the About Screen. Implemented by AndrewM.
 */


/* Entry Menu */

int
text_ui_entry_view_all();
/* list the entries in the diary for the user to choose one to edit
 */

int
text_ui_entry_view();
/* list the selected entry, unless none is selected
 */

int
text_ui_entry_select();
/* get an int and set offset to it
 */

int
text_ui_entry_add();
/* fork to the editor, add the entry to the diary
 */

int
text_ui_entry_delete();
/* read a line for the title of a new entry
 */

int
text_ui_entry_rename();
/* call entry_set_name(e, new_name)
 */

#endif /* BUILD_TEXT_UI */

#endif /* __TEXT_UI_H */
