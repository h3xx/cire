#! /bin/sh
# ***************************************
# Cire: An Open-Source Diary Program in C
#
# Copyright (C) 2004, Andrew Morritt, <andymorritt@rogers.com>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at http://cire.sourceforge.net/license.php
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
# File is a General Cire File. Anyone may commit changes.
#
# $Id: autogen.sh,v 1.2 2004/10/14 15:19:06 andrewm1 Exp $
# ***************************************


echo "Running aclocal..."
	aclocal

echo "Running autoconf..."
	autoconf

echo "Running automake..."
	automake -ac
