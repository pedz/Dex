
/* @(#)map.h	1.1 */

#include <sys/param.h>

typedef void m_type;
typedef unsigned int m_size;
typedef void *v_ptr;
typedef void *p_ptr;
typedef m_type (*m_func)(p_ptr, v_ptr, m_size, int);


m_type v_read(p_ptr buf, v_ptr addr, m_size size);
m_type v_write(p_ptr buf, v_ptr addr, m_size size);
m_type v_mkpage(v_ptr addr, m_func f);

/*
 * Note that we depend upon PAGESIZE and PGSHIFT from param.h
 */
#define PAGE_MASK (~(PAGESIZE - 1))

