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
 * File is Managed by Joe Abbey (joeabbey). Get Permission Before Commiting.
 *
 * $Id: ncurses_ui.h,v 1.35 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#ifndef __NCURSES_UI_H
#define __NCURSES_UI_H

#ifdef BUILD_NCURSES_UI

#define NCI_KEY_ENTER      10
#define NCI_KEY_BACKSPACE 127
#define NCI_KEY_ESCAPE     27
#define NCI_KEY_TAB         9
#define NCI_KEY_SHIFT_TAB  90
#define NCI_KEY_HOME      362
#define NCI_KEY_END       385

struct nci_options
{
	const char* name;
	char* buffer;
	int buffer_size;
};

struct nci
{
	struct diary* diary;
	char* filename;
	char* password;
	
	int modified;
	int selected_entry_offset;
	
	enum sort_type sort_type;
	enum sort_direction sort_direction;
	int sorting_set;
	
	char* last_search_string;
	int last_search_offset;
	enum search_type last_search_type;
};

int
ncurses_ui_main(int argc, char** argv);

int
nci_load_argv_file(struct nci* nci);

int
nci_diary_menu(struct nci* nci);

int
nci_entry_menu(struct nci* nci);

int
nci_tools_menu(struct nci* nci);

int
nci_tools_export_menu(struct nci* nci);

int
nci_sort_menu(struct nci* nci);

int
nci_help_menu(struct nci* nci);

int
nci_select_next(struct nci* nci);

int
nci_select_prev(struct nci* nci);

int
nci_quit(struct nci* nci);

int
nci_diary_new(struct nci* nci);

int
nci_diary_open(struct nci* nci);

int
nci_diary_save(struct nci* nci);

int
nci_diary_save_as(struct nci* nci);

int
nci_entry_add(struct nci* nci);

int
nci_entry_edit(struct nci* nci);

int
nci_entry_view(struct nci* nci);
/* Used to view the contents of the currently selected entry using
 * nci_text_viewer().
 */

int
nci_text_viewer(struct nci* nci, const char* text);
/* Less-like text viewer. Displays *text on the screen and allows the user to
 * scroll it up and down.
 */

int
nci_entry_delete(struct nci* nci);

int
nci_entry_rename(struct nci* nci);

int
nci_tools_preferences(struct nci* nci);

int
nci_tools_export_text(struct nci* nci);

int
nci_tools_export_html(struct nci* nci);

int
nci_tools_export_latex(struct nci* nci);

int
nci_tools_search_menu(struct nci* nci);

int
nci_tools_search(struct nci* nci, enum search_type type);

int
nci_tools_search_next(struct nci* nci);

int
nci_help_about(struct nci* nci);
/* Displays CIRE_ABOUT_MESSAGE using nci_text_viewer().
 */

int
nci_sort(struct nci* nci);

int
nci_update_screen(struct nci* nci);

int
nci_update_header(struct nci* nci);

int
nci_print_footer(const char* footer);

const char*
nci_get_user_text(struct nci* nci, const char* prompt);

int
nci_tools_prefs(struct nci* nci);

int
nci_global_key_handler(struct nci* nci, int key);
/* Checks if key is a global key. If it is it takes
 * the appropriate action for it and returns TRUE,
 * if not it returns FALSE.
 */
 
int
nci_press_any_key(struct nci* nci, const char* message);
/* Prints *message followed by "Press any key to continue"
 * to the footer area and then waits for a key press before
 * returning.
 */

#endif /* BUILD_NCURSES_UI */
#endif /* __NCURSES_UI_H */
