# @(#)Makefile	1.11
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

VPATH		= ${TARGET_MACHINE}

PROGRAMS	= dex
ILIST		= dex
IDIR		= /usr/sbin/

DEBUG=1
FPRINTF=
CFLAGS		= -D_AIX41 -DDEBUG=${DEBUG} -DFPRINTF=${FPRINTF}
LIBS		= -lreadline -ll -lxcurses
#
# New system: we no longer try and find ourselves.  Rather we compile
# base.o with all of the debug and quick.c load this first off to
# define all the system structures we believe to be interesting.  This
# gives us some flexibility.
LDFLAGS		= -bloadmap:dex.map -L/usr/local/lib
base.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include

OFILES		= \
	asgn_expr.o \
	base.o \
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

asgn_expr.o : asgn_expr.c
asgn_expr.c : pre-asgn_expr.c
	${_CC_} -E ${_CCFLAGS_} ${pre-asgn_expr.c:P} | \
		/usr/ucb/indent -npro -st -bad -bap -br -cdb -i4 | /bin/sed -e '/^#/d' -e '/^$$/d' > $@

scan.o : gram.h
scan.o : scan.c
gram.o : gram.c
stab.o : stab.c
tree.o : gram.h

.include <${RULES_MK}>
