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
 * $Id: entry.h,v 1.9 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#ifndef __ENTRY_H
#define __ENTRY_H

struct entry
{
	struct entry* next;

	time_t time; /* seconds since the epoch */
	char* title;
	char* contents;
};

/**** About time ;) ****
 *
 * The time of last modification (timestamp) of an entry is
 * stored in time_t time, in the form of seconds since the epoch.
 * It is automatically set to the curret time { time() } whenever
 * entry_new(), entry_set_title(), or entry_set_contents() are
 * called. I've written a wrapper function for strftime() called
 * entry_get_timef(). See the comment next to it's decleration for
 * details. When getting the time for use in the user interface
 * that is what should be used.
*/

struct entry*
entry_new();
/* This allocates memory for a new entry with the title and
 * contents unset and returns the address.
*/

void
entry_free(struct entry* obj);
/* This takes a pointer to an entry and frees the memory it uses.
 * It does not do anything to *next.
*/

void
entry_set_time(struct entry* obj, int time);
/* This sets the timestamp of the entry to int time
*/

int
entry_get_time(struct entry* obj);
/* This returns the timestamp of the entry in
 * the form of seconds since the epoch.
*/

size_t
entry_get_timef(struct entry* obj, char* s, size_t max, const char* format);
/* This takes an entry and the args used by strftime(). It calls strftime()
 * using the args provided and supplying the struct tm itself. It
 * returns what strftime() returns. It's basically just a wrapper.
*/

const char*
entry_get_title(struct entry* obj);
/* This returns a string containing the title of the entry if
 * it's title is set or returns NULL if it has not been set,
 * hence it can also be used to check whether the title is set.
 * The string it returns should be considered read-only, do not
 * modify or free it.
*/

int
entry_set_title(struct entry* obj, const char* title);
/* This sets the entry's title to be the same as the string passed
 * to it. The string can then be changed or freed, it is no longer
 * needed by the entry.
*/

const char*
entry_get_contents(struct entry* obj);
/* This works the same as entry_get_title but for the entries contents.
*/

int
entry_set_contents(struct entry* obj, const char* contents);
/* This works the same as entry_set_title but for the entries contents.
*/

#endif
