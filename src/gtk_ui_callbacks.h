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
 * $Id: gtk_ui_callbacks.h,v 1.22 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifndef __GTK_UI_CALLBACKS_H
#define __GTK_UI_CALLBACKS_H

#ifdef BUILD_GTK_UI

gboolean
window_main_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data);
/* Callback for the "delete event" signal. It uses config_quit() to
 * ask the user if they're sure they want to quit and shuts down the
 * program if they say yes.
*/

void
window_main_destroy(GtkObject* object, gpointer gtk_ui_data);
/* This handles the "destroy" signal and calls gtk_main_quit().
*/

void
menu_item_diary_new_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item );
/* Creates a new diary. Basically it just clears what's there already.
*/

void
menu_item_diary_open_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for diary->open. It asks the user to select a file and then
 * opens it. It asks for confirmation if there's already a file loaded.
*/

void
menu_item_diary_save_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for diary->save. It saves to the place it was last saved or
 * where it was opened from. If the diary hasn't been saved before it
 * calls the callback for save as.
*/

void
menu_item_diary_save_as_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for diary->save as. Asks the user where to save the diary.
 * Copies the new filename to curfile. Adds the extension .cire if nessecary
 * and updates the window title. If .cire is not part of the name the user
 * gives us we only add .cire if the name without it doesn't exist.
*/

void
menu_item_diary_quit_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for diary->quit. It uses confirm_quit() to ask the user
 * if they really want to quit. If they say yes it calls gtk_main_quit().
*/

void
menu_item_entry_add_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for entry->add. It adds an entry to the diary with
 * a title of 'Untitled' and contents of "".
*/

void
menu_item_entry_remove_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for entry->remove. It removes the currently selected entry
 * after asking the user for confirmation.
*/

void
menu_item_entry_rename_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for entry->rename. It gets a new name from the user via
 * a dialog box (get_text_via_dialog()) and sets the title of the
 * currently selected title to it.
*/

void
menu_item_tools_find_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for tools->search. Opens a window used to search for an entry
 */


void
menu_item_tools_export_html_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for tools->export->html. It gets a filename from the user
 * and tries to export the current diary to it. Opens an error dialog
 * if it fails.
*/

void
menu_item_tools_export_text_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Same as export_html_activate() above. Both this and the html export
 * only add the appropriate extension if the filename given to us by
 * the user (without the extension) does not exist.
*/

void
menu_item_tools_preferences_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for tools->preferences. Loads the current prefs into the window
 * and then shows it.
*/

void
menu_item_help_about_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item);
/* Callback for help->about. It opens a dialog box with GTK_UI_ABOUT_MESSAGE
 * as the message. GTK_UI_ABOUT_MESSAGE is #define'd in gtk_ui_main.h
*/

void
entry_list_title_column_clicked(GtkTreeViewColumn* column, gpointer gtk_ui_data);
/* Callback for the title column of the entry list tree view. If the title
 * column wasn't already set as for sorting it sets it and if it was then
 * it changes the sort direction. It then calles sync_diary_and_entry_list()
 * to sort and rebuild the entry list.
*/

void
entry_list_modified_column_clicked(GtkTreeViewColumn* column, gpointer gtk_ui_data);
/* Just like entry_list_title_column_clicked() but for the time-modified column.
*/

gboolean
entry_list_button_press_event(GtkWidget* widget, GdkEventButton* event, gpointer gtk_ui_data);
/* Callback when the tree view is clicked. If it's a right click the 'entry'
 * menu is displayed.
*/

void
tree_selection_entry_list_changed(GtkTreeSelection* slctn, gpointer gtk_ui_data);
/* Callback for when the tree view selection changes. It puts the contents
 * of the newly selected entry into the text view on the right.
*/

void
button_save_entry_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for 'save entry'. It saves the text in the entry editor text view
 * to the entry that it came from.
*/

/*************** Prefs window ************/
gboolean
window_prefs_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data);
/* Callback to handle the delete-event of the prefs window. It acts just
 * like clicking cancel. It hides the window, without saving changes.
*/

void
prefs_window_ok_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for the ok button in the prefs window. It saves the changes and
 * then hides the prefs window.
*/

void
prefs_window_cancel_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for the cancel button in the prefs window. It hides the window
 * without saving the changes.
*/

void
prefs_window_apply_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for the apply button in the prefs window. It saves the changes.
*/


/*************** Find Entry window ************/

gboolean 
window_find_entry_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data);
/* Handles deleting of the find entry window.  Works the same as the other delete event
 * callbacks 
 */

void 
find_entry_cancel_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback to close the find entry window when the cancel button is clicked
 */

void
find_entry_find_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback runs the search and interprets the results (if any)
 */


/*************** HTML export window ************/
gboolean
window_html_export_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data);
/* Identical callback to window_prefs_delete_event(), but for the HTML export
 * window instead
 */

void
html_export_ok_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for the ok button in the html_export window.  Exports the HTML file
 * and hides the window.
 */

void
html_export_cancel_clicked(GtkButton* button, gpointer gtk_ui_data);
/* Callback for the cancel button in the html_export window. Hides the window.
 */

void
html_export_use_header_html_toggled(GtkWidget* button, gpointer gtk_ui_data);

void
html_export_use_footer_html_toggled(GtkWidget* button, gpointer gtk_ui_data);

void
html_export_get_output_clicked(GtkButton* button, gpointer gtk_ui_data);

void
html_export_get_header_clicked(GtkButton* button, gpointer gtk_ui_data);

void
html_export_get_footer_clicked(GtkButton* button, gpointer gtk_ui_data);




#endif /* BUILD_GTK_UI */
#endif /* __GTK_UI_CALLBACKS_H */
