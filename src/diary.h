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
 * $Id: diary.h,v 1.24 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifndef __DIARY_H
#define __DIARY_H

struct diary
{
	struct entry* first_entry;
	int entry_count;
};

struct diary*
diary_new();
/* This allocates memory for a new diary object, with the
 * title not set and zero entries.
*/

void
diary_free(struct diary* obj);
/* This frees the memory used by a diary object. This includes
 * freeing the entries it contains.
*/

#define diary_is_empty(x) !(x)->first_entry
/* This is a macro to determine if a diary is empty.
 * x is a pointer to a diary struct. This will evaluate
 * to non-zero if the diary is empty or zero if it isn't.
*/

int
diary_get_entry_count(struct diary* obj);
/* This returns the number of entries contained by the diary.
*/

struct entry*
diary_get_entry_by_offset(struct diary* obj, int offset);
/* This takes a diary object and an offset. It return the address
 * of the entry at the specified offset. Basicly your asking for
 * entry number x of the diary. I only say offset because we start
 * counting at zero, not one.
*/

void
diary_add_entry(struct diary* obj, struct entry* new_entry);
/* Takes an entry and adds it to the list of entries in the diary.
*/

int
diary_remove_entry_by_offset(struct diary* obj, int offset);
/* This removes the entry at the specified offset. It returns
 * non-zero if the offset is invalid.
*/

int
diary_clear(struct diary* obj);
/* This clears a diary. All entries are removed.
*/

enum sort_type { sort_title, sort_timestamp };
enum sort_direction { sort_ascending, sort_descending };

int
diary_sort(struct diary* obj, enum sort_type type, enum sort_direction dir);
/* This sorts the entries contained by the diary. type is the
 * type of sort, i.e. what to sort based on and dir is the direction
 * to sort in. See the above enums for the possibilites.
 * Returns zero on success.
*/

int
diary_move_entry_at_offset_back(struct diary* obj, int offset);
/* This move the entry at offset back one place. It returns
 * zero if it succedes or non-zero if the offset is invalid.
*/

int
diary_load_from_memory(struct diary* obj, const char* data, const int len);
/* This parses *data as the contents of a diary file and loads
 * it into *obj. Returns 0 on succes.
*/

int
diary_save_to_memory(struct diary* obj, char** data, int* len);
/* This saves the contents of a diary into newly allocated memory,
 * the address of which is placed in *data and the size of which
 * in *len. Returns 0 on success.
*/

int
diary_load_from_file(struct diary* obj, const char* file, const char* key);
/* This loads the diary stored in *file into the specified file
 * into *obj, replacing the current contents. If key is non-null
 * the file's contents are unencrypted using *key before being
 * parsed. Returns 0 on success.
*/

int
diary_save_to_file(struct diary* obj, const char* file, const char* key);
/* This saves the specified diary to the specified file. If key
 * is non-null the data is encrypted before it is saved to the file.
 * 0 == success
*/

/* for compatability */
#define diary_export_html(diary, file)\
		diary_export_html_advanced((diary), (file), NULL, NULL)

int
diary_export_html_advanced(struct diary* obj, const char* file,
	const char* header, const char* footer);
/* This exports the diary in HTML format to *file.
 * If header is non-null that file will be used as a header.
 * If footer is non-null that file will be used as a footer.
 * Returns non-0 on error, 0 on success.
 */

int
diary_export_text(struct diary* obj, const char* file);
/* This exports the diary in plain text format to the
 * file specified. If *file does not end in .txt already
 * the extension will be (no longer be) added.
 * NOTE: the prefs module must alread be initialized.
 * Returns 0 for success or non-0 for error.
*/

int
diary_export_latex(struct diary* obj, const char* filename);
/* This exports the diary in TeX format to file specified.
 * Adds .tex according to the rules used by diary_export_text().
 */
 
enum search_type{ search_by_title, search_by_contents };

int
diary_search(struct diary* obj, enum search_type type,
				const char* needle, int starting_offset);
/* Starting with the entry at starting_offset, it searches
 * the diary for an entry containing *needle in (according
 * to type) either the title or the contents. Returns the
 * offset of the found entry, or -1 if none is found.
 */

#endif
