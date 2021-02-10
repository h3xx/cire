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
 * $Id: gtk_ui_main.h,v 1.20 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifndef __GTK_UI_MAIN_H
#define __GTK_UI_MAIN_H

#ifdef BUILD_GTK_UI

struct gtk_ui_data
{
	char* curfile;
	char* curfile_password;
	struct diary* diary;

	/* main window */
	GtkWidget* window_main;
	GtkWidget* vbox_main;

    GtkWidget* menu_bar_main;
    GtkItemFactoryEntry * menu_factory_items;
    GtkItemFactory * item_factory;
    guint n_factory_items;
    
	GtkWidget* main_hpaned;

	GtkWidget* right_vbox;
	GtkWidget* entry_editor_label;
	GtkWidget* scrolled_window_entry_editor;
		GtkWidget* text_view_entry_editor;
		int entry_editor_current_offset;
	GtkWidget* button_save_entry;

	GtkWidget* scrolled_window_entry_list;
		GtkWidget* tree_view_entry_list;
			GtkTreeViewColumn* entry_list_title_column;
				GtkCellRenderer* entry_list_title_renderer;
			GtkTreeViewColumn* entry_list_modified_column;
				GtkCellRenderer* entry_list_modified_renderer;
		GtkListStore* list_store_entry_list;

	/* prefs window */
	GtkWidget* window_prefs;
	GtkWidget* table_prefs;
	GtkWidget* lbl_editor;
		GtkWidget* entry_editor;
	GtkWidget* lbl_cire_dir;
		GtkWidget* entry_cire_dir;
	GtkWidget* lbl_timef;
		GtkWidget* entry_timef;
	GtkWidget* btn_ok;
	GtkWidget* btn_cancel;
	GtkWidget* btn_apply;

	/* html export window */
	GtkWidget* window_html_export;
	GtkWidget* table_html_export;
		GtkWidget* lbl_output_html;
			GtkWidget* entry_output_html;
			GtkWidget* btn_get_output_html;
			
		GtkWidget* chk_use_header_html;
			GtkWidget* lbl_header_html;
			GtkWidget* entry_header_html;
			GtkWidget* btn_get_header_html;
		
		GtkWidget* chk_use_footer_html;
			GtkWidget* lbl_footer_html;
			GtkWidget* entry_footer_html;
			GtkWidget* btn_get_footer_html;
			
		GtkWidget* btn_html_export_ok;
		GtkWidget* btn_html_export_cancel;

	/* find entry window */
	GtkWidget* window_find_entry;
	GtkWidget* table_find_entry;

	GtkWidget* lbl_search_for;
  GtkWidget* entry_search_for;

	GtkWidget* lbl_search_by;
	GSList* grp_search_by;
	GtkWidget* btn_search_by_contents;
	GtkWidget* btn_search_by_title;

	GtkWidget* btn_find_entry;
	GtkWidget* btn_find_entry_cancel;

};

int
gtk_ui_main(int argc, char** argv);

GtkWidget *
create_menu_bar( GtkWidget * window, struct gtk_ui_data * d );

#endif /* BUILD_GTK_UI */

#endif /* __GTK_UI_MAIN_H */
