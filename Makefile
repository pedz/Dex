# @(#)Makefile	1.3
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

CFLAGS		= -D_KERNEL -g
LIBS		= -ll
LDFLAGS		= -bloadmap:dex.map

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
gram.o : gram.c
gram.h : gram.y
tree.o : gram.h

.include <${RULES_MK}>
