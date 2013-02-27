h49723
s 00020/00011/00113
d D 1.13 10/08/23 17:01:52 pedzan 13 12
e
s 00070/00019/00054
d D 1.12 02/08/02 09:17:22 pedz 12 11
c Lots of changes
e
s 00003/00012/00070
d D 1.11 02/03/14 16:12:30 pedz 11 10
e
s 00000/00000/00082
d D 1.10 00/09/18 17:56:57 pedz 10 9
c Checking before V5 conversion
e
s 00011/00005/00071
d D 1.9 00/05/29 15:09:06 pedz 9 8
c Just before IA64 conversion
e
s 00003/00002/00073
d D 1.8 00/02/08 17:36:27 pedz 8 7
e
s 00001/00000/00074
d D 1.7 98/10/23 12:26:06 pedz 7 6
e
s 00005/00003/00069
d D 1.6 97/05/13 16:01:43 pedz 6 5
c Check point
e
s 00001/00001/00071
d D 1.5 95/05/17 16:39:01 pedz 5 4
c Check point
e
s 00012/00002/00060
d D 1.4 95/04/25 10:09:50 pedz 4 3
e
s 00005/00001/00057
d D 1.3 95/02/09 21:35:34 pedz 3 2
e
s 00007/00005/00051
d D 1.2 95/02/01 10:36:41 pedz 2 1
e
s 00056/00000/00000
d D 1.1 94/08/22 17:56:32 pedz 1 0
c date and time created 94/08/22 17:56:32 by pedz
e
u
U
t
T
I 1
# %W%
#
# COMPONENT_NAME:
#
# FUNCTIONS:
#
# ORIGINS: 27 83
#
# IBM CONFIDENTIAL -- (IBM Confidential Restricted when
# combined with the aggregated modules for this product)
# OBJECT CODE ONLY SOURCE MATERIALS
# (C) COPYRIGHT International Business Machines Corp. 1988, 1994
# All Rights Reserved
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# LEVEL 1,5 Years Bull Confidential Information
#

D 12
VPATH		= ${TARGET_MACHINE}
E 12
I 12
X=${MAKETOP}../obj/power/bos/usr/ccs/lib/libxcurses
VPATH		= ${TARGET_MACHINE}:${X}
E 12

D 12
PROGRAMS	= dex
ILIST		= dex
E 12
I 12
D 13
# SUBDIRS		= lib
PROGRAMS	= \
	base \
E 13
I 13
SUBDIRS		= lib

PROGRAMS++	= \
E 13
	dex \
I 13
	dex64 \
E 13
	view-dump \
D 13
	xcoff-view
E 13
I 13
	view-dump64
E 13

D 13
ILIST		= ${PROGRAMS}
E 13
I 13
PROGRAMS	= \
	base \
	base64 \
	xcoff-view \
	xcoff-view64

ILIST		= ${PROGRAMS} ${PROGRAMS++}
E 13
E 12
IDIR		= /usr/sbin/

D 4
CFLAGS		= -D_KERNEL -g
E 4
I 4
D 9
# CFLAGS		= -D_KERNEL
D 6
CFLAGS		= 
E 4
LIBS		= -ll
E 6
I 6
D 8
CFLAGS		= -D_AIX41
E 8
I 8
# CFLAGS	= -D_AIX41
E 9
D 11
CFLAGS		= -D_AIX41 -DPRINTF=
E 11
I 11
D 12
DEBUG=1
FPRINTF=
CFLAGS		= -D_AIX41 -DDEBUG=${DEBUG} -DFPRINTF=${FPRINTF}
E 11
E 8
LIBS		= -lreadline -ll -lxcurses
E 6
D 4
LDFLAGS		= -bloadmap:dex.map
E 4
I 4
#
D 11
# The -H4096 is prevent the mmap ping pong problem when dex looks at
# itself to load up its internal symbols.  The STRIP_FLAG is so the
# symbols do not get stripped.  We also add in that dmap.o is compiled
# with the -g option so that it has all of the proc, thread, ppda, etc
# structures that are pulled in at startup.
#
D 6
LDFLAGS		= -bloadmap:dex.map -H4096
E 6
I 6
D 9
LDFLAGS		= -bloadmap:dex.map -H4096 -L/usr/local/lib
E 6
STRIP_FLAG	=
dmap.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
E 9
I 9
# LDFLAGS		= -bloadmap:dex.map -H4096 -L/usr/local/lib
# STRIP_FLAG	=
# dmap.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
#
E 11
# New system: we no longer try and find ourselves.  Rather we compile
# base.o with all of the debug and quick.c load this first off to
# define all the system structures we believe to be interesting.  This
# gives us some flexibility.
LDFLAGS		= -bloadmap:dex.map -L/usr/local/lib
base.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
E 9
I 6
scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include
E 6
E 4

OFILES		= \
E 12
I 12
dex_OFILES	= \
E 12
D 2
	gram.o \
E 2
	asgn_expr.o \
I 9
D 12
	base.o \
E 12
E 9
	base_expr.o \
I 3
	binary_expr.o \
E 3
I 2
	builtins.o \
I 3
	cast_expr.o \
E 3
	dex.o \
I 3
	disasm.o \
E 3
E 2
D 11
	dmap.o \
E 11
	fcall.o \
D 2
	dex.o \
	hmap.o \
E 2
I 2
	gram.o \
	load.o \
E 2
	map.o \
I 7
	print.o \
E 7
	scan.o \
	stab.o \
	stmt.o \
	sym.o \
D 2
	tree.o
E 2
I 2
	tree.o \
D 3
	tree_dump.o
E 3
I 3
	tree_dump.o \
	unary_expr.o
E 3
E 2

I 12
base_OFILES = base.o
view-dump_OFILES = view-dump.o
xcoff-view_OFILES = xcoff-view.o

D 13
PROGRAMS_LDT	= \
	base \
E 13
I 13
PROGRAMS++_LDT	= \
E 13
	dex64 \
D 13
	view-dump64 \
E 13
I 13
	view-dump64

PROGRAMS_LDT	= \
	base64 \
E 13
	xcoff-view64

base64_BUILD64		= TRUE
base64_OFILES		= ${base_OFILES:.o=.64o}
dex64_BUILD64		= TRUE
dex64_OFILES		= ${dex_OFILES:.o=.64o}
view-dump64_BUILD64	= TRUE
view-dump64_OFILES	= ${view-dump_OFILES:.o=.64o}
xcoff-view64_BUILD64	= TRUE
xcoff-view64_OFILES	= ${xcoff-view_OFILES:.o=.64o}

CFLAGS		= \
		  -qlist \
		  -qsource \
		  -qshowinc \
		  -qinfo=pro \
		  -D_AIX41
LIBFLAGS	= -L${X}
D 13
# LIBS		= lib/readline.a -ll -lxcurses
LIBS		= -ll -lxcurses
E 13
I 13
LIBS		= lib/readline.a -ll -lxcurses
# LIBS		= -ll -lxcurses
E 13

#
# New system: we no longer try and find ourselves.  Rather we compile
# base.o with all of the debug and quick.c load this first off to
# define all the system structures we believe to be interesting.  This
# gives us some flexibility.
LDFLAGS		= -bloadmap:dex.map

base.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra 
base.o_INCFLAGS = -I${MAKETOP}/../export/power/kernel/include
base.64o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
base.64o_INCFLAGS = -I${MAKETOP}/../export/power/kernel/include
D 13
#scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE
E 13
I 13
scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE
E 13
#
# xcurses for 64 bit seems broken today.
#
# scan.64o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include
E 12
asgn_expr.o : asgn_expr.c
I 12
asgn_expr.64o : asgn_expr.c

E 12
asgn_expr.c : pre-asgn_expr.c
	${_CC_} -E ${_CCFLAGS_} ${pre-asgn_expr.c:P} | \
D 2
		indent -st | sed -e '/^#/d' -e '/^$$/d' > $@
E 2
I 2
D 8
		/usr/ucb/indent -st | /bin/sed -e '/^#/d' -e '/^$$/d' > $@
E 8
I 8
D 12
		/usr/ucb/indent -npro -st -bad -bap -br -cdb -i4 | /bin/sed -e '/^#/d' -e '/^$$/d' > $@
E 12
I 12
		/usr/ucb/indent -npro -st -bad -bap -br -cdb -i4 | \
		/bin/sed -e '/^#/d' -e '/^$$/d' > $@
	sed -e 's%pre-asgn_expr.o%asgn_expr.c%g' pre-asgn_expr.u > tmp
	mv tmp pre-asgn_expr.u
E 12
E 8
E 2

scan.o : gram.h
I 6
scan.o : scan.c
E 6
gram.o : gram.c
D 5
gram.h : gram.y
E 5
I 5
stab.o : stab.c
E 5
tree.o : gram.h
I 12
load.o : stab.h

scan.64o : gram.h
scan.64o : scan.c
gram.64o : gram.c
stab.64o : stab.c
tree.64o : gram.h
load.64o : stab.h
E 12

.include <${RULES_MK}>
E 1
