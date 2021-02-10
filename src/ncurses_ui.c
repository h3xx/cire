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
 * $Id: ncurses_ui.c,v 1.52 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifdef BUILD_NCURSES_UI
#include "main.h"

int 
ncurses_ui_main(int argc, char** argv)
{
	struct nci* nci;
	int quit = FALSE;
	int key;
	
	nci = malloc(sizeof(struct nci));
	nci->diary = diary_new();
	nci->filename = NULL;
	nci->password = NULL;
	
	nci->modified = FALSE;
	nci->selected_entry_offset = -1;
	
	nci->sort_type = sort_timestamp;
	nci->sort_direction = sort_ascending;
	nci->sorting_set = FALSE;
	
	nci->last_search_string = NULL;
	nci->last_search_offset = -1;
	nci->last_search_type = -1;
		
	prefs_init(argc, argv);

	
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	nci_load_argv_file(nci);
	
	while(!quit)
	{
		nci_update_screen(nci);
		nci_print_footer("Main Menu: |D|iary  |E|ntry  |T|ools  |S|ort  |H|elp  |Q|uit");

		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;

		switch(key)
		{
			case 'd':
			case 'D': nci_diary_menu(nci); break;
			
			case 'e':
			case 'E': nci_entry_menu(nci); break;
			
			case 't':
			case 'T': nci_tools_menu(nci); break;
			
			case 's':
			case 'S': nci_sort_menu(nci); break;
			
			case 'h':
			case 'H': nci_help_menu(nci); break;

			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q': quit = nci_quit(nci); break;
			
			default: beep();
		}
	}
	
	endwin();
	diary_free(nci->diary);
	free(nci->filename);
	free(nci->password);
	free(nci);
	prefs_save();
	prefs_free();
	return 0;
}

int
nci_load_argv_file(struct nci* nci)
{
	const char* filename;
	const char* password = NULL;
	
	if( !prefs_get_argv_file() )
		return 0;
	
	filename = misc_interpolate_filename(prefs_get_argv_file());
	
	if( !misc_file_exists(filename) )
		if( misc_file_exists(misc_force_file_extension(filename, ".cire")) )
			filename = misc_force_file_extension(filename, ".cire");
	
	if(misc_file_is_encrypted(filename))
	{
		password = nci_get_user_text(nci, "Password");
		if(!password)
			return 0;
	}
	
	if(diary_load_from_file(nci->diary, filename, password))
	{
		nci_press_any_key(nci, errmsg);
		
		return 1;
	}
	else
	{
		misc_str_assign(&(nci->filename), filename);
		misc_str_assign(&(nci->password), password);
		
		return 0;
	}
}	

int
nci_diary_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Diary Menu: |N|ew  |O|pen  |S|ave  Save |A|s  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
		
		switch(key)
		{
			case 'n':
			case 'N': nci_diary_new(nci); return 0;
			
			case 'o':
			case 'O': nci_diary_open(nci); return 0;
			
			case 's':
			case 'S': nci_diary_save(nci); return 0;
			
			case 'a':
			case 'A': nci_diary_save_as(nci); return 0;

			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_entry_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Entry Menu: |A|dd  |E|dit  |V|iew  |D|elete  |R|ename  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 'a':
			case 'A': nci_entry_add(nci); return 0;
			
			case 'e':
			case 'E': nci_entry_edit(nci); return 0;
			
			case 'v':
			case 'V': nci_entry_view(nci); return 0;
			
			case 'd':
			case 'D': nci_entry_delete(nci); return 0;
			
			case 'r':
			case 'R': nci_entry_rename(nci); return 0;
			
			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_tools_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Tools Menu: |S|earch  |E|xport  |P|references  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 'p':
			case 'P': nci_tools_prefs(nci); return 0;
			
			case 'x':
			case 'X':
			case 'e':
			case 'E': nci_tools_export_menu(nci); return 0;
			
			case 's':
			case 'S':
			case 'f':
			case 'F': nci_tools_search_menu(nci); return 0;

			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_tools_export_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Export Menu: To: |H|TML  Plain |T|ext  |L|atex  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 'h':
			case 'H': nci_tools_export_html(nci); return 0;
			
			case 't':
			case 'T': nci_tools_export_text(nci); return 0;

			case 'l':
			case 'L': nci_tools_export_latex(nci); return 0;
		    
			
			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_sort_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Sort Menu: Toggle |T|ype  Toggle |D|irection  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 't':
			case 'T':
				if(nci->sort_type == sort_timestamp)
					nci->sort_type = sort_title;
				else
					nci->sort_type = sort_timestamp;
				
				nci->sorting_set = TRUE;
				return 0;
			
			case 'd':
			case 'D':
				if(nci->sort_direction == sort_ascending)
					nci->sort_direction = sort_descending;
				else
					nci->sort_direction = sort_ascending;
				
				nci->sorting_set = TRUE;
				return 0;
			
			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_help_menu(struct nci* nci)
{	
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Help Menu: |A|bout  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 'a':
			case 'A': nci_help_about(nci); return 0;

			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B': return 0;
			
			default: beep();
		}
	}
}

int
nci_select_next(struct nci* nci)
{
	++nci->selected_entry_offset;
	
	if(nci->selected_entry_offset >= diary_get_entry_count(nci->diary))
	{
		nci->selected_entry_offset = diary_get_entry_count(nci->diary) - 1;
		beep();
	}

	return 0;
}

int
nci_select_prev(struct nci* nci)
{
	--nci->selected_entry_offset;
	
	if(nci->selected_entry_offset < 0)
	{
		nci->selected_entry_offset = 0;
		beep();
	}
	return 0;
}

int
nci_quit(struct nci* nci)
{
	if(nci->modified)
	{
		while(TRUE)
		{
			nci_update_screen(nci);
			nci_print_footer
				("Save changes before quiting? (|Y|es / |N|o / |C|ancel)");
		
			switch(getch())
			{
				case 'y':
				case 'Y': nci_diary_save(nci); return TRUE;
			
				case 'n':
				case 'N': return TRUE;
			
				case 'c':
				case 'C': return FALSE;
			
				default: beep();
			}
		}
	}
	else
	{
		return TRUE;
	}
}

int
nci_diary_new(struct nci* nci)
{
	int end_loop = FALSE;
	
	if(nci->modified)
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("Save changes first? (|Y|es / |N|o / |C|ancel)");
		
			switch(getch())
			{
				case 'y':
				case 'Y': nci_diary_save(nci); end_loop = TRUE; break;
				
				case 'n':
				case 'N': end_loop = TRUE; break;
			
				case NCI_KEY_ESCAPE:
				case 'c':
				case 'C': return 0;
				
				default: beep();
			}
		}
	}
	
	diary_clear(nci->diary);
	misc_str_assign(&(nci->filename), NULL);
	misc_str_assign(&(nci->password), NULL);
	nci->modified = FALSE;
	nci->selected_entry_offset = -1;
	
	return 0;
}

int
nci_diary_open(struct nci* nci)
{
	int end_loop = FALSE;
	const char* filename;
	const char* password = NULL;
	
	if(nci->modified)
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("Save changes first? (|Y|es / |N|o / |C|ancel)");
			
			switch(getch())
			{
				case 'y':
				case 'Y': nci_diary_save(nci); end_loop = TRUE; break;
				
				case 'n':
				case 'N': end_loop = TRUE; break;
				
				case NCI_KEY_ESCAPE:
				case 'c':
				case 'C': return 0;
				
				default: beep();
			}
		}
	}
	
	filename = nci_get_user_text(nci, "Filename");
	if(!filename)
		return 0;
	
	filename = misc_interpolate_filename(filename);
	
	/* if the name given doesn't exist, and the name given + .cire /does/
		exist then use it. */
	if(!misc_file_exists(filename))
		if(misc_file_exists(misc_force_file_extension(filename, ".cire")))
			filename = misc_force_file_extension(filename, ".cire");
	
	if(misc_file_is_encrypted(filename))
	{
		password = nci_get_user_text(nci, "Password");
		if(!password)
			return 0;
	}
	
	/* diary_load_from_file() *should* do this... but i'm not sure it does */
	diary_clear(nci->diary);
	
	if(diary_load_from_file(nci->diary, filename, password))
	{
		nci_press_any_key(nci, errmsg);
		
		misc_str_assign(&(nci->filename), NULL);
		misc_str_assign(&(nci->password), NULL);
		nci->modified = FALSE;
		nci->selected_entry_offset = -1;
		
		return 1;
	}
	else
	{
		misc_str_assign(&(nci->filename), filename);
		misc_str_assign(&(nci->password), password);
		nci->modified = FALSE;
		nci->selected_entry_offset = -1;
		
		return 0;
	}
}	

int
nci_diary_save(struct nci* nci)
{
	if(nci->filename)
	{
		if(diary_save_to_file(nci->diary, nci->filename, nci->password))
		{
			nci_press_any_key(nci, errmsg);

			return 1;
		}
		else
		{
			nci->modified = FALSE;
			return 0;
		}
	}
	else
	{
		return nci_diary_save_as(nci);
	}
}

int
nci_diary_save_as(struct nci* nci)
{
	int end_loop = FALSE;
	const char* filename;
	const char* password = NULL;
	
	filename = nci_get_user_text(nci, "Filename");
	if(!filename)
		return 0;
		
	filename = misc_interpolate_filename(filename);
	
	if(!misc_file_exists(filename))
		filename = misc_force_file_extension(filename, ".cire");
	
	if(misc_file_exists(filename))
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("File already exists, overwrite it? (|Y|es / |N|o)");
			
			switch(getch())
			{
				case 'y':
				case 'Y': end_loop = TRUE; break;
				
				case NCI_KEY_ESCAPE:
				case 'n':
				case 'N': return 0;
				
				default: beep();
			}
		}
	}
	
	password = nci_get_user_text(nci, "Password (blank for none)");
	if(password)
		if(*password == '\0')
			password = NULL;
	
	if(diary_save_to_file(nci->diary, filename, password))
	{
		nci_press_any_key(nci, errmsg);
		
		return 1;
	}
	else
	{
		misc_str_assign(&(nci->filename), filename);
		misc_str_assign(&(nci->password), password);
		nci->modified = FALSE;
	
		return 0;
	}
}

int
nci_entry_add(struct nci* nci)
{
	const char* title;
	struct entry* entry;
	
	title = nci_get_user_text(nci, "Title");
	if(!title)
		return 0;
	
	entry = entry_new();
	entry_set_title(entry, title);
	diary_add_entry(nci->diary, entry);
	nci->selected_entry_offset = diary_get_entry_count(nci->diary) - 1;
	nci->modified = TRUE;

	nci_entry_edit(nci);
	return 0;
}

int
nci_entry_edit(struct nci* nci)
{
	char* command;
	char* tmpfile;
	struct entry* entry;
	const char* contents;
	char* data;
	int data_len;
	
	if(nci->selected_entry_offset == -1)
	{
		beep();
		return 0;
	}
	
	tmpfile = malloc(16);
	strcpy(tmpfile, "/tmp/cire-XXXXXX");
	close(mkstemp(tmpfile));

	endwin();
	
	entry = diary_get_entry_by_offset(nci->diary, nci->selected_entry_offset);
	contents = entry_get_contents(entry);
	if(!contents)
		contents = "";
	
	if(misc_save_file(contents, strlen(contents), tmpfile))
	{
		free(tmpfile);
		return 1;
	}
	
	command = malloc(strlen(prefs_get_editor()) + 1 + strlen(tmpfile) + 1);
	strcpy(command, prefs_get_editor());
	strcat(command, " ");
	strcat(command, tmpfile);
	system(command);
	free(command);
	
	if(misc_load_file(&data, &data_len, tmpfile))
	{
		free(tmpfile);
		return 1;
	}
	
	entry_set_contents(entry, data);
	free(data);
	nci->modified = TRUE;
	
	command = malloc(6 + strlen(tmpfile) + 1);
	strcpy(command, "rm -f ");
	strcat(command, tmpfile);
	system(command);
	free(command);
	
	reset_prog_mode();
	refresh();
	curs_set(0);
	free(tmpfile);
	return 0;
}

int
nci_entry_view(struct nci* nci)
{
	const char* contents;
	
	if(nci->selected_entry_offset < 0)
	{
		beep();
		return 0;
	}
	
	contents = entry_get_contents(diary_get_entry_by_offset
		(nci->diary, nci->selected_entry_offset));
	
	if(!contents)
		contents = "";
		
	return nci_text_viewer(nci, contents);
}

int
nci_text_viewer(struct nci* nci, const char* text)
{
	int y = 0;
	int x = 0;
	int i;
	int cur_line = 0;
	int num_lines = 0;
	int offset = 0;
	char footer[48];
	char* search_string = NULL;
	int highlighting_end = -1;
	char* tmp;
	char* tmp2;
	
	for(i = 0; text[i]; ++i)
	{
		if(text[i] == '\n')
			num_lines++;
	}
	
	while(TRUE)
	{
		nci_update_header(nci);
		
		if(num_lines > 0)
			sprintf(footer, "Line %d of %d - %d%%\t\t|S|earch",
				cur_line + 1, num_lines, 100 * cur_line / num_lines);
		else
			sprintf(footer, "Line 0 of 0 - 0%%");
		nci_print_footer(footer);
		
		for(i = 0; i < LINES - 4; ++i)
		{
			move(i + 2, 0);
			clrtoeol();
		}
		
		i = 0;
		y = 0;
		x = 0;
		move(2, 0);
		while(i < strlen(text + offset) && y < LINES - 2)
		{
			if(x == 0) addstr("    ");
			
			if(search_string)
			{
				tmp = malloc(strlen(search_string) + 1);
				strncpy(tmp, text + offset + i, strlen(search_string));
				tmp2 = misc_str_to_upper(tmp);
				
				if( !strcmp(search_string, tmp2) )
				{
					attron(A_REVERSE);
					highlighting_end = i + strlen(search_string);
				}
				
				free(tmp);
				free(tmp2);

				if(i == highlighting_end)
					attroff(A_REVERSE);
			}
				
			addch( *(text + offset + i) );
			
			++i;
			getyx(stdscr, y, x);
		}
		
		switch(getch())
		{
			case KEY_DOWN:
			case 'j':
			case 'J':
				for(i = 0; i < strlen(text + offset); ++i)
				{
					if( *(text + offset + i) == '\n' )
					{
						if(i + 1 < strlen(text + offset))
						{
							offset += i + 1;
							cur_line++;
							goto end_down;
						}
					}
				}

				beep();
				end_down: break;
			
			case KEY_UP:
			case 'k':
			case 'K':
				if(offset == 0)
				{
					beep();
					break;
				}
				
				offset -= 2;
				cur_line--;
				while(offset > 0)
				{
					if( *(text + offset) == '\n' )
					{
						++offset;
						goto end_up;
					}
					else
						--offset;
				}

				offset = 0;
				end_up: break;
			
			case '/':
			case 'f':
			case 'F':
			case 's':
			case 'S':
				search_string = misc_str_to_upper(
					nci_get_user_text(nci, "Search") );
						
				break;
		
			case NCI_KEY_ESCAPE:
			case 'q':
			case 'Q':
			case 'b':
			case 'B':
				free(search_string);
				return 0;
			
			default: beep();
		}
	}
}

int
nci_entry_delete(struct nci* nci)
{
	if(nci->selected_entry_offset == -1)
	{
		beep();
		return 0;
	}
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer
			("Are you sure you want to delete the selected entry? (|Y|es / |N|o)");
		
		switch(getch())
		{
			case 'y':
			case 'Y':
				diary_remove_entry_by_offset(nci->diary, nci->selected_entry_offset);
				
				if(nci->selected_entry_offset >= diary_get_entry_count(nci->diary))
					nci->selected_entry_offset = diary_get_entry_count(nci->diary) - 1;
				
				nci->modified = TRUE;
				return 0;
			
			case 'n':
			case 'N': return 0;
				
			default: beep();
		}
	}
}

int
nci_entry_rename(struct nci* nci)
{
	const char* new_title;
	
	if(nci->selected_entry_offset == -1)
	{
		beep();
		return 0;
	}
	
	new_title = nci_get_user_text(nci, "New name");
	if(!new_title)
		return 0;
		
	entry_set_title(diary_get_entry_by_offset(nci->diary,
		nci->selected_entry_offset), new_title);
	
	nci->modified = TRUE;
	
	return 0;
}

int
nci_tools_export_text(struct nci* nci)
{
	int end_loop = FALSE;
	const char* filename;
	
	filename = nci_get_user_text(nci, "Filename");
	if(!filename)
		return 0;
	
	filename = misc_interpolate_filename(filename);
	
	if(!misc_file_exists(filename))
		filename = misc_force_file_extension(filename, ".txt");
	
	if(misc_file_exists(filename))
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("File exists, overwrite it? (|Y|es / |N|o)");
			
			switch(getch())
			{
				case 'y':
				case 'Y': end_loop = TRUE; break;
				
				case NCI_KEY_ESCAPE:
				case 'c':
				case 'C':
				case 'n':
				case 'N': return 0;
				
				default: beep();
			}
		}
	}
	
	if(diary_export_text(nci->diary, filename))
	{
		nci_press_any_key(nci, errmsg);

		return 1;
	}
	else
		return 0;
}

int
nci_tools_export_html(struct nci* nci)
{
	int retval = 0;
	int end_loop = FALSE;
	char* filename = NULL;
	char* header = NULL;
	char* footer = NULL;
	
	misc_str_assign(&filename, nci_get_user_text(nci, "Filename"));
	if(!filename)
		return 0;
		
	misc_str_assign(&filename, misc_interpolate_filename(filename));
	
	if(!misc_file_exists(filename))
		misc_str_assign(&filename, misc_force_file_extension(filename, ".html"));
	
	if(misc_file_exists(filename))
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("File exists, overwrite it? (|Y|es / |N|o)");
			
			switch(getch())
			{
				case 'y':
				case 'Y': end_loop = TRUE; break;
				
				case NCI_KEY_ESCAPE:
				case 'c':
				case 'C':
				case 'n':
				case 'N': retval = 0; goto end;
				
				default: beep();
			}
		}
	}
	
	misc_str_assign(&header, nci_get_user_text(nci, "Header file (blank for none)"));
	if(*header == '\0')
	{
		free(header);
		header = NULL;
	}

	if(header)
	{
		misc_str_assign(&header, misc_interpolate_filename(header));
		
		if(!misc_file_exists(header))
			misc_str_assign(&header, misc_force_file_extension(header, ".html"));
		
		if(!misc_file_exists(header))
		{
			nci_press_any_key(nci, "File does not exist.");
			
			retval = 0;
			goto end;
		}
	}
	
	misc_str_assign(&footer, nci_get_user_text(nci, "Footer file (blank for none)"));
	if(*footer == '\0')
	{
		free(footer);
		footer = NULL;
	}
	
	if(footer)
	{
		misc_str_assign(&footer, misc_interpolate_filename(footer));
		
		if(!misc_file_exists(footer))
			misc_str_assign(&footer, misc_force_file_extension(footer, ".html"));
		
		if(!misc_file_exists(footer))
		{
			nci_press_any_key(nci, "File does not exist.");
			
			retval = 0;
			goto end;
		}
	}
	
	if(diary_export_html_advanced(nci->diary, filename, header, footer))
	{
		nci_press_any_key(nci, errmsg);

		retval = 1;
		goto end;
	}
	else
	{
		retval = 0;
		goto end;
	}
	
	end:
		free(filename);
		free(header);
		free(footer);
		return retval;
}

int
nci_tools_export_latex(struct nci* nci)
{
	int end_loop = FALSE;
	const char* filename;
	
	filename = nci_get_user_text(nci, "Filename");
	if(!filename)
		return 0;
	
	filename = misc_interpolate_filename(filename);
	
	if(!misc_file_exists(filename))
		filename = misc_force_file_extension(filename, ".tex");
	
	if(misc_file_exists(filename))
	{
		while(!end_loop)
		{
			nci_update_screen(nci);
			nci_print_footer("File exists, overwrite it? (|Y|es / |N|o)");
			
			switch(getch())
			{
				case 'y':
				case 'Y': end_loop = TRUE; break;
				
				case NCI_KEY_ESCAPE:
				case 'c':
				case 'C':
				case 'n':
				case 'N': return 0;
				
				default: beep();
			}
		}
	}
	
	if(diary_export_latex(nci->diary, filename))
	{
		nci_press_any_key(nci, errmsg);

		return 1;
	}
	else
		return 0;

}

int
nci_tools_search_menu(struct nci* nci)
{
	int key;
	
	while(TRUE)
	{
		nci_update_screen(nci);
		nci_print_footer("Search Menu: By |T|itle  By |C|ontents  |N|ext  |B|ack");
		
		key = getch();
		if( nci_global_key_handler(nci, key) )
			continue;
			
		switch(key)
		{
			case 't':
			case 'T': nci_tools_search(nci, search_by_title); return 0;
			
			case 'c':
			case 'C': nci_tools_search(nci, search_by_contents); return 0;
			
			case 'n':
			case 'N': nci_tools_search_next(nci); return 0;
			
			case NCI_KEY_ESCAPE:
			case 'b':
			case 'B':
			case 'q':
			case 'Q': return 0;
			
			default: beep();
		}
	}
}

int
nci_tools_search(struct nci* nci, enum search_type type)
{
	int offset;
	const char* needle;
	
	needle = nci_get_user_text(nci, "Search for");
	if(!needle)
		return 0;
	
	offset = diary_search(nci->diary, type, needle, 0);
	
	if(offset >= 0)
	{
		nci->selected_entry_offset = offset;
		
		misc_str_assign(&(nci->last_search_string), needle);
		nci->last_search_type = type;
		nci->last_search_offset = offset;
	}
	else
	{
		beep();
		nci_print_footer("Not found. Press any key to continue.");
		getch();
	}
	
	return 0;
}

int
nci_tools_search_next(struct nci* nci)
{
	int offset;
	
	if( nci->last_search_string == NULL )
	{
		nci_press_any_key(nci, "You haven't started a search yet.");
		
		return 0;
	}
	
	offset = diary_search(nci->diary, nci->last_search_type,
		nci->last_search_string, nci->last_search_offset + 1);
	
	if( offset < 0 )
	{
		/* NOTE: Should we notify the user when the search wraps? */
		offset = diary_search(nci->diary, nci->last_search_type,
			nci->last_search_string, 0);
	}
	
	if( offset < 0 )
	{
		nci_press_any_key(nci, "Not found.");
		
		return 0;
	}
	
	nci->selected_entry_offset = offset;
	nci->last_search_offset = offset;
	
	return 0;
}

int
nci_help_about(struct nci* nci)
{
	return nci_text_viewer(nci, CIRE_ABOUT_MESSAGE);
}

int
nci_sort(struct nci* nci)
{
	int i;
	char* title = NULL;
	int timestamp = 0;
	struct entry* entry;
	
	if(nci->sorting_set)
	{
		if(nci->selected_entry_offset >= 0)
		{
			entry = diary_get_entry_by_offset(nci->diary, nci->selected_entry_offset);
		
			timestamp = entry_get_time(entry);
			misc_str_assign(&title, entry_get_title(entry));
		}
	
		diary_sort(nci->diary, nci->sort_type, nci->sort_direction);
		
		if(nci->selected_entry_offset >= 0)
		{
			for(i = 0; i < diary_get_entry_count(nci->diary); ++i)
			{
				entry = diary_get_entry_by_offset(nci->diary, i);
			
				if( timestamp == entry_get_time(entry) &&
					!strcmp(title, entry_get_title(entry)) )
				{
					nci->selected_entry_offset = i;
					break;
				}
			}
		
			if( !(i < diary_get_entry_count(nci->diary)) )
				nci->selected_entry_offset = -1;
			
			free(title);
		}
	}
	
	return 0;
}

int
nci_update_screen(struct nci* nci)
{
	static int top = 0; /* upper most entry offset */
	int i;
	int j;
	struct entry* entry;
	char timestamp[48];
	
	nci_update_header(nci);
	nci_sort(nci);
	
	for(i = 1; i < LINES - 1; ++i)
	{
		move(i, 0);
		clrtoeol();
	}
	
	if( nci->selected_entry_offset < top && nci->selected_entry_offset > -1 )
		top = nci->selected_entry_offset;
		
	if( nci->selected_entry_offset >= top + (LINES - 4) )
		top = nci->selected_entry_offset - (LINES - 5);
		
	for(i = top; i < top + (LINES - 4) && i < diary_get_entry_count(nci->diary); ++i)
	{
		entry = diary_get_entry_by_offset(nci->diary, i);
		entry_get_timef(entry, timestamp, 48, prefs_get_timef());
		
		if(i == nci->selected_entry_offset)
		{
			attron(A_REVERSE);
			
			for(j = 4; j < 76; ++j)
				mvprintw(i - top + 2, j, " ");
		}
	
			mvprintw(i - top + 2, 4, "%s",
				misc_str_trunc(entry_get_title(entry), 68 - strlen(timestamp)) );
			
			mvprintw(i - top + 2, 76 - strlen(timestamp), "%s", timestamp);
	
		if(i == nci->selected_entry_offset)
		{
			attroff(A_REVERSE);
		}
	}
	
	return 0;
}

int
nci_update_header(struct nci* nci)
{
	int i;
	const char* filename;
	char* sort_status;
	
	if(nci->filename)
		filename = nci->filename;
	else
		filename = "Untitled";
		
	sort_status = malloc(31);
	strcpy(sort_status, "Sorting: ");
	
	if(nci->sorting_set)
	{
		if(nci->sort_type == sort_title)
			strcat(sort_status, "Title, ");
		else
			strcat(sort_status, "Timestamp, ");
		
		if(nci->sort_direction == sort_ascending)
			strcat(sort_status, "Ascending");
		else
			strcat(sort_status, "Descending");
	}
	else
		strcat(sort_status, "None, None");
	
	attron(A_REVERSE);
	
		for(i = 0; i < COLS; ++i)
			mvprintw(0, i, " ");
	
		mvprintw(0, 0, "  Cire - ");
		mvprintw(0, 9, "%s", filename);
		mvprintw(0, COLS / 2 - strlen(sort_status) / 2, "%s", sort_status);
	
		if(nci->modified)
			mvprintw(0, COLS - 10, "Modified");
	
	attroff(A_REVERSE);
	
	free(sort_status);
	return 0;
}

int
nci_print_footer(const char* footer)
{
	int i;
	
	attron(A_REVERSE);
	
	for(i = 0; i < COLS; ++i)
		mvprintw(LINES - 1, i, " ");
	
	mvprintw(LINES - 1, 2, "%s", footer);
	
	attroff(A_REVERSE);
	return 0;
}

const char*
nci_get_user_text(struct nci* nci, const char* prompt)
{
	int i;
	int j;
	int key;
	static char* buffer = NULL;
	int buffer_len;
		
	free(buffer);
	buffer_len = COLS - strlen(prompt) - 5;
	buffer = malloc(buffer_len);
	memset(buffer, '\0', buffer_len);
	
	curs_set(1);
	i = 0;
	while(TRUE)
	{
		nci_update_header(nci);
		
		attron(A_REVERSE);
			for(key = 0; key < COLS; ++key) /* borrow key var for a counter */
				mvprintw(LINES - 1, key, " ");
				
			mvprintw(LINES - 1, 2, "%s", prompt);
			mvprintw(LINES - 1, 2 + strlen(prompt), ": ");
		
			mvprintw(LINES - 1, 2 + strlen(prompt) + 2, "%s", buffer);
		attroff(A_REVERSE);

		key = getch();
		switch(key)
		{
			case KEY_BACKSPACE:
			case KEY_DC:
			case NCI_KEY_BACKSPACE:
				if(i > 0)
				{
					--i;
					buffer[i] = '\0';
				}
				else
					beep();
				
				break;
			
			case NCI_KEY_ENTER:
				curs_set(0);
				if(*buffer == '\0')
					return NULL;
				else
					return buffer;

			case NCI_KEY_ESCAPE:
				curs_set(0);
				return NULL;
				
			case KEY_RESIZE:
				for(j = 0; j < LINES - 4; ++j)
				{
					move(j + 2, 0);
					clrtoeol();
				}
				
				break;
				
			default:
				if(isprint(key) && i < buffer_len)
				{
					buffer[i] = key;
					++i;
					buffer[i] = '\0';
				}
				else
					beep();
		}
	}
}

int 
nci_tools_prefs(struct nci* nci)
{
	int i;
	int field = 0;
	int key;
	struct nci_options prefs[NPREFS];

	curs_set(1);
	
	for(i = 0; i < NPREFS; ++i)
	{
		prefs[i].buffer_size = COLS - 19;
		prefs[i].buffer = malloc(prefs[i].buffer_size);
	}
	
	prefs[EDITOR].name = "Editor";
	if( strlen(prefs_get_editor()) < prefs[EDITOR].buffer_size )
		strcpy(prefs[EDITOR].buffer, prefs_get_editor());
	else
		strcpy(prefs[EDITOR].buffer, "ERROR");
	
	prefs[CIRE_DIR].name = "Cire Directory";
	if( strlen(prefs_get_cire_dir()) < prefs[CIRE_DIR].buffer_size )
		strcpy(prefs[CIRE_DIR].buffer, prefs_get_cire_dir());
	else
		strcpy(prefs[CIRE_DIR].buffer, "ERROR");
	
	prefs[TIMEF].name = "Time Format";
	if( strlen(prefs_get_timef()) < prefs[TIMEF].buffer_size )
		strcpy(prefs[TIMEF].buffer, prefs_get_timef());
	else
		strcpy(prefs[TIMEF].buffer, "ERROR");

	while(TRUE)
	{
		nci_update_header(nci);
		nci_print_footer("|ENTER| save   |ESC| discard   |DOWN| or |TAB| next element   |UP| previous"); 
		
		for(i = 1; i < LINES - 1; ++i)
		{
			move(i, 0);
			clrtoeol();
		}
	
		for(i = 0; i < NPREFS; ++i)
		{
			mvprintw(2, 0, "    Preferences");
			mvprintw(4 + (i * 2), 18 - strlen(prefs[i].name), "%s", prefs[i].name);
			mvprintw(4 + (i * 2), 18, ": ");
			
			attron(WA_UNDERLINE); /* Let me know if this works for you.. using _ just in case */
				mvprintw(4 + (i * 2), 20, "%s", prefs[i].buffer);
			attroff(WA_UNDERLINE);
		}

		move( 4 + (field * 2), 20 + strlen(prefs[field].buffer) );
		
		switch( (key = getch()) )
		{
			case NCI_KEY_TAB:
			case KEY_DOWN:
				field++;
				if(field >= NPREFS)
					field = 0;
					
				break;
			
			case KEY_BTAB:
			case KEY_UP:
				field--;
				if(field < 0)
					field = NPREFS - 1;
				
				break;
			
			case KEY_BACKSPACE:
			case KEY_DC:
			case NCI_KEY_BACKSPACE:
				i = strlen(prefs[field].buffer);
			
				if( i > 0 )
					prefs[field].buffer[i - 1] = '\0';
				else
					beep();
			
				break;
			
			case NCI_KEY_ENTER:
				prefs_set_editor(prefs[EDITOR].buffer);
				prefs_set_cire_dir(prefs[CIRE_DIR].buffer);
				prefs_set_timef(prefs[TIMEF].buffer);
				
				goto prefs_all_done;
			
								/* i would sorta expect f1 to give me help */
			case KEY_F(1):      /* on what the options mean... */
			case NCI_KEY_ESCAPE:
				goto prefs_all_done;
			
			default:
				if( isprint(key) && strlen(prefs[field].buffer) < prefs[field].buffer_size )
				{
					i = strlen(prefs[field].buffer);
					
					prefs[field].buffer[i] = key;
					prefs[field].buffer[i + 1] = '\0';
				}
				else
					beep();
		}
	}
	
	prefs_all_done:
		for(i = 0; i < NPREFS; ++i)
			free(prefs[i].buffer);
		
		return 0;
}

int
nci_global_key_handler(struct nci* nci, int key)
{
	switch(key)
	{
		case 'j':
		case 'J':
		case KEY_DOWN:
		case KEY_RIGHT: nci_select_next(nci); return TRUE;
			
		case 'k':
		case 'K':
		case KEY_UP:
		case KEY_LEFT: nci_select_prev(nci); return TRUE;

		case NCI_KEY_ENTER: nci_entry_view(nci); return TRUE;
		
		case KEY_IC: nci_entry_add(nci); return TRUE;
		case KEY_DC: nci_entry_delete(nci); return TRUE;
		
		case NCI_KEY_HOME:
		case KEY_HOME:
			if(diary_get_entry_count(nci->diary) > 0)
				nci->selected_entry_offset = 0;
			else
				beep();
			 
			return TRUE;
		
		case NCI_KEY_END:
		case KEY_END:
			if(diary_get_entry_count(nci->diary) > 0)
				nci->selected_entry_offset = diary_get_entry_count(nci->diary) - 1;
			else
				beep();
				
			return TRUE;
		
		case KEY_PPAGE:
			if(diary_get_entry_count(nci->diary) > 0)
			{
				nci->selected_entry_offset -= (LINES - 5);
				
				if(nci->selected_entry_offset < 0)
					nci->selected_entry_offset = 0;
			}
			else
				beep();
			
			return TRUE;
		
		case KEY_NPAGE:
			if(diary_get_entry_count(nci->diary) > 0)
			{
				nci->selected_entry_offset += (LINES - 5);
				
				if(nci->selected_entry_offset > diary_get_entry_count(nci->diary))
					nci->selected_entry_offset = diary_get_entry_count(nci->diary) - 1;
			}
			else
				beep();
				
			return TRUE;
		
		default: return FALSE;
	}
}

int
nci_press_any_key(struct nci* nci, const char* message)
{
	char* footer;
	
	nci_update_screen(nci);
	
	footer = malloc(strlen(message) + 28);
	strcpy(footer, message);
	strcat(footer, " Press any key to continue.");
	
	nci_print_footer(footer);
	
	getch();
	free(footer);
	return 0;
}

#endif /* BUILD_NCURSES_UI */
