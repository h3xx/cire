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
 * $Id: gtk_ui_main.c,v 1.28 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

 
#ifdef BUILD_GTK_UI

#include "main.h"


int
gtk_ui_main(int argc, char** argv)
{
	struct gtk_ui_data data;
	const char* start_file;
	const char* start_pass;

	data.curfile = 0;
	data.curfile_password = 0;
	data.entry_editor_current_offset = -1;

	/* create a diary object */
	data.diary = diary_new();

	/* initialize the prefs module */
	prefs_init(argc, argv);

	/* initialize gtk */
	gtk_init(&argc, &argv);

	/************ main window **********/
	data.window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	set_title_bar_text(&data, "Untitled");
	gtk_window_set_default_size(GTK_WINDOW(data.window_main), 600, 400);
	g_signal_connect(G_OBJECT(data.window_main), "delete_event",
	    G_CALLBACK(window_main_delete_event), (gpointer) &data);
	g_signal_connect(G_OBJECT(data.window_main), "destroy",
	    G_CALLBACK(window_main_destroy), (gpointer) &data);

	/* create a vbox for the main window */
	data.vbox_main = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(data.window_main), data.vbox_main);

	/* create the main menu bar */
	GtkItemFactoryEntry menu_items[] = {
	    { "/_Diary", NULL, NULL, 0, "<Branch>" },
	    { "/Diary/_New", "<Ctrl>N", menu_item_diary_new_activate, 0, "<StockItem>", GTK_STOCK_NEW },
	    { "/Diary/_Open...", "<Ctrl>O", menu_item_diary_open_activate, 0, "<StockItem>", GTK_STOCK_OPEN },
	    { "/Diary/_Save", "<Ctrl>S", menu_item_diary_save_activate, 0, "<StockItem>", GTK_STOCK_SAVE },
	    { "/Diary/Save _As...", NULL, menu_item_diary_save_as_activate, 0, "<StockItem>", GTK_STOCK_SAVE_AS },
	    { "/Diary/sep1", NULL, NULL, 0, "<Separator>" },
	    { "/Diary/_Quit", "<Ctrl>Q", menu_item_diary_quit_activate, 0, "<StockItem>", GTK_STOCK_QUIT},
	    { "/_Entry", NULL, NULL, 0, "<Branch>" },
	    { "/Entry/_Add", "<Ctrl><Shift>A", menu_item_entry_add_activate, 0, "<StockItem>", GTK_STOCK_ADD },
	    { "/Entry/_Delete", "<Ctrl><Shift>D", menu_item_entry_remove_activate, 0, "<StockItem>", GTK_STOCK_REMOVE },
	    { "/Entry/_Rename", "<Ctrl><Shift>R", menu_item_entry_rename_activate, 0, "<Item>" },
	    { "/_Tools", NULL, NULL, 0, "<Branch>" },
			{ "/Tools/_Find Entry", "<Ctrl>F", menu_item_tools_find_activate, 0, "<StockItem>", GTK_STOCK_FIND },
	    { "/Tools/_Export", NULL, NULL, 0, "<Branch>" },
	    { "/Tools/Export/To _Html", NULL, menu_item_tools_export_html_activate, 0, "<StockItem>", GTK_STOCK_CONVERT },
	    { "/Tools/Export/To Plain _Text", NULL, menu_item_tools_export_text_activate, 0, "<StockItem>", GTK_STOCK_CONVERT },
	    { "/Tools/sep1", NULL, NULL, 0, "<Separator>" },
	    { "/Tools/_Preferences", NULL, menu_item_tools_preferences_activate, 0, "<StockItem>", GTK_STOCK_PREFERENCES },
	    { "/_Help", NULL, NULL, 0, "<Branch>" },
	    { "/Help/_About", NULL, menu_item_help_about_activate, 0, "<Item>" }
    		/* TODO: I've seen other apps (eg gedit) using what appears to be a stock item form help->about. I'd like
    		 * to see that used here, but I can't find anything about it in the gtk docs.
    		 */
	};

	data.menu_factory_items = menu_items;
	data.n_factory_items = sizeof(menu_items) / sizeof(menu_items[0]);
	
	data.menu_bar_main = create_menu_bar(data.window_main, &data);
 	gtk_box_pack_start(GTK_BOX(data.vbox_main), data.menu_bar_main,
 		FALSE, FALSE, 0);


	/* create an hpaned for the main window */
	data.main_hpaned = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(data.main_hpaned), 200);
	gtk_box_pack_start(GTK_BOX(data.vbox_main), data.main_hpaned, TRUE, TRUE, 0);

	/* create the vbox on the right */
	data.right_vbox = gtk_vbox_new(FALSE, 0);
	gtk_paned_pack2(GTK_PANED(data.main_hpaned), data.right_vbox,
		TRUE, FALSE);

	/* create the entry label */
	data.entry_editor_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(data.entry_editor_label),
			     "<b><big>No Entry Selected</big></b>");
	gtk_misc_set_padding(GTK_MISC(data.entry_editor_label), 0, 3);
	gtk_box_pack_start(GTK_BOX(data.right_vbox),
				data.entry_editor_label, FALSE, TRUE, 0);
	
	/* create the 'entry editor' scrolled window */
	data.scrolled_window_entry_editor = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(data.right_vbox),
		data.scrolled_window_entry_editor, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(
	    (GtkScrolledWindow*) data.scrolled_window_entry_editor,
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	/* create the 'entry editor' text view */
	data.text_view_entry_editor = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(data.text_view_entry_editor),
		GTK_WRAP_WORD);
	gtk_container_add(GTK_CONTAINER(data.scrolled_window_entry_editor),
	    data.text_view_entry_editor);

	/* create the 'save entry' button */
	data.button_save_entry = gtk_button_new_from_stock(GTK_STOCK_SAVE);
	g_signal_connect(G_OBJECT(data.button_save_entry), "clicked",
	    G_CALLBACK(button_save_entry_clicked), (gpointer) &data);
	gtk_box_pack_start(GTK_BOX(data.right_vbox),
		data.button_save_entry, FALSE, FALSE, 0);

	/* create the 'entry list' scrolled window */
	data.scrolled_window_entry_list = gtk_scrolled_window_new(NULL, NULL);
	gtk_paned_pack1(GTK_PANED(data.main_hpaned),
		data.scrolled_window_entry_list, TRUE, FALSE);

	/* create the 'entry list' list store */
	data.list_store_entry_list = gtk_list_store_new(
	    2, G_TYPE_STRING, G_TYPE_STRING);

	/* create the 'entry list' tree view */
	data.tree_view_entry_list = gtk_tree_view_new_with_model(
	    GTK_TREE_MODEL(data.list_store_entry_list));

	data.entry_list_title_renderer = gtk_cell_renderer_text_new();
	data.entry_list_title_column = gtk_tree_view_column_new_with_attributes(
	    "Title", data.entry_list_title_renderer, "text", 0, NULL);
	gtk_tree_view_column_set_clickable(data.entry_list_title_column, TRUE);
	g_signal_connect(G_OBJECT(data.entry_list_title_column), "clicked",
		G_CALLBACK(entry_list_title_column_clicked), (gpointer) &data);
	gtk_tree_view_append_column(GTK_TREE_VIEW(data.tree_view_entry_list),
	    data.entry_list_title_column);

	data.entry_list_modified_renderer = gtk_cell_renderer_text_new();
	data.entry_list_modified_column = gtk_tree_view_column_new_with_attributes(
	    "Modified", data.entry_list_modified_renderer, "text", 1, NULL);
	gtk_tree_view_column_set_clickable(data.entry_list_modified_column, TRUE);
	g_signal_connect(G_OBJECT(data.entry_list_modified_column), "clicked",
		G_CALLBACK(entry_list_modified_column_clicked), (gpointer) &data);
	gtk_tree_view_append_column(GTK_TREE_VIEW(data.tree_view_entry_list),
	    data.entry_list_modified_column);

	g_signal_connect(G_OBJECT(data.tree_view_entry_list), "button-press-event",
		G_CALLBACK(entry_list_button_press_event), (gpointer) &data);

	g_signal_connect(G_OBJECT(gtk_tree_view_get_selection(
		GTK_TREE_VIEW(data.tree_view_entry_list))), "changed",
		G_CALLBACK(tree_selection_entry_list_changed), (gpointer) &data);

	gtk_container_add(GTK_CONTAINER(data.scrolled_window_entry_list),
		data.tree_view_entry_list);

	/*********** Prefs window ************/
	data.window_prefs = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(data.window_prefs), FALSE);
	gtk_window_set_title(GTK_WINDOW(data.window_prefs), "Cire Preferences");
	gtk_window_set_modal(GTK_WINDOW(data.window_prefs), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(data.window_prefs),
		GTK_WINDOW(data.window_main));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window_prefs), TRUE);
	g_signal_connect(G_OBJECT(data.window_prefs), "delete_event",
	    G_CALLBACK(window_prefs_delete_event), (gpointer) &data);


	/* prefs table */
	data.table_prefs = gtk_table_new(3, 4, FALSE);
	gtk_container_add(GTK_CONTAINER(data.window_prefs), data.table_prefs);
	gtk_widget_show(data.table_prefs);

	/* editor label */
	data.lbl_editor = gtk_label_new("Editor to use with the ncurses interface:");
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.lbl_editor,
		0, 2, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_editor);

	/* editor entry */
	data.entry_editor = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.entry_editor,
		2, 3, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_editor);

	/* cire dir label */
	data.lbl_cire_dir = gtk_label_new("Default directory to place files in:");
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.lbl_cire_dir,
		0, 2, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_cire_dir);

	/* cire dir entry */
	data.entry_cire_dir = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.entry_cire_dir,
		2, 3, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_cire_dir);

	/* timef label */
	data.lbl_timef = gtk_label_new("Format in which to show the modification time:");
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.lbl_timef,
		0, 2, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_timef);

	/* timef entry */
	data.entry_timef = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.entry_timef,
		2, 3, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_timef);

	/* ok button */
	data.btn_ok = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(data.btn_ok), "clicked",
		G_CALLBACK(prefs_window_ok_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.btn_ok,
		0, 1, 3, 4, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_ok);

	/* cancel button */
	data.btn_cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(data.btn_cancel), "clicked",
		G_CALLBACK(prefs_window_cancel_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.btn_cancel,
		1, 2, 3, 4, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_cancel);

	/* apply button */
	data.btn_apply = gtk_button_new_from_stock(GTK_STOCK_APPLY);
	g_signal_connect(G_OBJECT(data.btn_apply), "clicked",
		G_CALLBACK(prefs_window_apply_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_prefs), data.btn_apply,
		2, 3, 3, 4, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_apply);


	/*********** HTML export window ************/
	data.window_html_export = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(data.window_html_export), FALSE);
	gtk_window_set_title(GTK_WINDOW(data.window_html_export), "Export to HTML");
	gtk_window_set_modal(GTK_WINDOW(data.window_html_export), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(data.window_html_export),
		GTK_WINDOW(data.window_main));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window_html_export), TRUE);
	g_signal_connect(G_OBJECT(data.window_html_export), "delete_event",
	    G_CALLBACK(window_html_export_delete_event), (gpointer) &data);

	/* html export table */
	data.table_html_export = gtk_table_new(4, 4, FALSE);
	gtk_container_add(GTK_CONTAINER(data.window_html_export), data.table_html_export);
	gtk_widget_show(data.table_html_export);

	/* output label */
	data.lbl_output_html = gtk_label_new("Output HTML to file:");
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.lbl_output_html,
		1, 2, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_output_html);

	/* output entry */
	data.entry_output_html = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.entry_output_html,
		2, 3, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_output_html);

	/* output get filename from dialog button */
	data.btn_get_output_html = gtk_button_new_with_label("...");
	g_signal_connect(G_OBJECT(data.btn_get_output_html), "clicked",
		G_CALLBACK(html_export_get_output_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.btn_get_output_html,
		3, 4, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_get_output_html);

	/* header toggle */
	data.chk_use_header_html = gtk_check_button_new();
	g_signal_connect(G_OBJECT(data.chk_use_header_html), "toggled",
		G_CALLBACK(html_export_use_header_html_toggled), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.chk_use_header_html,
		0, 1, 1, 2, GTK_FILL, GTK_FILL, 10, 10);
	gtk_widget_show(data.chk_use_header_html);

	/* header file label */
	data.lbl_header_html = gtk_label_new("Use HTML Header File:");
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.lbl_header_html,
		1, 2, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_header_html);

	/* header file entry */
	data.entry_header_html = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.entry_header_html,
		2, 3, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_header_html);

	/* header get filename from dialog button */
	data.btn_get_header_html = gtk_button_new_with_label("...");
	g_signal_connect(G_OBJECT(data.btn_get_header_html), "clicked",
		G_CALLBACK(html_export_get_header_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.btn_get_header_html,
		3, 4, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_get_header_html);

	/* footer toggle */
	data.chk_use_footer_html = gtk_check_button_new();
	g_signal_connect(G_OBJECT(data.chk_use_header_html), "toggled",
		G_CALLBACK(html_export_use_footer_html_toggled), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.chk_use_footer_html,
		0, 1, 2, 3, GTK_FILL, GTK_FILL, 10, 10);
	gtk_widget_show(data.chk_use_footer_html);
	
	/* footer file label */
	data.lbl_footer_html = gtk_label_new("Use HTML Footer File:");
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.lbl_footer_html,
		1, 2, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_footer_html);

	/* footer file entry */
	data.entry_footer_html = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.entry_footer_html,
		2, 3, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_footer_html);

	/* footer get filename from dialog button */
	data.btn_get_footer_html = gtk_button_new_with_label("...");
	g_signal_connect(G_OBJECT(data.btn_get_footer_html), "clicked",
		G_CALLBACK(html_export_get_footer_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.btn_get_footer_html,
		3, 4, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_get_footer_html);

	/* ok button */
	data.btn_html_export_ok = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(data.btn_html_export_ok), "clicked",
		G_CALLBACK(html_export_ok_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.btn_html_export_ok,
		0, 2, 3, 4, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_html_export_ok);

	/* cancel button */
	data.btn_html_export_cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(data.btn_html_export_cancel), "clicked",
		G_CALLBACK(html_export_cancel_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_html_export), data.btn_html_export_cancel,
		2, 4, 3, 4, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_html_export_cancel);


	/*********** Find entry window ************/
	data.window_find_entry = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(data.window_find_entry), FALSE);
	gtk_window_set_title(GTK_WINDOW(data.window_find_entry), "Find Entry");
	gtk_window_set_modal(GTK_WINDOW(data.window_find_entry), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(data.window_find_entry),
		GTK_WINDOW(data.window_main));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window_find_entry), TRUE);
	g_signal_connect(G_OBJECT(data.window_find_entry), "delete_event",
	    G_CALLBACK(window_find_entry_delete_event), (gpointer) &data);


	/* find entry table */
	data.table_find_entry = gtk_table_new(3, 3, FALSE);
	gtk_container_add(GTK_CONTAINER(data.window_find_entry), data.table_find_entry);
	gtk_widget_show(data.table_find_entry);

	/* search for label */
	data.lbl_search_for = gtk_label_new("Search for:");
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.lbl_search_for,
		0, 1, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_search_for);


	/* search for text area */
	data.entry_search_for = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.entry_search_for,
		1, 3, 0, 1, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.entry_search_for);


	/* search by label */
	data.lbl_search_by = gtk_label_new("Search by:");
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.lbl_search_by,
		0, 1, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.lbl_search_by);
	

	/* search by radio buttons */
	/* contents radio button */
	data.btn_search_by_contents = gtk_radio_button_new_with_mnemonic (NULL, "_Contents");
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.btn_search_by_contents,
									 1, 2, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show (data.btn_search_by_contents);


	/* radio button group */
	data.grp_search_by = gtk_radio_button_get_group (GTK_RADIO_BUTTON (data.btn_search_by_contents));

	/* title radio button */
	data.btn_search_by_title = gtk_radio_button_new_with_mnemonic (data.grp_search_by, "_Title");
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.btn_search_by_title,
									 2, 3, 1, 2, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show (data.btn_search_by_title);

	/* find button */
	data.btn_find_entry = gtk_button_new_from_stock(GTK_STOCK_FIND);
	g_signal_connect(G_OBJECT(data.btn_find_entry), "clicked",
		G_CALLBACK(find_entry_find_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.btn_find_entry,
									 0, 1, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_find_entry);
	
	/* cancel button */
	data.btn_find_entry_cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(data.btn_find_entry_cancel), "clicked",
									 G_CALLBACK(find_entry_cancel_clicked), (gpointer) &data);
	gtk_table_attach(GTK_TABLE(data.table_find_entry), data.btn_find_entry_cancel,
									 2, 3, 2, 3, GTK_FILL, GTK_EXPAND, 10, 10);
	gtk_widget_show(data.btn_find_entry_cancel);
	
	  	
	/* if we were given a filename on the command line, load it */
	if( (start_file = prefs_get_argv_file()) )
	{
		if( !misc_file_exists(start_file) ) { goto main_loop; }
		
		if(misc_file_is_encrypted(start_file))
		{
			start_pass = get_text_via_dialog(&data, "Enter password:",
			"The file you selected seems to be\nencrypted, please enter the password:",
			TRUE);
			
			if(!start_pass) { goto main_loop; }
		}
		else start_pass = NULL;
		
		if( diary_load_from_file(data.diary, start_file, start_pass) )
		{
			report_error_via_dialog(&data, "Failed to open file.");
			goto main_loop;
		}
		
		set_curfile(&data, start_file);
		set_curfile_password(&data, start_pass);
		sync_diary_and_entry_list(&data);
		set_title_bar_text(&data, start_file);
	}
	
	/*********** main loop ***********/
	main_loop:
		gtk_widget_show_all(data.window_main);
		gtk_main();

	/* deinit prefs module */
	if( prefs_save() )
		report_error_via_dialog(&data, "Couldn't save preferences.");

	prefs_free();

	return 0;
}


GtkWidget *
create_menu_bar(GtkWidget * window, struct gtk_ui_data * d)
{
    /* some/most of this is copied from the gtk tutorial */
    
    GtkAccelGroup *accel_group;

    /* Make an accelerator group (shortcut keys) */
    accel_group = gtk_accel_group_new ();
    
    /* Make an ItemFactory (that makes a menubar) */
    d->item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",
					 accel_group);

    /* This function generates the menu items. Pass the item factory,
       the number of items in the array, the array itself, and any
       callback data for the the menu items. */
    gtk_item_factory_create_items (d->item_factory, d->n_factory_items, d->menu_factory_items, d);

    /* Attach the new accelerator group to the window. */
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

    /* Finally, return the actual menu bar created by the item factory. */
    return gtk_item_factory_get_widget (d->item_factory, "<main>");

}

#endif
