h37327
s 00015/00000/00000
d D 1.1 02/03/14 16:44:15 pedz 1 0
c date and time created 02/03/14 16:44:15 by pedz
e
u
U
t
T
I 1

/* %W% */

#ifndef __PRINT_H
#define __PRINT_H

void print_name(char *name, typeptr tptr);
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name);
void print_sym(symptr s);
void _dump_symtable(ns *nspace, symtabptr old_sytp);
void _dump_type(typeptr t);
void _dump_types(ns *nspace);

#endif /* __PRINT_H */
E 1
