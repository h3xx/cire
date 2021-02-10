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
 * $Id: gtk_ui_other.c,v 1.19 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

 
#ifdef BUILD_GTK_UI

#include "main.h"

int
ask_question_via_dialog(struct gtk_ui_data* gtk_ui_data, char* question)
{
	gint response;
	GtkWidget* message_dialog;

	message_dialog = gtk_message_dialog_new(
		GTK_WINDOW(gtk_ui_data->window_main),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		question
		);

	response = gtk_dialog_run(GTK_DIALOG(message_dialog));
	gtk_widget_destroy(message_dialog);

	if(response == GTK_RESPONSE_YES)
		return 1;
	else
		return 0;
}

void
report_error_via_dialog(struct gtk_ui_data* gtk_ui_data, char* error)
{
	GtkWidget* message_dialog;

	message_dialog = gtk_message_dialog_new(
		GTK_WINDOW(gtk_ui_data->window_main),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		error
		);

	gtk_dialog_run(GTK_DIALOG(message_dialog));
	gtk_widget_destroy(message_dialog);
}

const char*
get_text_via_dialog(struct gtk_ui_data* data, const char* title,
	const char* message, int password_mode)
{
	GtkWidget* dialog;
	GtkWidget* label;
	GtkWidget* entry;
	int response;
	static char* user_entry = 0;
	
	/* get rid of last times response */
	free(user_entry);
	
	/* create the dialog */
	dialog = gtk_dialog_new_with_buttons(title,
		GTK_WINDOW(data->window_main), GTK_DIALOG_MODAL,
		GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	
	/* create and add a label containing *message */
	label = gtk_label_new(message);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show(label);
	
	/* create and add an entry for the user to type in */
	entry = gtk_entry_new();
	if(password_mode) gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), entry);
	gtk_widget_show(entry);
	
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	
	if(response == GTK_RESPONSE_ACCEPT)
	{
		user_entry = malloc( strlen(gtk_entry_get_text(GTK_ENTRY(entry))) + 1 );
		strcpy(user_entry, gtk_entry_get_text(GTK_ENTRY(entry)));
	}
	else
	{
		user_entry = NULL;
	}
	
	gtk_widget_destroy(entry);
	gtk_widget_destroy(label);
	gtk_widget_destroy(dialog);
	
	return user_entry;
}

int
sync_diary_and_entry_list(struct gtk_ui_data* gtk_ui_data)
{
	int i;
	GtkTreeIter iter;
	struct diary* diary;
	char timestamp[50];

	/* for convenience */
	diary = gtk_ui_data->diary;
	
	/* first make sure the diary is properly sorted */
	gtk_ui_sort_diary(gtk_ui_data);

	/* clear the list of entries */
	gtk_list_store_clear(gtk_ui_data->list_store_entry_list);

	/* fill the list of entries with the entries in the diary */
	for(i = 0; i < diary_get_entry_count(diary); ++i)
	{
		/* get timestamp */
		entry_get_timef(diary_get_entry_by_offset(diary, i),
		    timestamp, 50, prefs_get_timef());
		
		/* add a row to the list of entries */
		gtk_list_store_append(
		    GTK_LIST_STORE(gtk_ui_data->list_store_entry_list),
			&iter );
		gtk_list_store_set(
			GTK_LIST_STORE(gtk_ui_data->list_store_entry_list),
			&iter,
			0, misc_str_trunc(entry_get_title(diary_get_entry_by_offset(diary, i)), 20),
			1, timestamp,
			-1);
	}

	return 0;
}

int
gtk_ui_sort_diary(struct gtk_ui_data* data)
{
	if(gtk_tree_view_column_get_sort_indicator(data->entry_list_title_column))
	{
		if(gtk_tree_view_column_get_sort_order(data->entry_list_title_column)
			== GTK_SORT_ASCENDING)
		{
			diary_sort(data->diary, sort_title, sort_ascending);
		}
		else
		{
			diary_sort(data->diary, sort_title, sort_descending);
		}
	}
	else if(gtk_tree_view_column_get_sort_indicator(data->entry_list_modified_column))
	{
		if(gtk_tree_view_column_get_sort_order(data->entry_list_modified_column)
			== GTK_SORT_ASCENDING)
		{
			diary_sort(data->diary, sort_timestamp, sort_ascending);
		}
		else
		{
			diary_sort(data->diary, sort_timestamp, sort_descending);
		}
	}
	else
	{
		return 1;
	}
	
	return 0;
}

char*
get_filename_via_dialog(char* default_filename)
{
	char* tmp;
	static char* filename = 0;
	GtkWidget* file_selection_dialog;

	free(filename);

	file_selection_dialog = gtk_file_selection_new("Select a Diary file");

	if(default_filename)
	{
		gtk_file_selection_set_filename(
			GTK_FILE_SELECTION(file_selection_dialog),
			default_filename);
	}

	if( gtk_dialog_run(GTK_DIALOG(file_selection_dialog)) == GTK_RESPONSE_OK )
	{
		tmp = (char*) gtk_file_selection_get_filename(
		    GTK_FILE_SELECTION(file_selection_dialog));

		filename = malloc( strlen(tmp) + 1 );
		strcpy(filename, tmp);
	}
	else
	{
		filename = 0;
	}

	gtk_widget_destroy(file_selection_dialog);

	return filename;
}

int
set_title_bar_text(struct gtk_ui_data* data, const char* text)
{
	char* title;

	title = malloc( strlen("Cire - ") + strlen(text) + 1 );

	strcpy(title, "Cire - ");
	strcat(title, text);

	gtk_window_set_title(GTK_WINDOW(data->window_main), title);

	free(title);
	return 0;
}

int
set_curfile(struct gtk_ui_data* data, const char* new_file)
{
	free(data->curfile);

	if(new_file)
	{
		data->curfile = malloc( strlen(new_file) + 1 );
		strcpy(data->curfile, new_file);
	}
	else
	{
		data->curfile = 0;
	}

	return 0;
}

int
set_curfile_password(struct gtk_ui_data* data, const char* new_password)
{
	free(data->curfile_password);

	if(new_password)
	{
		data->curfile_password = malloc( strlen(new_password) + 1 );
		strcpy(data->curfile_password, new_password);
	}
	else
	{
		data->curfile_password = NULL;
	}

	return 0;
}

int
get_selected_entry_offset(struct gtk_ui_data* data)
{
	GtkTreeSelection* selection;
	GtkTreeModel* model;
	GtkTreeIter iter_selected;
	GtkTreeIter iter_counter;
	GtkTreePath* path_a;
	GtkTreePath* path_b;
	int offset;
	
	/* get the tree selection */
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->tree_view_entry_list));
	
	/* get the tree model and the selected iter */
	if( !gtk_tree_selection_get_selected(selection, &model, &iter_selected) )
	{
		return -1;
	}
	
	/* scroll thru all the iters and find one that matches the selected one */
	gtk_tree_model_get_iter_first(model, &iter_counter);
	for(offset = 0; ; ++offset)
	{
		path_a = gtk_tree_model_get_path(model, &iter_selected);
		path_b = gtk_tree_model_get_path(model, &iter_counter);
		
		if( gtk_tree_path_compare(path_a, path_b) == 0 )
		{
			gtk_tree_path_free(path_a);
			gtk_tree_path_free(path_b);
			break;
		}
		else
		{
			gtk_tree_path_free(path_a);
			gtk_tree_path_free(path_b);
			
			if( !gtk_tree_model_iter_next(model, &iter_counter) )
			{
				fprintf(stderr, "No matching iter found.\n");
				return -1;
			}
			
			continue;
		}
	}
	
	return offset;
}

int
select_entry_by_offset(struct gtk_ui_data* data, int offset)
{
	GtkTreeSelection* selection;
	GtkTreeModel* model;
	GtkTreeIter iter_selected;
	GtkTreeIter iter_counter;
	int i;

	/* get the tree selection */
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->tree_view_entry_list));
	
	/* get the tree model and the selected iter */
	if( !gtk_tree_selection_get_selected(selection, &model, &iter_selected) )
	{
		return -1;
	}

	/* start at the beginning */
	gtk_tree_model_get_iter_first(model, &iter_counter);

	for(i = 0; i < offset ; i++) {

		if(!gtk_tree_model_iter_next(model, &iter_counter) )
			{
				fprintf(stderr, "No matching iter found.\n");
				return -1;
			}
	}


	/* Ok, now we have the corresponding iter, we can select that entry */
	gtk_tree_selection_select_iter(selection, &iter_counter);

	return 0;
}


int
prefs_window_load(struct gtk_ui_data* data)
{
	gtk_entry_set_text(GTK_ENTRY(data->entry_editor), prefs_get_editor());
	gtk_entry_set_text(GTK_ENTRY(data->entry_cire_dir), prefs_get_cire_dir());
	gtk_entry_set_text(GTK_ENTRY(data->entry_timef), prefs_get_timef());
	
	return 1;
}

int
prefs_window_save(struct gtk_ui_data* data)
{
	prefs_set_editor(gtk_entry_get_text(GTK_ENTRY(data->entry_editor)));
	prefs_set_cire_dir(gtk_entry_get_text(GTK_ENTRY(data->entry_cire_dir)));
	prefs_set_timef(gtk_entry_get_text(GTK_ENTRY(data->entry_timef)));
	
	return 1;
}

#endif
