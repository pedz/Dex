
/* @(#)map.h	1.10 */

#ifndef __MAP_H
#define __MAP_H

#include <sys/param.h>
#include <setjmp.h>

#define THREAD_SLOT \
    ((thread_slot < 0 || thread_slot >= thread_max) ? -1 : thread_slot)

#ifdef __64BIT__

/*
 * Mapping in this case is four levels of 13 bits each.  Pointers are
 * 8 bytes so each stage is 2^16 bytes or 64K.
 */

#define STAGE_BITS 13
#define MAX_SHIFT_CNT 3

#define V2F_MACRO(p) \
    ((v_ptr)((page_table[THREAD_SLOT]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*3)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*2)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK]) + \
     (((unsigned long)(p)) & (PAGESIZE - 1))))

#define INITIAL_STAGE STAGE0
#define INITIAL_STAGE_P1 STAGE1
#define USER_START 0x0000000110000000L
#define USER_END   0x000000011fffffffL

typedef struct stage0 initial_stage_t;

#else

/*
 * Mapping in this case is two levels of 10 bits each.  Pointers are 4
 * bytes for each stage is one page (4 * 1024)
 */

#define STAGE_BITS 10
#define MAX_SHIFT_CNT 1

#define V2F_MACRO(p) \
    ((p_ptr)((page_table[THREAD_SLOT]-> \
      pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK]-> \
      pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK]) + \
     (((unsigned long)(p)) & (PAGESIZE - 1))))

#define INITIAL_STAGE STAGE2
#define INITIAL_STAGE_P1 FINAL_STAGE
#define USER_START 0x20000000
#define USER_END   0x28000000

typedef struct stage2 initial_stage_t;

#endif
#define V2F(p) v2f(p)
#define F2V(v) f2v(v)

#define STAGE_SIZE (1 << STAGE_BITS)
#define STAGE_MASK (STAGE_SIZE - 1)

struct stage0 {
    struct stage1 *pte[1 << STAGE_BITS];
};

struct stage1 {
    struct stage2 *pte[1 << STAGE_BITS];
};

struct stage2 {
    struct final_stage *pte[1 << STAGE_BITS];
};

struct final_stage {
    unsigned long pte[1 << STAGE_BITS];
};

initial_stage_t **page_table;

/*
 * map_top is the next virtual page that has not been "allocated" with
 * mmap.
 */
extern int thread_slot;
extern size_t thread_max;
extern long map_top;
extern int fromdump;

#define v2f_type(type, vaddr) ((type)V2F((p_ptr)vaddr))
#define f2v_type(type, vaddr) ((type)F2V((v_ptr)vaddr))

/* The following typedef is system dependant */
#ifdef __MTYPE
typedef __MTYPE jmp_type;
#else
typedef int jmp_type;
#endif
jmp_type *map_jmp_ptr;
int map_init(void);

typedef void *v_ptr;			/* A virtual address */
typedef void *p_ptr;			/* A physical address */

extern p_ptr v2f(v_ptr);
extern v_ptr f2v(p_ptr);
int open_dump(char *path);
int purge_user_pages(void);
int purge_all_pages(void);
int return_range(char *t_name, char *d_name, void **startp, int *lenp);
void trace_mappings(void);

#define BEGIN_PROTECT(faultp) \
{ \
    jmp_type *old; \
    jmp_buf jbuf; \
    old = map_jmp_ptr; \
    map_jmp_ptr = jbuf; \
    DEBUG_PRINTF(("begin_protect: %s=>%s\n", \
                 P(old), P(map_jmp_ptr))); \
    if ((*faultp = setjmp(map_jmp_ptr)) == 0) { \

#define EXIT_PROTECT(thunk) \
	map_jmp_ptr = old; \
	thunk;
	
#define END_PROTECT() \
    } \
    DEBUG_PRINTF(("end_protect  : %s=>%s\n", \
                 P(map_jmp_ptr), P(old))); \
    map_jmp_ptr = old; \
}

enum stages {
    STAGE0, STAGE1, STAGE2, FINAL_STAGE, PARTIAL_PAGE, REAL_STAGE
};

long get_addr2seg(long addr);

#endif /* __MAP_H */
