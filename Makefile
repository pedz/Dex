# @(#)Makefile	1.7
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

# CFLAGS		= -D_KERNEL
CFLAGS		= -D_AIX41
LIBS		= -lreadline -ll -lxcurses
#
# The -H4096 is prevent the mmap ping pong problem when dex looks at
# itself to load up its internal symbols.  The STRIP_FLAG is so the
# symbols do not get stripped.  We also add in that dmap.o is compiled
# with the -g option so that it has all of the proc, thread, ppda, etc
# structures that are pulled in at startup.
#
LDFLAGS		= -bloadmap:dex.map -H4096 -L/usr/local/lib
STRIP_FLAG	=
dmap.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include

OFILES		= \
	asgn_expr.o \
	base_expr.o \
	binary_expr.o \
	builtins.o \
	cast_expr.o \
	dex.o \
	disasm.o \
	dmap.o \
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
		/usr/ucb/indent -st | /bin/sed -e '/^#/d' -e '/^$$/d' > $@

scan.o : gram.h
scan.o : scan.c
gram.o : gram.c
stab.o : stab.c
tree.o : gram.h

.include <${RULES_MK}>
