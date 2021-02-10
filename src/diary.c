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
 * $Id: diary.c,v 1.31 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#include "main.h"

struct diary*
diary_new()
{
	struct diary* obj;

	obj = malloc( sizeof(struct diary) );

	obj->first_entry = 0;
	obj->entry_count = 0;

	return obj;
}

void
diary_free(struct diary* obj)
{
	diary_clear(obj);

	free(obj);
}

int
diary_get_entry_count(struct diary* obj)
{
	return obj->entry_count;
}

struct entry*
diary_get_entry_by_offset(struct diary* obj, int offset)
{
	int i;
	struct entry* entry;

	if( offset < 0 || offset > (diary_get_entry_count(obj) - 1) )
	{
		misc_str_assign(&errmsg, "Invalid entry offset.");
		return 0;
	}

	entry = obj->first_entry;

	for(i = 0; i < offset; ++i)
		entry = entry->next;

	return entry;
}

void
diary_add_entry(struct diary* obj, struct entry* new_entry)
{
	if(obj->first_entry)
	{
		diary_get_entry_by_offset(obj, diary_get_entry_count(obj) - 1)->next =
			new_entry;
	}
	else
	{
		obj->first_entry = new_entry;
	}

	new_entry->next = 0;
	
	++obj->entry_count;
}

int
diary_remove_entry_by_offset(struct diary* obj, int offset)
{
	struct entry* entry;

	if( (entry = diary_get_entry_by_offset(obj, offset)) == NULL )
		return 1;

	if(offset < 1)
	{
		obj->first_entry = entry->next;
	}
	else
	{
		diary_get_entry_by_offset(obj, offset - 1)->next = entry->next;
	}

	entry_free(entry);
	
	--obj->entry_count;

	return 0;
}

int
diary_clear(struct diary* obj)
{
	while( !diary_is_empty(obj) )
		diary_remove_entry_by_offset(obj, 0);
		
	return 0;
}

int
diary_sort(struct diary* obj, enum sort_type type, enum sort_direction dir)
{
	int i;
	int made_change;

	made_change = 1;
	while(made_change)
	{
		made_change = 0;

		for(i = 0; i < diary_get_entry_count(obj) - 1; ++i)
		{
			if(type == sort_title)
			{
				if(dir == sort_ascending)
				{
					if( strcmp( entry_get_title(diary_get_entry_by_offset(obj, i)),
								entry_get_title(diary_get_entry_by_offset(obj, i + 1)) )
							> 0)
					{
						diary_move_entry_at_offset_back(obj, i);
						made_change = 1;
					}
				}
				else
				{
					if( strcmp( entry_get_title(diary_get_entry_by_offset(obj, i)),
								entry_get_title(diary_get_entry_by_offset(obj, i + 1)) )
							< 0)
					{
						diary_move_entry_at_offset_back(obj, i);
						made_change = 1;
					}
				}
			}
			else if(type == sort_timestamp)
			{
				if(dir == sort_ascending)
				{
					if( entry_get_time(diary_get_entry_by_offset(obj, i)) >
						entry_get_time(diary_get_entry_by_offset(obj, i + 1)) )
					{
						diary_move_entry_at_offset_back(obj, i);
						made_change = 1;
					}
				}
				else
				{
					if( entry_get_time(diary_get_entry_by_offset(obj, i)) <
						entry_get_time(diary_get_entry_by_offset(obj, i + 1)) )
					{
						diary_move_entry_at_offset_back(obj, i);
						made_change = 1;
					}
				}
			}
			else
			{
				misc_str_assign(&errmsg, "Invalid sort type.");
				return 1;
			}
		}
	}

	return 0;
}

int
diary_move_entry_at_offset_back(struct diary* obj, int offset)
{
	struct entry* prev_entry;
	struct entry* entry;
	struct entry* next_entry;

	/* make sure it's a valid offset */
	if(offset < 0 || offset >= diary_get_entry_count(obj) - 1)
	{
		misc_str_assign(&errmsg, "Invalid entry offset.");
		return 1;
	}

	/* get the previous entry or set it to zero if we're moving the first */
	if(offset == 0)
		prev_entry = 0;
	else
		prev_entry = diary_get_entry_by_offset(obj, offset - 1);

	/* get the entry we're moving */
	entry = diary_get_entry_by_offset(obj, offset);

	/* get the next entry */
	next_entry = diary_get_entry_by_offset(obj, offset + 1);

	/* hook the entry to the one after the next_entry */
	entry->next = next_entry->next;

	/* hook the next_entry to the entry */
	next_entry->next = entry;

	/* hook the previous entry / head to the next_entry */
	if(prev_entry)
		prev_entry->next = next_entry;
	else
		obj->first_entry = next_entry;

	return 0;
}

int
diary_load_from_memory(struct diary* obj, const char* data, const int len)
{
	int i;
	int j;
	int k;
	struct entry* tmp_entry;
	char* tmp;
	int tmp_timestamp = 0; /* make gcc happy */
	char* endptr;

	for(i = 0; i < len; ++i)
	{
		if( strncmp(data + i, "<entry>", strlen("<entry>")) == 0 )
		{
			tmp_entry = entry_new();

			for(j = 0; (i + j) < len; ++j)
			{
				if( strncmp(data + i + j, "<title>", strlen("<title>")) == 0 )
				{
					for(k = 0; strncmp(data + i + j + k, "</title>",
									   strlen("</title>")); ++k)
					{
						if( (i + j + k) >= len )
						{
							misc_str_assign(&errmsg, "File is corrupt: No </title>.");

							entry_free(tmp_entry);
							return 1;
						}
					}

					tmp = malloc( k - strlen("<title>") + 1 );

					strncpy(tmp, data + i + j + strlen("<title>"),
							k - strlen("<title>"));
					tmp[k - strlen("<title>")] = '\0';

					misc_str_remove_newlines(tmp);

					entry_set_title(tmp_entry, tmp);

					free(tmp);
				}
				else if( strncmp(data + i + j, "<timestamp>",
							strlen("<timestamp>")) == 0 )
				{
					for(k = 0; strncmp(data + i + j + k, "</timestamp>",
							strlen("</timestamp>")); ++k)
					{
						if( (i + j + k) >= len )
						{
							misc_str_assign(&errmsg, "File is corrupt: No </timestamp>.");

							entry_free(tmp_entry);
							return 1;
						}
					}

					tmp = malloc( k - strlen("<timestamp>") + 1 );

					strncpy(tmp, data + i + j + strlen("<timestamp>"),
							k - strlen("<timestamp>"));
					tmp[k - strlen("<timestamp>")] = '\0';

					misc_str_remove_newlines(tmp);

					tmp_timestamp = strtol(tmp, &endptr, 10);

					if(tmp == endptr)
					{
						misc_str_assign(&errmsg, "File is corrupt: Bad timestamp.");

						free(tmp);
						entry_free(tmp_entry);
						return 1;
					}

					free(tmp);
				}
				else if( strncmp(data + i + j, "<contents>",
								 strlen("<contents>")) == 0 )
				{
					for(k = 0; strncmp(data + i + j + k, "</contents>",
									   strlen("</contents>")); ++k)
					{
						if( (i + j + k) >= len )
						{
							misc_str_assign(&errmsg, "File is corrupt: No </contents>.");

							entry_free(tmp_entry);
							return 1;
						}
					}

					tmp = malloc( k - strlen("<contents>") + 1 );

					strncpy(tmp, data + i + j + strlen("<contents>"),
							k - strlen("<contents>"));
					tmp[k - strlen("<contents>")] = '\0';

					misc_str_remove_first_and_last_newlines(tmp);

					entry_set_contents(tmp_entry, tmp);

					free(tmp);
				}
				else if( strncmp(data + i + j, "</entry>",
								 strlen("</entry>")) == 0 )
				{
					break;
				}
			}

			if( (i + j) >= len )
			{
				misc_str_assign(&errmsg, "File is corrupt: No </entry>.");

				entry_free(tmp_entry);
				return 1;
			}

			if( entry_get_title(tmp_entry) == NULL )
			{
				misc_str_assign(&errmsg, "File is corrupt: No title in entry.");

				entry_free(tmp_entry);
				return 1;
			}

			if( entry_get_contents(tmp_entry) == NULL )
			{
				misc_str_assign(&errmsg, "File is corrupt: No contents in entry.");

				entry_free(tmp_entry);
				return 1;
			}

			entry_set_time(tmp_entry, tmp_timestamp);
			/* we have to wait until here to call entry_set_time otherwise
			 * when we set the contents the time will get updated wiping
			 * out the timestamp we read from the file
			*/

			diary_add_entry(obj, tmp_entry);
		}
	}

	return 0;
}

int
diary_save_to_memory(struct diary* obj, char** data, int* len)
{
	int i;
	int entry_count = diary_get_entry_count(obj);
	struct entry* tmp_entry;
	char timestamp[20];
	
	*len = 0;
	
	/* figure out the size of the neccessary formating and add it to *len */
	*len += strlen("<entry>\n") * entry_count;
	
	*len += strlen("<title>\n") * entry_count;
	*len += strlen("\n") * entry_count;
	*len += strlen("</title>\n") * entry_count;
	*len += strlen("<timestamp>\n") * entry_count;
	*len += strlen("\n") * entry_count;
	*len += strlen("</timestamp>\n") * entry_count;
	*len += strlen("<contents>\n") * entry_count;
	*len += strlen("\n") * entry_count;
	*len += strlen("</contents>\n") * entry_count;
	*len += strlen("</entry>\n\n") * entry_count;
	
	/* figure out the size of the data of each entry and add it to *len */
	for(i = 0; i < entry_count; ++i)
	{
		tmp_entry = diary_get_entry_by_offset(obj, i);
		
		*len += sprintf( timestamp, "%d", entry_get_time(tmp_entry) );
		
		*len += strlen( entry_get_title(tmp_entry) );
		*len += strlen( entry_get_contents(tmp_entry) );
	}
	
	*data = malloc( *len + 1 );
	**data = '\0';
	
	for(i = 0; i < entry_count; ++i)
	{
		tmp_entry = diary_get_entry_by_offset(obj, i);
		
		sprintf( timestamp, "%d", entry_get_time(tmp_entry) );
		
		strcat(*data, "<entry>\n");
		strcat(*data, "<title>\n");
		strcat(*data, entry_get_title(tmp_entry)); strcat(*data, "\n");
		strcat(*data, "</title>\n");
		strcat(*data, "<timestamp>\n");
		strcat(*data, timestamp); strcat(*data, "\n");
		strcat(*data, "</timestamp>\n");
		strcat(*data, "<contents>\n");
		strcat(*data, entry_get_contents(tmp_entry)); strcat(*data, "\n");
		strcat(*data, "</contents>\n");
		strcat(*data, "</entry>\n\n");
	}
	
	return 0;
}

int
diary_load_from_file(struct diary* obj, const char* file, const char* key)
{
	char* file_data;
	int file_len;
	
	if( misc_load_file(&file_data, &file_len, file) )
		return 1;
	
	if(key)
		if( misc_crypt_skip_guessables(file_data, file_len, key) )
			return 1;
	
	if( diary_load_from_memory(obj, file_data, file_len) )
	{
		free(file_data);
		return 1;
	}
	
	free(file_data);
	return 0;
}

int
diary_save_to_file(struct diary* obj, const char* file, const char* key)
{
	char* file_data;
	int file_len;
	
	if( diary_save_to_memory(obj, &file_data, &file_len) )
		return 1;
	
	if(key)
		if( misc_crypt_skip_guessables(file_data, file_len, key) )
			return 1;
	
	if( misc_save_file(file_data, file_len, file) )
	{
		free(file_data);
		return 1;
	}
	
	free(file_data);
	return 0;
}

int
diary_export_html_advanced(struct diary* obj, const char* file,
	const char* header, const char* footer)
{
	int i;
	int j;
	int letter;
	FILE* out;
	FILE* head;
	FILE* foot;

	struct entry* entry;
	char timestamp[48];
	const char* contents;

	if(header)
	{
	    if( (head = fopen(header, "r")) == NULL )
		{
			free(errmsg);
			errmsg = malloc(strlen(header) + strlen(strerror(errno)) + 30);
			sprintf(errmsg, "Couldn't open %s for reading: %s.", header, strerror(errno));
        
			return 1;
		}
	}
	else
		head = NULL;
		
	if(footer)
	{		
		if( (foot = fopen(footer, "r")) == NULL )
		{
			free(errmsg);
			errmsg = malloc(strlen(footer) + strlen(strerror(errno)) + 30);
			sprintf(errmsg, "Couldn't open %s for reading: %s.", footer, strerror(errno));
      		
      		if(header) fclose(head);
			return 1;
		}
	}
	else
		foot = NULL;

	if( (out = fopen(file, "w")) == NULL )
	{
		free(errmsg);
		errmsg = malloc(strlen(file) + strlen(strerror(errno)) + 30);
		sprintf(errmsg, "Couldn't open %s for writing: %s.", file, strerror(errno));
		
		if(header) fclose(head);
		if(footer) fclose(foot);
		return 1;
	}

	/* loop through the header file and place it at the top of the output file */
	if(header)
	{
		while( (letter = getc(head)) != EOF)
			putc(letter, out);

		fclose(head);
	}
	else
	{
		/* print some stuff at the head of the file */
		fprintf(out, "<html>\n");
		fprintf(out, "<head>\n");
		fprintf(out, "\t<title>Diary exported from Cire</title>\n");
		fprintf(out, "</head>\n");
		fprintf(out, "<body>\n");
	}

	/* loop thru all the entry's and print them to the file */
	for(i = 0; i < diary_get_entry_count(obj); ++i)
	{
		entry = diary_get_entry_by_offset(obj, i);
		entry_get_timef(entry, timestamp, 48, prefs_get_timef());
		contents = entry_get_contents(entry);

		fprintf(out, "<p>\n");
		fprintf(out, "<h1>%s</h1>\n", entry_get_title(entry));
		fprintf(out, "<h2>%s</h2>\n", timestamp);

		/* loop thru the contents and use the correct html code if nessessary */
		for(j = 0; j < strlen(contents); ++j)
		{
			switch(contents[j]) /* is there anything else we should conver? */
			{
				case '\n': fprintf(out, "<br>\n"); break;
				
				default: fprintf(out, "%c", contents[j]);
			}
		}

		fprintf(out, "\n");
		fprintf(out, "</p>\n");
	}

	/* now copy the footer to the end of the output file */
	if(footer)
	{
		while( (letter = getc(foot)) != EOF )
			putc(letter, out);

		fclose(foot);
	}
	else
	{
		fprintf(out, "</body>\n");
		fprintf(out, "</html>\n");
	}

	fclose(out);
	return 0;
}

int
diary_export_text(struct diary* obj, const char* file)
{
	int i;
	FILE* fd;
	char timestamp[50];
	struct entry* entry;
	
	if( (fd = fopen(file, "w")) == NULL )
	{
		free(errmsg);
		errmsg = malloc(strlen(file) + strlen(strerror(errno)) + 30);
		sprintf(errmsg, "Couldn't open %s for writing: %s.", file, strerror(errno));
		
		return 1;
	}
	
	for(i = 0; i < diary_get_entry_count(obj); ++i)
	{
		entry = diary_get_entry_by_offset(obj, i);
		
		entry_get_timef(entry, timestamp, 50, prefs_get_timef());
		
		fprintf(fd, "===========================================\n");
		fprintf(fd, "# %s\n", entry_get_title(entry));
		fprintf(fd, "# Modified: %s\n", timestamp);
		fprintf(fd, "===========================================\n");
		fprintf(fd, "%s\n\n", entry_get_contents(entry));
	}
	
	fclose(fd);
	return 0;
}

int
diary_export_latex(struct diary* obj, const char* file)
{
	int i;
	int j;
	int count;
	int entry_len;
	const char * entry_contents;
	FILE* fd;
	char timestamp[48];
	struct entry* entry;
	
	if( (fd = fopen(file, "w")) == NULL )
	{
		free(errmsg);
		errmsg = malloc(strlen(file) + strlen(strerror(errno)) + 30);
		sprintf(errmsg, "Couldn't open %s for writing: %s.", file, strerror(errno));
		
		return 1;
	}

	count = diary_get_entry_count(obj);

	fprintf(fd, "\\documentclass[12pt]{article}\n\n");
	fprintf(fd, "\\begin{document}\n\n");
    
	for(i = 0; i < count; ++i)
	{
		entry = diary_get_entry_by_offset(obj, i);
		entry_get_timef(entry, timestamp, 48, prefs_get_timef());
		
		fprintf(fd, "\\section*{\\centerline{%s}}\n", entry_get_title(entry));
		fprintf(fd, "\\small{\\centerline{%s}}\n\n", timestamp);

		entry_len = strlen(entry_get_contents(entry));
		entry_contents = entry_get_contents(entry);
		
		for(j = 0; j < entry_len; ++j)
		{
		    switch(entry_contents[j])
		    {
			    case '#':
			    case '$':
			    case '%':
			    case '&':
			    case '_':
			    case '{':
			    case '}':
					fprintf(fd, "\\");
					fputc(entry_contents[j], fd);
					break;
					
				case '\\':
					fprintf(fd, "$\\backslash$");
					break;
					
				case '~':
				case '^':
					fprintf(fd, "\\verb=");
					fputc(entry_contents[j], fd);
					fprintf(fd, "=");
					break;

				default:
					fputc(entry_contents[j], fd);
			}
		}
	}

	fprintf(fd, "\\end{document}\n");
	
	fclose(fd);
	return 0;
}

int
diary_search(struct diary* obj, enum search_type type,
				const char* needle, int starting_offset)
{
	int i;
	const char* haystack;
	
	if(starting_offset >= 0)
		i = starting_offset;
	else
		i = 0;
	
	for( ; i < diary_get_entry_count(obj); ++i)
	{
		switch(type)
		{
			case search_by_title: case_title:
				haystack = entry_get_title(diary_get_entry_by_offset(obj, i));
				break;
			
			case search_by_contents:
				haystack = entry_get_contents(diary_get_entry_by_offset(obj, i));
				break;
			
			default:
				goto case_title;
		}
		
		
		if( misc_str_search(haystack, needle) >= 0 )
			return i;
	}
	
	return -1;
}
