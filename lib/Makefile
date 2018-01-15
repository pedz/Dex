LIBRARIES	= readline.a

# OFILES	= \
# bind.o complete.o display.o emacs_keymap.o funmap.o history.o
# isearch.o keymaps.o parens.o readline.o rltty.o search.o
# signals.o tilde.o vi_keymap.o vi_mode.o xmalloc.o getopt.o
# getopt1.o
# getopt_int.o

OFILES	= \
	bind.o \
	callback.o \
	colors.o \
	compat.o \
	complete.o \
	display.o \
	emacs_keymap.o \
	funmap.o \
	getopt.o \
	getopt1.o \
	getopt_init.o \
	histexpand.o \
	histfile.o \
	history.o \
	histsearch.o \
	input.o \
	isearch.o \
	keymaps.o \
	kill.o \
	macro.o \
	mbutil.o \
	misc.o \
	nls.o \
	parens.o \
	parse-colors.o \
	readline.o \
	rltty.o \
	savestring.o \
	search.o \
	shell.o \
	signals.o \
	terminal.o \
	text.o \
	tilde.o \
	undo.o \
	util.o \
	vi_keymap.o \
	vi_mode.o \
	xfree.o \
	xmalloc.o


CFLAGS	= \
	-q64 \
	-qcpluscmt \
	-DHAVE_DIRENT_H \
	-DHAVE_POSIX_SIGNALS \
	-DHAVE_PWD_H \
	-DHAVE_STDLIB_H \
	-DHAVE_STRING_H \
	-DHAVE_SYS_STREAM_H \
	-DHAVE_UNISTD_H \
	-DHAVE_VARARGS_H \
	-DPRAGMA_ALLOCA \
	-DVOID_SIGHANDLER

emacs_keymap.o_CFLAGS = ${CFLAGS} -DREADLINE_LIBRARY
vi_keymap.o_CFLAGS = ${CFLAGS} -DREADLINE_LIBRARY
tilde.o_CFLAGS = ${CFLAGS} -DREADLINE_LIBRARY

.include <${RULES_MK}>

