
/* @(#)map.h	1.8 */

extern long h_base;
extern long h_high;

#define DUMP_BASE ((void *)0x30000000)
#define LOAD_BASE ((void *)0xD0000000)

/*
 * For reference purposes: An address like 0x00000123 in the kernel is
 * a "virtual" address and it is mapped to a "physical" address which
 * is actually accessible.
 */
#define v2f_type(type, vaddr) ((type)v2f((p_ptr)vaddr))
#define f2v_type(type, vaddr) ((type)f2v((v_ptr)vaddr))

/* The following typedef is system dependant */
typedef int jmp_type;
jmp_type *map_jmp_ptr;
void map_init(void);

typedef void *v_ptr;			/* A virtual address */
typedef void *p_ptr;			/* A physical address */

extern p_ptr v2f(v_ptr);
extern v_ptr f2v(p_ptr);

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

/* #define MAP_DEBUG */
