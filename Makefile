# @(#)Makefile	1.13
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

SUBDIRS		= lib

PROGRAMS	= \
	base \
	dex \
	view-dump \
	xcoff-view \
	consts.c

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
	play.o \
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

base_BUILD64		= TRUE
dex_BUILD64		= TRUE
view-dump_BUILD64	= TRUE
xcoff-view_BUILD64	= TRUE

C++FLAGS = -q64
CFLAGS		= \
		  -q64 \
		  -qlist \
		  -qsource \
		  -qshowinc \
		  -qinfo=pro \
		  -D_AIX41
LIBFLAGS	= -L${X}
dex_LIBS	= lib/readline.a -ll -lxcurses

#
# New system: we no longer try and find ourselves.  Rather we compile
# base.o with all of the debug and quick.c load this first off to
# define all the system structures we believe to be interesting.  This
# gives us some flexibility.
LDFLAGS		= -bloadmap:dex.map

# base.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g -qdbxextra
base.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -g
base.o_INCFLAGS = \
	-I${MAKETOP}../export/power_64/kernel/include \
	-I${MAKETOP}bos/kernext/sctp

vmsslb.h : ${MAKETOP}../../export/power_64/kernel/include/vmm/vmsslb.h
${MAKETOP}../../export/power_64/kernel/include/vmm/vmsslb.h :
	@echo MAKETOP=${MAKETOP}
	mkdir -p ${MAKETOP}../../export/power_64/kernel/include/vmm
	ln -s ${MAKETOP}../link/src/bos/kernel/vmm/vmsslb.h $@

play.o_INCFLAGS = \
	-I${MAKETOP}../export/power_64/kernel/include \
	-I${MAKETOP}bos/kernext/sctp
scan.o_CC_OPT_LEVEL = $(CC_OPT_LEVEL) -DUSE_READLINE -I/usr/local/include
asgn_expr.o : asgn_expr.c

asgn_expr.c : pre-asgn_expr.c
	${_CC_} -E ${_CCFLAGS_} ${pre-asgn_expr.c:P} | \
		/usr/ucb/indent -npro -st -bad -bap -br -cdb -i4 | \
		/bin/sed -e '/^#/d' -e '/^$$/d' > $@
	sed -e 's%pre-asgn_expr.o%asgn_expr.c%g' pre-asgn_expr.u > tmp
	mv tmp pre-asgn_expr.u

gram.o : gram.c
load.o : stab.h
scan.o : gram.h
scan.o : scan.c
stab.o : stab.c
tree.o : gram.h

consts.c : base
	base > consts.c

.include <${RULES_MK}>
