dnl ***************************************
dnl Cire: An Open-Source Diary Program in C
dnl
dnl Copyright (C) 2004, Andrew Morritt, <andymorritt@rogers.com>, et al.
dnl
dnl This software is licensed as described in the file COPYING, which
dnl you should have received as part of this distribution. The terms
dnl are also available at http://cire.sourceforge.net/license.php
dnl
dnl You may opt to use, copy, modify, merge, publish, distribute and/or sell
dnl copies of the Software, and permit persons to whom the Software is
dnl furnished to do so, under the terms of the COPYING file.
dnl
dnl This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
dnl KIND, either express or implied.
dnl
dnl File is a General Cire File. Anyone may commit changes.
dnl
dnl $Id: configure.ac,v 1.4 2004/11/24 02:54:44 andrewm1 Exp $
dnl ***************************************

dnl Andrew: Change c.v.s to the version number when you do a release.

AC_INIT(cire, 0.14.0)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_SUBST(CFLAGS, "-O2 -Wall")


AC_ARG_WITH(
	gtk-ui,
	AC_HELP_STRING( [--with-gtk-ui], [build the gtk user interface (default=yes)] ),
	[
		if test $withval = "no"
		then
			build_gtk="false"
		else
			build_gtk="true"
		fi
	],
	[ build_gtk="true" ]
)

AC_ARG_WITH(
	ncurses-ui,
	AC_HELP_STRING( [--with-ncurses-ui], [build the ncurses user interface (default=yes)] ),
	[
		if test $withval = "no"
		then
			build_ncurses="false"
		else
			build_ncurses="true"
		fi
	],
	[ build_ncurses="true" ]
)

AC_ARG_WITH(
	text-ui,
	AC_HELP_STRING( [--with-text-ui], [build the plain text user interface (default=no)] ),
	[
		if test $withval = "no"
		then
			build_text="false"
		else
			build_text="true"
		fi
	],
	[ build_text="false" ]
)

AC_ARG_WITH(
	debugging,
	AC_HELP_STRING( [--with-debugging], [build with debug flags (default=no)] ),
	[
		if test $withval = "no"
		then
			build_debug="false"
		else
			build_debug="true"
		fi
	],
	[ build_debug="false" ]
)



if test $build_gtk = "true"
then
	PKG_CHECK_MODULES(
		GTK,
		gtk+-2.0 >= 2.0.2,
		[
			AC_DEFINE(BUILD_GTK_UI)
			LIBS="$LIBS $GTK_LIBS"
			CFLAGS="$CFLAGS $GTK_CFLAGS"
		],
		[ AC_MSG_WARN([Couldn't find GTK, building without it.]) ]
	)
fi

if test $build_ncurses = "true"
then
	AC_CHECK_HEADER(ncurses.h, [ found_ncurses_header="true" ])
	AC_CHECK_LIB(ncurses, getch, [ found_ncurses_library="true" ])

	if test $found_ncurses_header = "true" && $found_ncurses_library = "true"
	then
		AC_DEFINE(BUILD_NCURSES_UI)
		LIBS="-lncurses $LIBS"
	else
		AC_MSG_WARN([Couldn't find NCurses, building without it.])
	fi
fi

if test $build_text = "true"
then
	AC_DEFINE(BUILD_TEXT_UI)
fi


AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
