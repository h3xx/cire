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
 * File is a General Cire File. Anyone may commit changes.
 *
 * $Id: main.c,v 1.9 2004/11/24 02:54:44 andrewm1 Exp $
 *  ***************************************
*/

#include "main.h"

const char* cire_about_message = {
	"Cire 0.14.0 - A Diary Program Written in C\n"
	"Copyright (C) 2004 Andrew Morritt\n"
	"and Other Contributors.\n"
	"\n"
	"This program is free software; you can redistribute\n"
	"it and/or modify it under the terms of the GNU General\n"
	"Public License as published by the Free Software Foundation;\n"
	"either version 2 of the License, or (at your option)\n"
	"any later version.\n"
	"\n"
	"More Information is in the file COPYING.\n"
	"--------------------------------------------------\n"
	"Cire is a Free and Open-Source Program\n"
	"for keeping an Electronic Journal or Diary.\n"
	"It is designed to run on Linux and\n"
	"other Unixen.\n"
	"\n"
	"Some Possibilities include:\n"
	"*Keep a Personal Diary\n"
	"*Keep a Log of Progress on a Programming Project\n"
	"*Do Anything You Want!\n"
	"\n"
	"See the File README for Contact Details and Help."
};

char* errmsg = NULL;

int
main(int argc, char** argv)
{
	int i;

	for(i = 1; i < argc; ++i)
	{
		if( strcmp(argv[i], "--text") == 0 ||
				strcmp(argv[i], "-t") == 0 )
		{
			#ifdef BUILD_TEXT_UI
				return text_ui_main(argc, argv);
			#else
				printf("This binary was built without the Text interface.\n");
				return 1;
			#endif
		}

		if( strcmp(argv[i], "--ncurses") == 0 ||
				strcmp(argv[i], "-n") == 0 )
		{
			#ifdef BUILD_NCURSES_UI
				return ncurses_ui_main(argc, argv); /* call joe's main function */
			#else
				printf("This binary was built without the Ncurses interface.\n");
				return 1;
			#endif
		}

		if( strcmp(argv[i], "--gtk") == 0 ||
				strcmp(argv[i], "-g") == 0 )
		{
			#ifdef BUILD_GTK_UI
				return gtk_ui_main(argc, argv);
			#else
				printf("This binary was built without the Gtk interface.\n");
				return 1;
			#endif
		}

		if( strcmp(argv[i], "--help") == 0 ||
				strcmp(argv[i], "-h") == 0 )
		{
			printf("User interfaces available:\n\n");
			
			#ifdef BUILD_TEXT_UI
				printf("-t\t--text\t\tPlain text interface.\n");
			#endif
			
			#ifdef BUILD_NCURSES_UI
				printf("-n\t--ncurses\tNCurses interface.\n");
			#endif
			
			#ifdef BUILD_GTK_UI
				printf("-g\t--gtk\t\tGtk interface.\n\n");
			#endif
			
			#ifndef BUILD_TEXT_UI
				#ifndef BUILD_NCURSES_UI
					#ifndef BUILD_GTK_UI
						printf("Hrmm, you didn't include any user interfaces in this build.\n");
					#endif
				#endif
			#endif

			return 0;
		}
	}

	/* :-) */
	#ifdef BUILD_GTK_UI
		if(getenv("DISPLAY"))
			return gtk_ui_main(argc, argv);
		else
			
		#ifdef BUILD_NCURSES_UI
			return ncurses_ui_main(argc, argv);
		#else
			#ifdef BUILD_TEXT_UI
				return text_ui_main(argc, argv);
			#else
				printf("X windows is not running, and you did not include any ");
					printf("text based user interfaces in this build.\n");
			#endif
		#endif
	#else
		#ifdef BUILD_NCURSES_UI
			return ncurses_ui_main(argc, argv);
		#else
			#ifdef BUILD_TEXT_UI
				return text_ui_main(argc, argv);
			#else
				printf("Hrmm, you don't seem to have included any user interfaces in this build.\n");
				return 1;
			#endif
		#endif
	#endif

	return 0;
}
