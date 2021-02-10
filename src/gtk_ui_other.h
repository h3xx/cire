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
 * File is Managed by John Ilves (thestarz). Get Permission Before Commiting.
 *
 * $Id: gtk_ui_other.h,v 1.18 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifndef __GTK_UI_OTHER_H
#define __GTK_UI_OTHER_H

#ifdef BUILD_GTK_UI

int
ask_question_via_dialog(struct gtk_ui_data* gtk_ui_data, char* question);
/* Uses a GtkMessageDialog to ask the user *question and
 * returns non-zero if they say yes and zero if not.
*/

void
report_error_via_dialog(struct gtk_ui_data* gtk_ui_data, char* error);
/* Opens an error dialog box and displays *error in it.
*/

const char*
get_text_via_dialog(struct gtk_ui_data* data, const char* title,
	const char* message, int password_mode);
/* This opens a dialog box with *title in the title bar.
 * It will contain *message in a label and will have an entry
 * widget for the user to enter text. If the user hits ok
 * the entered text is returned as a statically allocated char*
 * otherwise NULL is returned. If password_mode is true (non-0)
 * asterisks (*) will be shown instead of the keys the user types.
*/

int
sync_diary_and_entry_list(struct gtk_ui_data* gtk_ui_data);
/* This clears the list store used to display the entries and
 * then refills it with the contents of gtk_ui_data->diary.
 * It calls gtk_ui_sort_diary() first so that any newly added
 * or changed entries will be properly sorted.
*/

int
gtk_ui_sort_diary(struct gtk_ui_data* data);
/* This sorts the diary according to the columns of the entry
 * list tree view. Returns zero on success or non-zero if neither
 * column has it's sort indicator set.
*/

char*
get_filename_via_dialog(char* default_filename);
/* This function uses a GtkFileSelection dialog to get a filename
 * from the user. It returns a pointer to a statically allocated
 * string containing the filename or a NULL if the user hits cancel.
 * If *default_filename is non-null it will be used as the default
 * filename.
*/

int
set_title_bar_text(struct gtk_ui_data* data, const char* text);
/* This sets the title bar of the main window to be "Cire - <text>".
 * Returns zero if all goes well.
*/

int
set_curfile(struct gtk_ui_data* data, const char* new_file);
/* This copies *new_file into *curfile. If new_file is null this
 * simply unsets curfile. This returns zero on succes.
*/

int
set_curfile_password(struct gtk_ui_data* data, const char* new_password);
/* This copies *new_password into *curfile_password. if new_password
 * is null this simply unsets curfile_password. 0 == success.
*/

int
get_selected_entry_offset(struct gtk_ui_data* data);
/* Returns the offset of the currently selected
 * entry or -1 if none is selected.
*/


int
select_entry_by_offset(struct gtk_ui_data* data, int offset);
/* Selects an entry, given an offset - a nasty hack!
 */

int
prefs_window_load(struct gtk_ui_data* data);
/* Loads the stuff in the prefs module into the GtkEntryS of
 * the prefs window. Returns 0 on succes, non-0 on error.
*/

int
prefs_window_save(struct gtk_ui_data* data);
/* Saves the stuff in the GtkEntryS in the prefs window
 * back into the prefs module. Success ~ 0 Error ~ non-0
*/


#endif /* BUILD_GTK_UI */
#endif /* __GTK_UI_OTHER_H */
