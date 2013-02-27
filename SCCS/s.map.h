h13623
s 00011/00007/00134
d D 1.10 10/08/23 17:01:53 pedzan 10 9
e
s 00109/00014/00032
d D 1.9 02/03/14 16:12:33 pedz 9 8
e
s 00002/00002/00044
d D 1.8 00/09/18 17:57:01 pedz 8 7
c Checking before V5 conversion
e
s 00001/00001/00045
d D 1.7 00/05/29 15:09:08 pedz 7 6
c Just before IA64 conversion
e
s 00001/00000/00045
d D 1.6 00/02/08 17:36:48 pedz 6 5
e
s 00007/00011/00038
d D 1.5 98/10/23 12:26:11 pedz 5 4
e
s 00009/00001/00040
d D 1.4 97/05/13 16:02:58 pedz 4 3
c Check pointer
e
s 00002/00002/00039
d D 1.3 95/02/09 21:35:50 pedz 3 2
e
s 00033/00013/00008
d D 1.2 95/02/01 10:36:59 pedz 2 1
e
s 00021/00000/00000
d D 1.1 94/08/22 17:56:34 pedz 1 0
c date and time created 94/08/22 17:56:34 by pedz
e
u
U
t
T
I 1

/* %W% */

D 2
#include <sys/param.h>
E 2
I 2
D 4
#define h_base ((v_ptr)0x40000000)
E 4
I 4
D 8
#define h_base ((v_ptr)0x30000000)
E 4
#define h_high ((v_ptr)(((long)(h_base)) + 0x10000000))
E 8
I 8
D 9
extern long h_base;
extern long h_high;
E 9
I 9
#ifndef __MAP_H
#define __MAP_H
E 9
E 8
I 4

D 5
/*
 * Pseudo variables use f2v(hbase) which currently translates into
 * 0x90000000
 */
E 5
I 5
D 9
#define DUMP_BASE ((void *)0x30000000)
#define LOAD_BASE ((void *)0xD0000000)
E 9
I 9
#include <sys/param.h>
#include <setjmp.h>
E 9
E 5

I 9
#define THREAD_SLOT \
    ((thread_slot < 0 || thread_slot >= thread_max) ? -1 : thread_slot)

#ifdef __64BIT__

E 9
D 5
#define DUMP_BASE ((void *)0xC0000000)	/* vaddr 0x40000000 */
#define LOAD_BASE ((void *)0xD0000000)	/* vaddr 0x50000000 */
E 4
E 2

E 5
D 2
typedef void m_type;
typedef unsigned int m_size;
typedef void *v_ptr;
typedef void *p_ptr;
typedef m_type (*m_func)(p_ptr, v_ptr, m_size, int);
E 2
I 2
/*
D 9
 * For reference purposes: An address like 0x00000123 in the kernel is
 * a "virtual" address and it is mapped to a "physical" address which
 * is actually accessible.
E 9
I 9
 * Mapping in this case is four levels of 13 bits each.  Pointers are
 * 8 bytes so each stage is 2^16 bytes or 64K.
E 9
 */
D 5
#define v2f(vaddr) ((v_ptr)(((long)(vaddr)) ^ 0x80000000))
#define f2v(paddr) ((p_ptr)(((long)(paddr)) ^ 0x80000000))
#define v2f_type(type, vaddr) ((type)v2f(vaddr))
#define f2v_type(type, vaddr) ((type)f2v(vaddr))
E 5
I 5
D 9
#define v2f_type(type, vaddr) ((type)v2f((p_ptr)vaddr))
#define f2v_type(type, vaddr) ((type)f2v((v_ptr)vaddr))
E 9
E 5
E 2

I 9
#define STAGE_BITS 13
I 10
#define MAX_SHIFT_CNT 3
E 10

#define V2F_MACRO(p) \
    ((v_ptr)((page_table[THREAD_SLOT]-> \
D 10
	pte[(((long)(p)) >> PGSHIFT+(STAGE_BITS*3)) & STAGE_MASK]-> \
	pte[(((long)(p)) >> PGSHIFT+(STAGE_BITS*2)) & STAGE_MASK]-> \
	pte[(((long)(p)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK]-> \
	pte[(((long)(p)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK]) + \
     (((long)(p)) & (PAGESIZE - 1))))
E 10
I 10
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*3)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*2)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK]-> \
	pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK]) + \
     (((unsigned long)(p)) & (PAGESIZE - 1))))
E 10

#define INITIAL_STAGE STAGE0
I 10
#define INITIAL_STAGE_P1 STAGE1
E 10
#define USER_START 0x0000000110000000L
#define USER_END   0x000000011fffffffL

typedef struct stage0 initial_stage_t;

#else

/*
 * Mapping in this case is two levels of 10 bits each.  Pointers are 4
 * bytes for each stage is one page (4 * 1024)
 */

#define STAGE_BITS 10
I 10
#define MAX_SHIFT_CNT 1
E 10

#define V2F_MACRO(p) \
    ((p_ptr)((page_table[THREAD_SLOT]-> \
      pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK]-> \
      pte[(((unsigned long)(p)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK]) + \
     (((unsigned long)(p)) & (PAGESIZE - 1))))

#define INITIAL_STAGE STAGE2
I 10
#define INITIAL_STAGE_P1 FINAL_STAGE
E 10
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

E 9
I 2
/* The following typedef is system dependant */
I 9
#ifdef __MTYPE
typedef __MTYPE jmp_type;
#else
E 9
typedef int jmp_type;
I 9
#endif
E 9
jmp_type *map_jmp_ptr;
D 9
void map_init(void);
E 9
I 9
int map_init(void);
E 9
E 2

D 2
m_type v_read(p_ptr buf, v_ptr addr, m_size size);
m_type v_write(p_ptr buf, v_ptr addr, m_size size);
m_type v_mkpage(v_ptr addr, m_func f);
E 2
I 2
typedef void *v_ptr;			/* A virtual address */
typedef void *p_ptr;			/* A physical address */
I 5

D 9
extern p_ptr v2f(v_ptr);
extern v_ptr f2v(p_ptr);
E 9
I 9
D 10
p_ptr v2f(v_ptr);
v_ptr f2v(p_ptr);
E 10
I 10
extern p_ptr v2f(v_ptr);
extern v_ptr f2v(p_ptr);
E 10
int open_dump(char *path);
int purge_user_pages(void);
int purge_all_pages(void);
int return_range(char *t_name, char *d_name, void **startp, int *lenp);
void trace_mappings(void);
E 9
E 5
E 2

D 2
/*
 * Note that we depend upon PAGESIZE and PGSHIFT from param.h
 */
#define PAGE_MASK (~(PAGESIZE - 1))
E 2
I 2
D 3
#define BEGIN_PROTECT() \
E 3
I 3
#define BEGIN_PROTECT(faultp) \
E 3
{ \
    jmp_type *old; \
    jmp_buf jbuf; \
    old = map_jmp_ptr; \
    map_jmp_ptr = jbuf; \
D 3
    if (setjmp(map_jmp_ptr) == 0) {
E 3
I 3
D 9
    if ((*faultp = setjmp(map_jmp_ptr)) == 0) {
E 9
I 9
    DEBUG_PRINTF(("begin_protect: %s=>%s\n", \
                 P(old), P(map_jmp_ptr))); \
    if ((*faultp = setjmp(map_jmp_ptr)) == 0) { \
E 9
E 3

#define EXIT_PROTECT(thunk) \
	map_jmp_ptr = old; \
	thunk;
	
#define END_PROTECT() \
    } \
I 9
    DEBUG_PRINTF(("end_protect  : %s=>%s\n", \
                 P(map_jmp_ptr), P(old))); \
E 9
    map_jmp_ptr = old; \
}
E 2

I 6
D 7
#define MAP_DEBUG
E 7
I 7
D 9
/* #define MAP_DEBUG */
E 9
I 9
enum stages {
    STAGE0, STAGE1, STAGE2, FINAL_STAGE, PARTIAL_PAGE, REAL_STAGE
};

long get_addr2seg(long addr);

#endif /* __MAP_H */
E 9
E 7
E 6
E 1
