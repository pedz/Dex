/* @(#)base_expr.h	1.4 */

ularge_t get_field(void *p, int offset, int size);
void set_field(void *p, int offset, int size, ularge_t val);
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name);
