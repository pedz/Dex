
/* @(#)map.h	1.3 */

#define h_base ((v_ptr)0x40000000)
#define h_high ((v_ptr)(((long)(h_base)) + 0x10000000))

/*
 * For reference purposes: An address like 0x00000123 in the kernel is
 * a "virtual" address and it is mapped to a "physical" address which
 * is actually accessible.
 */
#define v2f(vaddr) ((v_ptr)(((long)(vaddr)) ^ 0x80000000))
#define f2v(paddr) ((p_ptr)(((long)(paddr)) ^ 0x80000000))
#define v2f_type(type, vaddr) ((type)v2f(vaddr))
#define f2v_type(type, vaddr) ((type)f2v(vaddr))

/* The following typedef is system dependant */
typedef int jmp_type;
jmp_type *map_jmp_ptr;
void map_init(void);

typedef void *v_ptr;			/* A virtual address */
typedef void *p_ptr;			/* A physical address */

#define BEGIN_PROTECT(faultp) \
{ \
    jmp_type *old; \
    jmp_buf jbuf; \
    old = map_jmp_ptr; \
    map_jmp_ptr = jbuf; \
    if ((*faultp = setjmp(map_jmp_ptr)) == 0) {

#define EXIT_PROTECT(thunk) \
	map_jmp_ptr = old; \
	thunk;
	
#define END_PROTECT() \
    } \
    map_jmp_ptr = old; \
}

