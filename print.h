
/* @(#)print.h	1.1 */

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
