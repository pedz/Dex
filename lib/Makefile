LIBRARIES	= readline.a

# OFILES	= \
# bind.o complete.o display.o emacs_keymap.o funmap.o history.o
# isearch.o keymaps.o parens.o readline.o rltty.o search.o
# signals.o tilde.o vi_keymap.o vi_mode.o xmalloc.o getopt.o
# getopt1.o
# getopt_int.o

OFILES	= \
	bind.o \
	complete.o \
	display.o \
	emacs_keymap.o \
	funmap.o \
	getopt.o \
	getopt1.o \
	history.o \
	isearch.o \
	keymaps.o \
	parens.o \
	readline.o \
	rltty.o \
	search.o \
	signals.o \
	tilde.o \
	vi_keymap.o \
	vi_mode.o \
	xmalloc.o \


CFLAGS	= \
	-q64 \
	-DHAVE_UNISTD_H \
	-DHAVE_STDLIB_H \
	-DHAVE_VARARGS_H \
	-DHAVE_STRING_H \
	-DPRAGMA_ALLOCA \
	-DVOID_SIGHANDLER \
	-DHAVE_DIRENT_H \
	-DHAVE_SYS_STREAM_H

emacs_keymap.o_CFLAGS = ${CFLAGS} -DREADLINE_LIBRARY
vi_keymap.o_CFLAGS = ${CFLAGS} -DREADLINE_LIBRARY

.include <${RULES_MK}>
