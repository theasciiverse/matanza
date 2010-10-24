## Process this file with automake to produce Makefile.in
##
##      Copyright (C) 1999 Alejandro Forero Cuervo
##
##   This file is part of HB.
##
##   HB is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2, or
##   (at your option) any later version.
##
##   HB is distributed in the hope that it will be useful, but
##   WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with HB; see the file COPYING.  If not, write
##   to the Free Software Foundation, Inc., 59 Temple Place, Suite
##   330, Boston, MA 02111-1307 USA

bin_PROGRAMS = matanza matanza-ai

matanza_ai_SOURCES =    \
        ai.c            \
	getopt.h        \
	getopt.c        \
	getopt1.c

EXTRA_matanza_ai_SOURCES =      \
        mmp.h

matanza_SOURCES =	\
	matanza.c       \
	getopt.h        \
	getopt.c        \
	getopt1.c       \
        xmlparse.h xmlrole.h xmltok.h \
        xmlparse.c xmlrole.c xmltok.c \
        xmldef.h ascii.h asciitab.h iasciitab.h latin1tab.h nametab.h utf8tab.h

BUILT_SOURCES =		\
	matanza.c       \
        ai.c

EXTRA_matanza_SOURCES =	\
	shipreal.c      \
        mmp.h           \
        winconfig.h     \
        xmltok_impl.c   \
        xmltok_impl.h   \
        xmltok_ns.c

matanza.c:      matanza.w
	ctangle matanza.w

ai.c:		ai.w
	ctangle ai.w

EXTRA_DIST =		\
 	matanza.w       \
        ai.w            \
        TODO            \
        ship.txt        \
        shiptank.txt    \
        matanza.xml     \
        maze.png
