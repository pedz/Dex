
/* @(#)base_expr.h	1.6 */

#ifndef __BASE_EXPR_H
#define __BASE_EXPR_H

ularge_t get_field(void *p, int offset, int size);
void set_field(void *p, int offset, int size, ularge_t val);

#endif /* __BASE_EXPR_H */
