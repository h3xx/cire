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
 * $Id: gtk_ui_callbacks.c,v 1.31 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifdef BUILD_GTK_UI

#include "main.h"


gboolean
window_main_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* give the user a chance to save first */
	if( !diary_is_empty(data->diary) || data->curfile )
	{
		if( ask_question_via_dialog(data, "Save current diary first?") )
		{
		    menu_item_diary_save_activate(gtk_ui_data, 0,
						  gtk_item_factory_get_widget(data->item_factory, "/File/Save"));
		}
	}

	return FALSE;
}

void
window_main_destroy(GtkObject* object, gpointer gtk_ui_data)
{
	gtk_main_quit();
}

void
menu_item_diary_new_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* give the user a chance to save first */
	if( !diary_is_empty(data->diary) )
	{
		if( ask_question_via_dialog(data, "Save current diary first?") )
		{
		    menu_item_diary_save_activate(gtk_ui_data, 0,
						  gtk_item_factory_get_widget(data->item_factory, "/File/Save"));
		}
	}

	/* get rid of what we have open now */
	set_curfile(data, NULL);
	diary_clear(data->diary);

	/* update the title bar */
	set_title_bar_text(data, "Untitled");

	/* update the list of entries */
	sync_diary_and_entry_list(data);
}

void
menu_item_diary_open_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	char* new_file;
	const char* new_password;
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* get the file to open */
	new_file = get_filename_via_dialog(prefs_get_cire_dir());

	if(!new_file)
	    return;
	
	if( misc_file_is_encrypted(new_file) )
	{
		new_password = get_text_via_dialog(data, "Enter password:",
			"The file you selected seems to be\nencrypted, please enter the password:",
			TRUE);
		
		if(!new_password)
			return;
	}
	else
	{
		new_password = NULL;
	}

	/* call new to get rid of what's there already */
	menu_item_diary_new_activate(gtk_ui_data, 0, 
				     gtk_item_factory_get_widget(data->item_factory, "/File/New"));

	if( diary_load_from_file(data->diary, new_file, new_password) )
	{
		report_error_via_dialog(data, "Failed to open file.");
		return;
	}

	/* record what file we opened */
	set_curfile(data, new_file);
	
	/* record the password used */
	set_curfile_password(data, new_password);

	/* update the entry list */
	sync_diary_and_entry_list(data);

	/* update the title bar */
	set_title_bar_text(data, new_file);
}

void
menu_item_diary_save_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* if the diary hasn't been saved before delegate to save_as */
	if(!data->curfile)
	{
	    menu_item_diary_save_as_activate(gtk_ui_data, 0,
					     gtk_item_factory_get_widget(data->item_factory, "/File/Save As"));
	}
	else /* otherwise save where it was last saved / where it was opened from */
	{
		if( diary_save_to_file(data->diary, data->curfile, data->curfile_password) )
		    report_error_via_dialog(data, "Failed to save file.");
	}
}

void
menu_item_diary_save_as_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	const char* dest_file;
	const char* dest_file_with_ext;
	char* password;
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	GtkWidget* dialog;
	GtkWidget* hbox;
	GtkWidget* check_button_password;
	GtkWidget* entry_password;

	dialog = gtk_file_selection_new("Select a Diary file");

	gtk_file_selection_set_filename(GTK_FILE_SELECTION(dialog),
		prefs_get_cire_dir());
	
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	
	check_button_password = gtk_check_button_new_with_label("Encrypt file using password:");
	gtk_box_pack_start(GTK_BOX(hbox), check_button_password, FALSE, FALSE, 0);
	gtk_widget_show(check_button_password);

	entry_password = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), entry_password, TRUE, TRUE, 0);
	gtk_widget_show(entry_password);
	
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		dest_file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button_password)))
		{
			password = malloc(strlen(gtk_entry_get_text(GTK_ENTRY(entry_password))) + 1);
			strcpy( password, gtk_entry_get_text(GTK_ENTRY(entry_password)) );
		}
		else
		{
			password = NULL;
		}
	}
	else
	{
		dest_file = NULL;
		password = NULL;
	}

	gtk_widget_destroy(dialog);

	if(!dest_file)
	    return;

	if(misc_file_exists(dest_file))
		dest_file_with_ext = dest_file;
	else
		dest_file_with_ext = misc_force_file_extension(dest_file, ".cire");
	
	/* if it already exits ask before overwriting it */
	if(misc_file_exists(dest_file_with_ext))
	{
		if(!ask_question_via_dialog(data, "File exists, overwrite it?"))
		{
			free(password);
			return;
		}
	}

	/* try to save it */
	if( diary_save_to_file(data->diary, dest_file_with_ext, password) )
	{
		report_error_via_dialog(data, "Failed to save file.");
	}
	else
	{
		set_curfile(data, dest_file_with_ext);
		set_curfile_password(data, password);
	}

	/* update the window title bar */
	set_title_bar_text(data, data->curfile);

	free(password);
}

void
menu_item_diary_quit_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* give the user a chance to save first */
	if( !diary_is_empty(data->diary) || data->curfile )
	{
		if( ask_question_via_dialog(data, "Save current diary first?") )
		{
		    menu_item_diary_save_activate(gtk_ui_data, 0,
						  gtk_item_factory_get_widget(data->item_factory, "/File/Save"));
		}
	}

	gtk_main_quit();
}

void
menu_item_entry_add_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct entry* new_entry;
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* create an entry and add it to the diary */
	new_entry = entry_new();
	entry_set_title(new_entry, "Untitled");
	entry_set_contents(new_entry, "");
	diary_add_entry(data->diary, new_entry);

	/* update the treeview of entries */
	sync_diary_and_entry_list(data);
}

void
menu_item_entry_remove_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	int offset;

	/* get the offset of the entry currently selected */
	if( (offset = data->entry_editor_current_offset) == -1 )
		return;

	if( ask_question_via_dialog(data, "Are you sure you want to remove this entry?") )
		diary_remove_entry_by_offset(data->diary, offset);

	data->entry_editor_current_offset = -1;

	/* rebuild the entry list */
	sync_diary_and_entry_list(data);
}

void
menu_item_entry_rename_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	const char* new_name;
	int offset;

	/* get the offset */
	if( (offset = data->entry_editor_current_offset) == -1 )
		return;

	new_name = get_text_via_dialog(data, "Rename Entry", "Enter new name:", FALSE);

	if(!new_name)
		return;

	entry_set_title(diary_get_entry_by_offset(data->diary, offset), (char*) new_name);

	/* refresh the list of enries */
	sync_diary_and_entry_list(data);
}

void
menu_item_tools_find_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item) {

	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_show(data->window_find_entry);

}

void
menu_item_tools_export_html_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->chk_use_header_html), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->chk_use_footer_html), FALSE);
	gtk_widget_set_sensitive(data->entry_header_html, FALSE);
	gtk_widget_set_sensitive(data->entry_footer_html, FALSE);
	gtk_widget_set_sensitive(data->btn_get_header_html, FALSE);
	gtk_widget_set_sensitive(data->btn_get_footer_html, FALSE);
	
	gtk_widget_show(data->window_html_export);
}

void
menu_item_tools_export_text_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	const char* file;
	const char* file_with_ext;
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	if( (file = get_filename_via_dialog(prefs_get_cire_dir())) == NULL )
		return;
	
	if(misc_file_exists(file))
		file_with_ext = file;
	else
		file_with_ext = misc_force_file_extension(file, ".txt");

	if( misc_file_exists(file_with_ext) )
	{
		if(!ask_question_via_dialog(data, "File exists, overwrite it?"))
			return;
	}

	if( diary_export_text(data->diary, file_with_ext) )
	{
		report_error_via_dialog(data, "Failed to export diary.");
	}
}

void
menu_item_tools_preferences_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	prefs_window_load(data);

	gtk_widget_show(data->window_prefs);
}

void
menu_item_help_about_activate(gpointer gtk_ui_data, guint callback_data, GtkWidget* menu_item)
{
	GtkWidget* message_dialog;

	message_dialog = gtk_message_dialog_new(
	    GTK_WINDOW( ((struct gtk_ui_data*)gtk_ui_data)->window_main ),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_CLOSE,
		CIRE_ABOUT_MESSAGE
		);

	gtk_dialog_run(GTK_DIALOG(message_dialog));
	gtk_widget_destroy(message_dialog);
}

void
entry_list_title_column_clicked(GtkTreeViewColumn* column, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* if we were already sorting based on title flip the direction */
	if( gtk_tree_view_column_get_sort_indicator(data->entry_list_title_column) )
	{
		if( gtk_tree_view_column_get_sort_order(data->entry_list_title_column)
			== GTK_SORT_ASCENDING )
		{
			gtk_tree_view_column_set_sort_order(data->entry_list_title_column,
				GTK_SORT_DESCENDING);
		}
		else
		{
			gtk_tree_view_column_set_sort_order(data->entry_list_title_column,
				GTK_SORT_ASCENDING);
		}
	}
	else /* otherwise change the sort basis */
	{
		gtk_tree_view_column_set_sort_indicator(data->entry_list_modified_column, FALSE);
		gtk_tree_view_column_set_sort_indicator(data->entry_list_title_column, TRUE);
	}

	sync_diary_and_entry_list(data);
}

void
entry_list_modified_column_clicked(GtkTreeViewColumn* column, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	/* if we were already sorting based on modified flip the direction */
	if( gtk_tree_view_column_get_sort_indicator(data->entry_list_modified_column) )
	{
		if( gtk_tree_view_column_get_sort_order(data->entry_list_modified_column)
			== GTK_SORT_ASCENDING )
		{
			gtk_tree_view_column_set_sort_order(data->entry_list_modified_column,
				GTK_SORT_DESCENDING);
		}
		else
		{
			gtk_tree_view_column_set_sort_order(data->entry_list_modified_column,
				GTK_SORT_ASCENDING);
		}
	}
	else /* otherwise change the sort basis */
	{
		gtk_tree_view_column_set_sort_indicator(data->entry_list_title_column, FALSE);
		gtk_tree_view_column_set_sort_indicator(data->entry_list_modified_column, TRUE);
	}

	sync_diary_and_entry_list(data);
}

gboolean
entry_list_button_press_event(GtkWidget* widget, GdkEventButton* event, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	
	tree_selection_entry_list_changed(NULL, gtk_ui_data);
	
	if(event->button == 3)
	{
	    gtk_menu_popup(GTK_MENU(gtk_item_factory_get_widget(data->item_factory, "/Entry")),
			   NULL, NULL, NULL, NULL,
			   event->button, event->time);
	}
	
	return FALSE;
}

void
tree_selection_entry_list_changed(GtkTreeSelection* slctn, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	GtkTextBuffer* editor_buffer;
	const char* entry_contents;
	const char* entry_title;
	char* entry_markup_title;
	int offset;

	/* get the selected entry's offset */
	offset = get_selected_entry_offset(data);

	/* get the editor buffer */
	editor_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text_view_entry_editor));

	/* check if we need to save the current text first */
	if( gtk_text_buffer_get_modified(editor_buffer) )
	{
		if( ask_question_via_dialog(data, "Save current entry first?") )
			button_save_entry_clicked(GTK_BUTTON(data->button_save_entry), gtk_ui_data);
	}

	/* get the entry contents and title */
	if( offset != -1 )
	{
	    entry_contents = entry_get_contents(diary_get_entry_by_offset(data->diary, offset));
	    entry_title = entry_get_title(diary_get_entry_by_offset(data->diary, offset));
	}
	else
	{
	    entry_contents = "";
	    entry_title = "No Entry Selected";
	}

	/* set the label */
	entry_markup_title = malloc(strlen(entry_title) + strlen("<b><big></big></b>") + 1);
	strcpy(entry_markup_title, "<b><big>");
	strcat(entry_markup_title, misc_str_trunc(entry_title, 40));
	strcat(entry_markup_title, "</big></b>");

	/* put the title into the label */
	gtk_label_set_markup(GTK_LABEL(data->entry_editor_label), entry_markup_title);
	
	/* put the contents of the selected entry into the buffer */
	gtk_text_buffer_set_text(editor_buffer, entry_contents, strlen(entry_contents));

	/* record the offset */
	data->entry_editor_current_offset = offset;

	/* set the entry editor to modified=false */
	gtk_text_buffer_set_modified(editor_buffer, FALSE);
}

void
button_save_entry_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	GtkTextBuffer* editor_buffer;
	char* editor_contents;
	int offset;
	GtkTextIter start;
	GtkTextIter end;

	/* get the offset of the entry they were working with */
	if( (offset = data->entry_editor_current_offset) == -1 )
	{
		/* TODO tell the user he can't save */
		return;
	}

	/* get the editor buffer */
	editor_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text_view_entry_editor));

	/* get the start and end iters */
	gtk_text_buffer_get_start_iter(editor_buffer, &start);
	gtk_text_buffer_get_end_iter(editor_buffer, &end);

	/* get the current contents of the editor text view */
	editor_contents = gtk_text_buffer_get_text(editor_buffer, &start, &end, FALSE);

	/* put the editor contents into the entry in the diary */
	entry_set_contents(diary_get_entry_by_offset(data->diary, offset), editor_contents);

	/* now that it's saved, set the editor to modified=false */
	gtk_text_buffer_set_modified(editor_buffer, FALSE);
}

/****************** Prefs window ******************/
gboolean
window_prefs_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_prefs);

	return TRUE;
}


void
prefs_window_ok_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	prefs_window_save(data);

	gtk_widget_hide(data->window_prefs);
}

void
prefs_window_cancel_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_prefs);
}

void
prefs_window_apply_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	prefs_window_save(data);
}

/****************** Find entry window ******************/
gboolean 
window_find_entry_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_find_entry);

	return TRUE;
}

void 
find_entry_cancel_clicked(GtkButton* button, gpointer gtk_ui_data)
{

	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_find_entry);

}


void
find_entry_find_clicked(GtkButton* button, gpointer gtk_ui_data)
{

	enum search_type type;
	const char* needle;
	int offset;

	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_find_entry);

	/* Assign the search text.  If nothing has been entered, don't bother searching */
	if( (needle = gtk_entry_get_text(GTK_ENTRY(data->entry_search_for))) == NULL )
		return;

	/* Assign the correct search type */
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->btn_search_by_title)))
		type = search_by_title;
	else
		type = search_by_contents;

	/* Use the currently selected entry as the starting offset */
	if((offset = get_selected_entry_offset(data)) == -1 )
		return;


	/* Right, now we can perform the search */
	if((offset = diary_search(data->diary, type, needle, offset)) == -1)
		return;

	printf("%d\n", offset);

	/* Update the current offset with the returned value */
	data->entry_editor_current_offset = offset;
	(void) select_entry_by_offset(data, offset);



}	



/****************** Html export window ******************/
gboolean
window_html_export_delete_event(GtkWidget* wgt, GdkEvent* evnt, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_widget_hide(data->window_html_export);

	return TRUE;
}

void
html_export_get_output_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_entry_set_text(GTK_ENTRY(data->entry_output_html), get_filename_via_dialog(prefs_get_cire_dir()));
}

void
html_export_get_header_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_entry_set_text(GTK_ENTRY(data->entry_header_html), get_filename_via_dialog(prefs_get_cire_dir()));
}

void
html_export_get_footer_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

	gtk_entry_set_text(GTK_ENTRY(data->entry_footer_html), get_filename_via_dialog(prefs_get_cire_dir()));
}

void
html_export_ok_clicked(GtkButton* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	const char* file;
	const char* header;
	const char* footer;
	const char* file_with_ext;

	if( (file = gtk_entry_get_text(GTK_ENTRY(data->entry_output_html))) == NULL )
		return;

	if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->chk_use_header_html)) )
		header = gtk_entry_get_text(GTK_ENTRY(data->entry_header_html));
	else
		header = NULL;
	
	if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->chk_use_footer_html)) )
		footer = gtk_entry_get_text(GTK_ENTRY(data->entry_footer_html));
	else
		footer = NULL;

	if( misc_file_exists(file) )
		file_with_ext = file;
	else
		file_with_ext = misc_force_file_extension(file, ".html");

	if(header)
	{
		if( !misc_file_exists(header) )
		{
			report_error_via_dialog(data, "Failed to load header file.");
			return;
		}
	}

	if(footer)
	{
		if( !misc_file_exists(footer) )
		{
			report_error_via_dialog(data, "Failed to load footer file.");
			return;
		}
	}

	/* Check to see if the output file is to be overwritten */
	if( misc_file_exists(file_with_ext) )
	{
		if(!ask_question_via_dialog(data, "File exists, overwrite it?"))
			return;
	}

	if( diary_export_html_advanced(data->diary, file_with_ext, header, footer) )
		report_error_via_dialog(data, "Failed to export diary.");

	gtk_widget_hide(data->window_html_export);
}

void
html_export_cancel_clicked(GtkButton* button, gpointer gtk_ui_data)
{
   struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;

   gtk_widget_hide(data->window_html_export);
}

void
html_export_use_header_html_toggled(GtkWidget* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	gboolean state;
	
	state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->chk_use_header_html));
	gtk_widget_set_sensitive(data->entry_header_html, state);
	gtk_widget_set_sensitive(data->btn_get_header_html, state);
}

void
html_export_use_footer_html_toggled(GtkWidget* button, gpointer gtk_ui_data)
{
	struct gtk_ui_data* data = (struct gtk_ui_data*) gtk_ui_data;
	gboolean state;
	
	state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->chk_use_footer_html));
	gtk_widget_set_sensitive(data->entry_footer_html, state);
	gtk_widget_set_sensitive(data->btn_get_footer_html, state);
}



#endif
