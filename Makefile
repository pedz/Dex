# @(#)Makefile	1.12
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

X=${MAKETOP}../obj/power/bos/usr/ccs/lib/libxcurses
VPATH		= ${TARGET_MACHINE}:${X}

# SUBDIRS		= lib
PROGRAMS	= \
	base \
	dex \
	view-dump \
	xcoff-view

ILIST		= ${PROGRAMS}
IDIR		= /usr/sbin/

dex_OFILES	= \
	asgn_expr.o \
	base_expr.o \
	binary_expr.o \
	builtins.o \
	cast_expr.o \
	dex.o \
	disasm.o \
	fcall.o \
	gram.o \
	load.o \
	map.o \
	print.o \
	scan.o \
	stab.o \
	stmt.o \
	sym.o \
	tree.o \
	tree_dump.o \
	unary_expr.o

base_OFILES = base.o
view-dump_OFILES = view-dump.o
xcoff-view_OFILES = xcoff-view.o

PROGRAMS_LDT	= \
	base \
	dex64 \
	view-dump64 \
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
# LIBS		= lib/readline.a -ll -lxcurses
LIBS		= -ll -lxcurses

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
#scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE
#
# xcurses for 64 bit seems broken today.
#
# scan.64o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include
asgn_expr.o : asgn_expr.c
asgn_expr.64o : asgn_expr.c

asgn_expr.c : pre-asgn_expr.c
	${_CC_} -E ${_CCFLAGS_} ${pre-asgn_expr.c:P} | \
		/usr/ucb/indent -npro -st -bad -bap -br -cdb -i4 | \
		/bin/sed -e '/^#/d' -e '/^$$/d' > $@
	sed -e 's%pre-asgn_expr.o%asgn_expr.c%g' pre-asgn_expr.u > tmp
	mv tmp pre-asgn_expr.u

scan.o : gram.h
scan.o : scan.c
gram.o : gram.c
stab.o : stab.c
tree.o : gram.h
load.o : stab.h

scan.64o : gram.h
scan.64o : scan.c
gram.64o : gram.c
stab.64o : stab.c
tree.64o : gram.h
load.64o : stab.h

.include <${RULES_MK}>
