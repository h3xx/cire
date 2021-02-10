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
 * $Id: misc.c,v 1.15 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#include "main.h"

int
misc_str_remove_newlines(char* string)
{
	int copy_i;
	int string_i;
	char* copy;

	/* assign some memory to create a temporary copy of the string */
	copy = malloc( strlen(string) + 1 );

	/* make the copy of the string */
	strcpy(copy, string);

	/* copy stuff back to string from copy, but only if it's not a newline */
	for(string_i = 0, copy_i = 0; copy_i < strlen(copy); ++copy_i)
	{
		if( copy[copy_i] != '\n' )
		{
			string[string_i] = copy[copy_i];
			++string_i;
		}
	}

	/* add a null to the end of string */
	string[string_i] = '\0';

	/* free the memory we used and return */
	free(copy);
	return 0;
}

int
misc_str_remove_first_and_last_newlines(char* string)
{
	int i;

	/* if the last char is a newline, replace with null */
	if( string[strlen(string) - 1] == '\n' )
		string[strlen(string) - 1] = '\0';

	/* if the first char is a newline, shift everything down by one */
	if( string[0] == '\n' )
	{
		for(i = 0; i < strlen(string); ++i)
			string[i] = string[i + 1];
	}

	return 0;
}

const char*
misc_str_trunc(const char* str, int len)
{
	char* return_str = 0;
	
	if(len < 3)
	{
		misc_str_assign(&errmsg, "Invalid length passed to misc_str_trunc().");

		return NULL;
	}
	
	free(return_str);
	
	return_str = malloc(strlen(str) + 1);
	
	strcpy(return_str, str);
	
	if(strlen(str) > len)
	{
		strcpy(return_str + len - 3, "...");
	}
	
	return return_str;
}

int
misc_crypt(char* data, const int len, const char* key)
{
	int i;
	int keylen = strlen(key);
	
	for(i = 0; i < len; ++i)
		data[i] ^= key[i % keylen];
	
	return 0;
}

int
misc_crypt_skip_guessables(char* data, const int len, const char* key)
{
	char* data_i;
	int keylen = strlen(key);
	
	for(data_i = data; (data_i - data) < len; ++data_i)
	{
		if( *data_i == '\n' ) { continue; }
		
		if( !strncmp(data_i, "<entry>",      7) ) { data_i +=  6; continue; }
		if( !strncmp(data_i, "</entry>",     8) ) { data_i +=  7; continue; }
		if( !strncmp(data_i, "<title>",      7) ) { data_i +=  6; continue; }
		if( !strncmp(data_i, "</title>",     8) ) { data_i +=  7; continue; }
		if( !strncmp(data_i, "<contents>",  10) ) { data_i +=  9; continue; }
		if( !strncmp(data_i, "</contents>", 11) ) { data_i += 10; continue; }
		
		if( !strncmp(data_i, "<timestamp>", 11) )
		{
			while(strncmp(data_i, "</timestamp>", 12) && (data_i - data) < len)
				++data_i;
			
			if( (data_i - data) < len )
			{
				data_i += 11;
				continue;
			}
			else
			{
				misc_str_assign(&errmsg, "File is corrupt: No matching </timestamp>.");

				return 1;
			}
		}
		
		*data_i ^= key[(data_i - data) % keylen];
	}
	
	return 0;
}

int
misc_file_is_encrypted(const char* filename)
{
	int i;
	char* file_data;
	int file_len;
	int printing_count = 0;
	int non_printing_count = 0;
	
	if( misc_load_file(&file_data, &file_len, filename) )
		return 0;
	
	for(i = 0; i < file_len; ++i)
	{
		if( isprint(file_data[i]) || isspace(file_data[i]) )
			++printing_count;
		else
			++non_printing_count;
	}
	
	if( non_printing_count == 0 || printing_count / non_printing_count < 5 )
		return 0;
	else
		return 1;
}

int
misc_load_file(char** data, int* len, const char* filename)
{
	int i;
	FILE* fd;
	
	if( (fd = fopen(filename, "r")) == 0 )
	{
		free(errmsg);
		errmsg = malloc(strlen(filename) + strlen(strerror(errno)) + 30);
		sprintf(errmsg, "Couldn't open %s for reading: %s.", filename, strerror(errno));

		return 1;
	}
	
	errno = 0;

	for(*len = 0;  fgetc(fd) != EOF; ++*len)
		;

	if(errno)
	{
		free(errmsg);
		errmsg = malloc(strlen(filename) + strlen(strerror(errno)) + 23);
		sprintf(errmsg, "Couldn't read from %s: %s.", filename, strerror(errno));

		fclose(fd);
		return 1;
	}

	*data = malloc( *len + 1);
	rewind(fd);

	for(i = 0; i < *len; ++i)
	{
		if( ((*data)[i] = fgetc(fd)) == EOF )
		{
			free(errmsg);
			errmsg = malloc(strlen(filename) + strlen(strerror(errno)) + 23);
			sprintf(errmsg, "Couldn't read from %s: %s.", filename, strerror(errno));

			free(*data);
			fclose(fd);
			return 1;
		}
	}
	
	(*data)[*len] = '\0';
	fclose(fd);
	return 0;
}

int
misc_save_file(const char* data, const int len, const char* filename)
{
	int i;
	FILE* fd;
	
	if( (fd = fopen(filename, "w")) == 0 )
	{
		free(errmsg);
		errmsg = malloc(strlen(filename) + strlen(strerror(errno)) + 30);
		sprintf(errmsg, "Couldn't open %s for writing: %s.", filename, strerror(errno));

		return 1;
	}

	for(i = 0; i < len; ++i)
	{
		if( fputc(data[i], fd) == EOF )
		{
			free(errmsg);
			errmsg = malloc(strlen(filename) + strlen(strerror(errno)) + 22);
			sprintf(errmsg, "Couldn't write to %s: %s.", filename, strerror(errno));
			
			fclose(fd);
			return 1;
		}
	}
	
	fclose(fd);
	return 0;
}

int
misc_file_exists(const char* filename)
{
	FILE* fd;
	
	fd = fopen(filename, "r");
	
	if(fd)
		fclose(fd);
	
	return (int) fd;
}

const char*
misc_force_file_extension(const char* filename, const char* extension)
{
	static char* filename_with_ext = NULL;
	
	free(filename_with_ext);
	filename_with_ext = malloc(strlen(filename) + strlen(extension) + 1);
	
	strcpy(filename_with_ext, filename);
	
	if(strcmp(filename + strlen(filename) - strlen(extension), extension))
	{
		strcat(filename_with_ext, extension);
	}
	
	return filename_with_ext;
}

const char*
misc_interpolate_filename(const char* filename)
{
	static char* returned = NULL;
	int len;
	int i;
	int tmp;
	
	free(returned);
	
	for(i = 0, len = 1; i < strlen(filename); ++i)
	{
		switch(filename[i])
		{
			case '~':
				len += strlen(getenv("HOME"));
				break;
			
			default:
				len += 1;
				break;
		}
	}
	
	returned = malloc(len);
	returned[0] = '\0';
	
	for(i = 0; i < strlen(filename); ++i)
	{
		switch(filename[i])
		{
			case '~':
				strcat(returned, getenv("HOME"));
				break;
			
			default:
				tmp = strlen(returned);
				returned[tmp] = filename[i];
				returned[tmp + 1] = '\0';
				break;
		}
	}
	
	return returned;
}

void
misc_str_assign(char** dest, const char* src)
{
	free(*dest);

	if(src)
	{
		*dest = malloc( strlen(src) + 1 );
		strcpy(*dest, src);
	}
	else
	{
		*dest = NULL;
	}
}

int
misc_str_search(const char* haystack, const char* needle)
{
	int i;
	int retval = -1;
	char* haystack_upper;
	char* needle_upper;
	
	haystack_upper = misc_str_to_upper(haystack);
	needle_upper = misc_str_to_upper(needle);
	
	for(i = 0; i <= strlen(haystack) - strlen(needle); ++i)
	{
		if( !strncmp(haystack_upper + i, needle_upper, strlen(needle)) )
		{
			retval = i;
			goto ret;
		}
	}
	
	ret:
		free(haystack_upper);
		free(needle_upper);
		return retval;
}

char*
misc_str_to_upper(const char* str)
{
	int i;
	int str_len;
	char* str_upper;
	
	if(!str) return NULL;
	
	str_len = strlen(str);
	str_upper = malloc(str_len + 1);
	
	for(i = 0; i < str_len; ++i)
		str_upper[i] = toupper(str[i]);
	
	str_upper[str_len] = '\0';
	
	return str_upper;
}
