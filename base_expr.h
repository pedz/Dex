/* @(#)base_expr.h	1.3 */

unsigned int get_field(void *p, int offset, int size);
void set_field(void *p, int offset, int size, unsigned int val);
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name);
