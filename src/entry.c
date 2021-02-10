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
 * $Id: entry.c,v 1.9 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/


#include "main.h"

struct entry*
entry_new()
{
	struct entry* obj;

	obj = malloc( sizeof(struct entry) );

	obj->next = 0;
	obj->time = time(0);
	obj->title = 0;
	obj->contents = 0;

	return obj;
}

void
entry_free(struct entry* obj)
{
	free(obj->title);
	free(obj->contents);
	free(obj);
}

void
entry_set_time(struct entry* obj, int time)
{
	obj->time = time;
}

int
entry_get_time(struct entry* obj)
{
	return obj->time;
}

size_t
entry_get_timef(struct entry* obj, char* s, size_t max, const char* format)
{
	time_t time;
	struct tm time_tm;

	time = entry_get_time(obj);

	localtime_r(&time, &time_tm);

	return strftime(s, max, format, &time_tm);
}

const char*
entry_get_title(struct entry* obj)
{
	return obj->title;
}

int
entry_set_title(struct entry* obj, const char* title)
{
	free(obj->title);

	obj->title = malloc( strlen(title) + 1 );

	strcpy(obj->title, title);

	obj->time = time(0);

	return 0;
}

const char*
entry_get_contents(struct entry* obj)
{
	return obj->contents;
}

int
entry_set_contents(struct entry* obj, const char* contents)
{
	free(obj->contents);

	obj->contents = malloc( strlen(contents) + 1 );

	strcpy(obj->contents, contents);

	obj->time = time(0);

	return 0;
}
