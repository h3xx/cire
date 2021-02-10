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
 * $Id: misc.h,v 1.14 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#ifndef __MISC_H
#define __MISC_H

int
misc_str_remove_newlines(char* string);
/* This removes all the newlines from string. The empty
 * space is moved to the end of the string. If you pass
 * it "\nfoo\nbar\n\0" it will become "foobar\0xxx".
 * Returns zero on success or non-zero on error.
*/

int
misc_str_remove_first_and_last_newlines(char* string);
/* This removes the first and last newlines from string
 * if they exist. If the first or last char is not
 * a newline it won't be changed. "\nfoo\nbar\n\0" will
 * become "foo\nbar\0\0x" and "foo\nbar\0" won't be changed.
 * Returns zero on success or non-zero on error.
*/

const char*
misc_str_trunc(const char* str, int len);
/* If str is longer than len it is truncated and ... is
 * appended. The resulting string is returned as a statically
 * allocated const char* If len is an invalid size (less
 * than 3) an error message is printed and NULL returned.
*/

int
misc_crypt(char* data, const int len, const char* key);
/* *data ~ data to be either encrypted or decrypted
 * len ~ length of *data
 * *key ~ null terminated key to use to [en/de]crypt *data
 * If *data is encryped it is unencrypted, if it is unencrypted
 * it is encrypted.
*/

int
misc_crypt_skip_guessables(char* data, const int len, const char* key);
/* Just like misc_crypt() except it does not crypt any data
 * that a cracker could guess is there, such as tags,
 * timestamps and newlines. Returns non 0 on error.
*/

int
misc_file_is_encrypted(const char* filename);
/* Guesses whether *filename is encrypted based on the ratio of printing
 * to non-printing characters.
*/

int
misc_load_file(char** data, int* len, const char* filename);
/* This opens *filename for reading and loads the contents
 * into memory. The address of the memory allocated to store
 * the file is placed in *data. The amount (lenght) of memory
 * used is placed in *len. It is up to the calling function
 * to at some point free *data. Returns 0 on success. If
 * an error occurs non-zero is returned and non memory is
 * allocated. In this case, do *not* free *data.
 * Update 2004.03.28: It now allocates one more byte of
 * memory than necessary and sets the last byte to null.
*/

int
misc_save_file(const char* data, const int len, const char* filename);
/* This saves the contents of *data to *filename. len is the
 * size of *data. Returns 0 on success.
*/

int
misc_file_exists(const char* filename);
/* Returns >0 if *filename exists or 0 if it doesn't.
*/

const char*
misc_force_file_extension(const char* filename, const char* extension);
/* Appends *extension to *filename if *filename does not already end
 * in *extension. Returns a statically allocated const char* which is
 * guarenteed to end with *extension.
*/

const char*
misc_interpolate_filename(const char* filename);
/* Interpolates *filename and returns the result in a statically
 * allocated string. '~' is replaced with the actually path of
 * home. Returns 0 if things go bad.
 */

void
misc_str_assign(char** dest, const char* src);
/* Frees *dest if it's non-null, allocates enough memory to hold
 * *src then copies src into the newly allocated memory.
*/

int
misc_str_search(const char* haystack, const char* needle);
/* Searches *haystack for *needle. If it finds it, it returns it's
 * offset; if not, it returns -1. 2004.04.13: Is now case insensetive.
 */

char*
misc_str_to_upper(const char* str);
/* Returns newly allocated memory containing *str in all upper
 * case letters. If *str is null, returns null.
 */

#endif
