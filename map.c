static char sccs_id[] = "@(#)map.c	1.12";

#include <sys/param.h>
#include <sys/signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dump.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/seg.h>
#include <dbxstclass.h>
#include "map.h"
#include "dex.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "inter.h"
#include "fcall.h"

#define DEBUG_BIT MAP_C_BIT
/*
 * This is the third and hopefully final attempt at this.
 * 
 * Lets say that you want to look at the contents of 0x5080 which is
 * the ppda structure.  The description below assumes we are working
 * with a dump.  I am not sure how I will modify this to work on a
 * running system and it may not even be possible.
 * 
 * The 0x5080 address is called a virtual address.  It is not part of
 * dex's address space.  The task is to map this address into
 * something that is in dex's address space which is called the
 * physical address.  Note that what is called a physical address here
 * is actually a virtual address to the kernel.
 *
 * For a 32 bit system, the translation is done in two stages with
 * each stage using 10 bits of the virtual address (which gives 20
 * bits) and the bottom 12 bits are used as in index into the final
 * page entry.  For a 64 bit system, the translation is done in four
 * stages with each stage using 13 bits of virtual address and the
 * final low 12 bits are used as an index into the page.
 *
 * The result in a 32 bit system is a page (since each entry will be 4
 * bytes and there will be 1024 entries) for each translation stage.
 * For a 64 bit system, each translation stage will be 13 bits times 8
 * bytes per entry since it will use 64 bit pointers to the next stage
 * which will result in 64K for each stage.  Hopefully most of the
 * 2'nd, 3'rd, and 4'th level stages will not be needed.
 * 
 * The first level stages are pointed to by the "page_table" which is
 * indexed by thread_slot.  page_table is set up to have an entry for
 * slot -1 and entries up to the largest thread found in the dump.
 * This is one of two places where the dump reading code knows a
 * little bit about cdt names in the dump.  The dump is read through
 * twice for a number of reasons.  On the first pass, the largest
 * thread number is found.  Between passes t_map is allocated and
 * zeroed.  Then on the second pass, a 1 is set into t_map for each
 * thread found in the dump.  Later, when page_table is set up, if
 * t_map is set, a first level stage is allocated.  Otherwise, the
 * entry for that thread is set to point to the first level stage for
 * thread_slot -1.  thread_slot -1 is eventually set up to have
 * reasonable defaults.
 *
 * An anonymous, variable stage is allocated with mmap.  This will be
 * the first level stage for thread slot -1.  A variable called
 * map_top is set to point at the end of this region.  All other mmap
 * calls are fixed which implies they must be at the address
 * specified.  When the dump file is mapped, it is place where map_top
 * currently is and then map_top is move to the next page boundy past
 * the end of the dump.
 *
 * Other stages and areas are "allocated" by noting the current value
 * of map_top and then moving map_top by the size of the area.  The
 * pages are not actually mmap'ed until they are needed.  So, for
 * example, when page_table is initialized, first level stages are
 * allocated for each stage that was found in the dump but the space
 * for these stages are not mmap'ed yet and so consume no paging
 * space.  Because the initial stage for thread slot -1 was actually
 * mapped, it must be filled out and so eventually a call to
 * setup_thread is made with an argument of -1.  Also note that the
 * v2f translation does not use thread_slot directly.  Rather it uses
 * a macro which is equal to thread_slot if it is between 0 and
 * thread_max or -1 otherwise.  So page_table[THREAD_SLOT] always
 * points to a page that has been allocated.  When the code is working
 * correctly, all pte entries also point to pages that have at least
 * been allocated.
 *
 * no_page_start is a start of an area of memory the size of a stage
 * that is not mapped.  The top of the address resolution routine
 * (map_addr) checks for this case and returns 1 (unhappy) if the
 * fault address lies between no_page_start and no_page_end.
 *
 * With each allocation, an rmap entry is created.  The rmap variable
 * points to a array of rmap entries.  The array is realloc'ed if it
 * needs to grow.  Because map_top only increases, and a new rmap
 * entry is used each time, the physical address for the rmap entries
 * (r_phys) is kept sorted.  At the same time, under normal
 * circumstances, a hash table is kept of the rmap entries based upon
 * the virtual address, segment id, and type of stage.  This "normal
 * circumstances" will be explained shortly.  Note that there are rmap
 * entries created for each full page found in the dump.  I can not
 * recall the reason these entries are created.
 *
 * After the initialization code is complete, dex begins to run and
 * eventually will touch a page that has been allocated but not
 * actually mapped.  In the example above, the 0x5080 will start to go
 * through the "v2f" translation.  When the touch happens, a
 * segmentation violiation signal is given to dex which is trapped.
 * Along with the signal is the address that we are trying to touch.
 * The rmap structures are searched using a binary search.  The
 * address being touched should fall between r_phys and
 * r_phys+r_psize.  The rmap will tell us the type of stage.  The
 * stage is mapped and then filled out by calling the appropriate
 * setup_xxx routine.
 *
 * If the pte's for the stage each cover more than a segment, the call
 * is eventually routed to mult_seg_setup.  In this case, we are
 * worried about only three cases for each pte entry.  If the dump
 * does not have any addresses in the range that the pte covers, the
 * pte is set to point to no_page_start.  If all the pages in the dump
 * for the address range covered by the pte have the same segment
 * value, then the pte is set to an allocated map for that range and
 * segment id.  Note that all threads will be able to access this
 * range no matter what segment registers they have.  Also note that
 * since a specific segment value is used, the next thread hitting a
 * fault for the same range will find the rmap entry and so the lower
 * level stages will get reused.  Finally is the case where the pages
 * in the dump within the range the pte covers have more than one
 * segment value.  In this case, a seperate page is allocated.  The
 * negative of the thread slot is used as the segment id.  This will
 * force all threads to keep seperate entries and not use (and reuse)
 * each others.
 *
 * If the pte's for the stage cover exactly one page, then by this
 * point the segment value should already have been determine.  The
 * process is simply a matter of hooking up pointers to the pages in
 * the dump.  The one small extra part is if the dump does not have a
 * full page.  In this case, a page is allocated and mapped.  The rmap
 * entry for that page is set to PARTIAL_PAGE.  The dump is searched
 * and all pieces of the page that are in the dump (with the proper
 * segment value) are put into the page at the right offsets.
 * Whatever part is not in the dump is left as 0.
 *
 * The third stage setup routine called span_seg_setup is the most
 * wierd.  This is the case where the pte's each cover less than a
 * segment but the range of all of the pte's of the stage cover more
 * than a segment.  In the 32 bit case, this is the top level stage
 * (initial_stage).  In the 64 bit case, this is stage2 (starting from
 * 0).  It is during this stage that addr2seg is called.
 *
 * addr2seg: this routine starts out living in builtin.c but looks
 * like a pseudo-C code routine.  In practice the builtin routine
 * should never get called.  The pseudo-C code defines addr2seg.  The
 * beauty of this is that all of the data system structures are not
 * known by the real C code but are defined from debugging information
 * pulled in by an object file that is loaded.  (Currently this object
 * file come from base.c and is called base.o for 32 bit and base.64o
 * for 64 bit.)  addr2seg can then use pseudo-C code to dig around in
 * the kernel to find the value of the segment register for the
 * address that it is passed.
 *
 * The down fall with this is that addr2seg can page fault.  So, while
 * addr2seg is being to resolve a page fault, a second page fault can
 * happen.  This is span_seg_setup is "wierd" or hard to do.
 *
 * The way the above problem is solved is to first allocate (but not
 * map) pages for all of the pte's in the stage.  But this needs to be
 * done wisely.  If the pages in the dump for the range covered by the
 * pte all have the same segment value, then we can do like we did for
 * mult_seg_setup and allocate the map with the segment value found
 * for the pages in the dump.  Likewise if there are no pages in the
 * dump for the address range, then the pte can be set to
 * no_page_start.  In the other case, we allocate a page but set the
 * segment value to UNKNOWN.
 *
 * Then on a second pass through the pte's, if we have allocated one
 * of these temporary pages, we then call addr2seg to find out the
 * true segment value.  When addr2seg returns, we check to see if the
 * dump has pages in the range with the segment value.  If it does
 * not, we set the pte to no_page_start and we "free" the rmap entry.
 * If the dump does have a page, we set the segment value in the rmap
 * entry properly and also enter the rmap into hash table now that all
 * of the values are known.
 *
 * If addr2seg causes a page fault, the map_addr routine must notice
 * the special case of finding an rmap entry but the segment value
 * being set to UNKNOWN.  In this case, addr2seg is called (note that
 * this will now be the second concurrent call).  If more page faults
 * happen, we limit the recursion to 10.  Eventually, the second
 * addr2seg will return with a valid segment id.  We mark the
 * associated rmap entry as initialized and enter the rmap into the
 * hash table.  Because we are processing a page fault to fill out the
 * stage (up in map_addr), we follow the normal flow as if the rmap
 * had not had its segment value set to UNKNOWN which causes the next
 * lower stage to be set up.
 *
 * When the temporary rmaps are set up, we note in the rmap the index
 * of the parent rmap as well as the pte index for this rmap in the
 * parent.  (Note that the parent's pte's do not point to rmap
 * structures but to stages allocated but not mapped.  But for each of
 * these stages, there is an associate rmap entry.  Thus, by
 * induction, we can say the parent rmap entry points to this rmap
 * entry.)  When map_addr discovers an rmap with the segment value set
 * to UNKNOWN, it also uses the parent information to find all of the
 * other sibling rmap entries within the same segment.  The objective
 * here is to make at most one call to addr2seg for each segment for
 * each thread under the assumption that the call is relatively
 * expensive.  This also implies that in the second pass of the pte
 * enties that span_seg_setup makes, it needs to check and notice that
 * a "temporary" rmap entry has magically changed to initialized and
 * just skip over it.
 *
 * Above, the term "freed" is used for an rmap entry.  This process is
 * simply to set a bit in the rmap structure as being freed and also
 * setting an index called rmap_first_free to the index lowest freed
 * rmap entry.  When an rmap entry is allocated, the existing rmap are
 * scanned starting with rmap_first_free up to rmap_used looking for a
 * free rmap and in that case it is reused.  Remember though that
 * r_phys is already set up in this case.
 *
 * Additions:
 *
 * The r_initialized bit is kept off unstead of a special segment
 * value of UNKNOWN.
 *
 * We can add a bit like r_true_segval which will be set if addr2seg
 * is called.  In that case, in span_seg_setup, before addr2seg is
 * called, pte entries in the same segment can be scanned and if they
 * are initialized and true_segval is set, then we can not call
 * addr2seg.  Otherwise, we can not tell if the segment value is the
 * true segment value or if there happened to be only one segment
 * value for all the pages in the dump for that range.
 *
 * The "parent" stuff about keeping the rmap index and slot of the
 * parent rmap is not needed.  Rather, in span_seg_setup, we will
 * simply re-check the rmap entries to see if they are now initialized
 * (and also check and make sure that true_segval is set).
 *
 * Added r_in_addr2seg to catch and stop a recursive loop.
 */

struct rmap {
    unsigned long r_phys;		/* physical address in dex */
    unsigned long r_psize;		/* size of structure */
    /* */
    unsigned long r_virt;		/* virtual address */
    int r_thread;			/* thread this is for (0 => all) */
    enum stages r_stage;		/* what stage is this */
    /* */
    union {
	struct {			/* if initialized */
	    unsigned long _r_seg;	/* segment value */
	    struct rmap *_r_hash;	/* hash chain */
	} r_init;
	struct {			/* if not initialized */
	    int _r_parent;		/* rmap index of parent */
	    int _r_slot;		/* pte index that points to us */
	} r_pending;
    } r_dog;

    /* */
    uint r_mapped : 1;			/* true if actually mmaped */
    uint r_freed : 1;			/* true if freed */
    uint r_initialized : 1;		/* true if initilized */
    uint r_phys_set : 1;		/* true if r_phys/r_psize is valid */
    uint r_true_segval : 1;
    uint r_in_addr2seg : 1;
};

#define r_seg    r_dog.r_init._r_seg
#define r_hash   r_dog.r_init._r_hash
#define r_parent r_dog.r_pending._r_parent
#define r_slot   r_dog.r_pending._r_slot

struct dump_entry {
    unsigned int de_isreal : 1;
    unsigned short de_len;		/* length of entry */
    union {
	struct {
	    unsigned long dv_virt;	/* virtual address */
	    unsigned long dv_segval;	/* segment register */
	} de_virt;
	unsigned long long de_real;	/* real address */
    };
    void *de_dump;			/* offset into dump */
};

/* Local Prototypes */
p_ptr v2f(v_ptr v);
v_ptr f2v(p_ptr p);
static struct rmap *rmap_find(long l);
static void add_to_hash(struct rmap *r);
static int map_addr(long l);
#if 1 /* def __64BIT__ */
static void map_catch(int sig, siginfo_t *info, ucontext_t *context);
#else
static void map_catch(int sig, int code, struct sigcontext *scp);
#endif
int map_init(void);
static int xmap_compare(struct rmap *ra, struct rmap *rb);
static int rmap_hash(struct rmap *r);
static struct rmap *rmap_alloc(int size);
static void rmap_free(struct rmap *r);
static void rmap_fill(unsigned long phys,
		      unsigned long psize,
		      enum stages stage,
		      int thread,
		      unsigned long seg,
		      unsigned long virt);
static struct rmap *xmap_find(enum stages stage,
			      unsigned long seg,
			      unsigned long virt);
static long allocate_map(int thread,
			 enum stages stage,
			 unsigned long size,
			 unsigned long seg,
			 unsigned long virt);
static struct dump_entry *starting_d(unsigned long addr);
static int setup_thread(int thread);
static int setup_stage(struct stage0 *s,
		       int thread,
		       unsigned long segval,
		       unsigned long addr,
		       unsigned long skip,
		       enum stages stage);
static int mult_seg_setup(struct stage0 *s,
			  int thread,
			  unsigned long segval,
			  unsigned long addr,
			  unsigned long skip,
			  enum stages stage);
static int span_seg_setup(struct stage0 *s,
			  int thread,
			  unsigned long segval,
			  unsigned long addr,
			  unsigned long skip,
			  enum stages stage);
static int setup_final(struct final_stage *s,
		       int thread,
		       unsigned long segval,
		       unsigned long addr,
		       unsigned long skip,
		       enum stages stage);
static int setup_partial_page(unsigned long s,
			      int thread,
			      unsigned long segval,
			      unsigned long addr);
static int de_compare(void *a, void *b);
static int init_dump(void);
int return_range(char *t_name, char *d_name, void **startp, int *lenp);
int purge_all_pages(void);
int purge_user_pages(void);
int open_dump(char *path);
void trace_mappings(void);
long get_addr2seg(long addr);


/* Locals */
static struct rmap *rmap;
static struct rmap *(*rmap_hash_table)[];
static size_t rmap_first_free;		/* index of first free rmap */
static size_t rmap_used;		/* index of last used rmap */
static size_t rmap_max;			/* index of last rmap */
static int rmap_shift;			/* (1<<rmap_shift) == rmap_max */
static int dump_fd;
static char *dump_file;
static char *dump_file_end;
static struct dump_entry *dump_entries;
static int dump_entry_cnt;
static long no_page_start;
static long no_page_end;
static int quick_dump;
static long bos_segval;
static long bos_addr_start;
static long bos_addr_end;
static char *s_strings[] = {
    "stage0", "stage1", "stage2", "final_stage", "partial_page", "real_stage"
};

int thread_slot = -1;
size_t thread_max;			/* last thread in dump */
char *t_map;
long map_top;				/* last allocated virtual address */
int fromdump = 1;			/* always from a dump for now */
void * volatile last_v;
void * volatile last_f;

p_ptr v2f(v_ptr v)
{
    last_v = v;
    last_f = (void *)-1;
    return last_f = V2F_MACRO(v);
}

v_ptr f2v(p_ptr p)
{
    long l = (long)p;
    struct rmap *r = rmap_find(l);

    DEBUG_PRINTF(("f2v called with %s\n", P(p)));

    if (!r) {
	fflush(stdout);
	fprintf(stderr, "f2v for %s failed to find rmap\n", P(l));
	fail(1);
    }
    if (r->r_stage != REAL_STAGE && r->r_stage != PARTIAL_PAGE) {
	fflush(stdout);
	fprintf(stderr, "f2v for %s did not find real page\n", P(l));
	fprintf(stderr, "phys = %s\n", P(r->r_phys));
	fprintf(stderr, "psize = %s\n", P(r->r_psize));
	fprintf(stderr, "stage = %13s\n", s_strings[r->r_stage]);
	fprintf(stderr, "thread = %d\n", r->r_thread);
	fprintf(stderr, "seg = %s\n", P(r->r_seg));
	fprintf(stderr, "virt = %s\n", P(r->r_virt));
	fail(1);
    }
    return (v_ptr)(l - r->r_phys + r->r_virt);
}

static struct rmap *rmap_find(long l)
{
    int low = 0;
    int high = rmap_used - 2;
    int mid;
    int width;

    DEBUG_PRINTF(("rmap_find: %s\n", P(l)));
    if (debug_mask & DEBUG_BIT) {
	int temp = high;
	for (width = 1; temp > 10; ++width, temp /= 10);
	DEBUG_PRINTF(("%*s %*s %*s\n",
		      width, "low", width, "mid", width, "high"));
    }

    while (low <= high) {
	mid = (low + high) / 2;
	DEBUG_PRINTF(("%*d %*d %*d %s %s\n",
		      width, low, width, mid, width, high,
		      P(rmap[mid].r_phys), P(rmap[mid].r_psize)));
	if (l >= (rmap[mid].r_phys + rmap[mid].r_psize))
	    low = mid + 1;
	else
	    high = mid - 1;
    }

    DEBUG_PRINTF(("%*d %*d %*d %s %s\n",
		  width, low, width, mid, width, high,
		  P(rmap[low].r_phys), P(rmap[low].r_psize)));
    if (rmap[low].r_phys <= l && l < (rmap[low].r_phys + rmap[low].r_psize))
	return rmap + low;

    return 0;
}

static void add_to_hash(struct rmap *r)
{
    int hash = rmap_hash(r);
    r->r_hash = (*rmap_hash_table)[hash];
    (*rmap_hash_table)[hash] = r;
}

static int map_addr(long l)
{
    struct rmap *r;
    unsigned long skip;
    void *mmap_ret;
    int stage;
    int i;

    /*
     * If we hit the page that says "the page does not exist in the
     * dump" then we just return unhappy.
     */
    if (no_page_start <= l && l < no_page_end)
	return 1;

    if (!(r = rmap_find(l))) {
	fflush(stdout);
	fprintf(stderr, "map_addr: rmap_find failed for %s\n", P(l));
	fail(1);
    }

    /*
     * We can hit this condtion if we are poking into "user" space
     * that has not been allocated.  This can happen by mistake and so
     * we return as if the page is not mapped (because it isn't).
     */
    if (r->r_stage == REAL_STAGE)
	return 1;

    DEBUG_PRINTF(("map_addr: phys=%s size=%s thread=%d\n",
		  P(r->r_phys), P(r->r_psize), r->r_thread));
    DEBUG_PRINTF(("seg=%s virt=%s\n",
		  P(r->r_seg), P(r->r_virt)));

    if (!r->r_initialized) {
	long segval;
	int hash;

	if (r->r_in_addr2seg) {
	    printf("addr2seg recursed %d %s\n", r->r_thread, P(r->r_virt));
	    return 1;
	}
	r->r_in_addr2seg = 1;
	segval = get_addr2seg(r->r_virt);
	r->r_in_addr2seg = 0;
	r->r_initialized = 1;
	r->r_true_segval = 1;
	r->r_seg = segval;
	add_to_hash(r);
    }

    mmap_ret = mmap((void *)r->r_phys, r->r_psize, PROT_READ|PROT_WRITE,
		    MAP_FIXED|MAP_ANONYMOUS, -1, 0);
    DEBUG_PRINTF(("mmap_ret = 0x%s\n", P(mmap_ret)));
    if (mmap_ret == (void *)-1) {
	fflush(stdout);
	fprintf(stderr, "Failed to mmap stage %s for %s\n",
		s_strings[r->r_stage], P(r->r_phys));
	fail(1);
    }
    r->r_mapped = 1;

    switch (r->r_stage) {
    case STAGE0:
	return setup_thread(r->r_thread);

    case STAGE1:
	stage = r->r_stage + 1;
	skip = ((((unsigned long)-1) >> (STAGE_BITS * 2)) + 1);
	return setup_stage((struct stage0 *)r->r_phys, r->r_thread, r->r_seg,
			   r->r_virt, skip, stage);

    case STAGE2:
	if (INITIAL_STAGE == STAGE2)
	    return setup_thread(r->r_thread);
	stage = r->r_stage + 1;
	skip = ((((unsigned long)-1) >> (STAGE_BITS * 3)) + 1);
	return setup_stage((struct stage0 *)r->r_phys, r->r_thread, r->r_seg,
			   r->r_virt, skip, stage);

    case FINAL_STAGE:
	return setup_final((struct final_stage *)r->r_phys, r->r_thread,
			   r->r_seg, r->r_virt, PAGESIZE, REAL_STAGE);

    case REAL_STAGE:
	fflush(stdout);
	fprintf(stderr, "Faulted on REAL_STAGE %s\n", P(l));
	fail(1);

    case PARTIAL_PAGE:
	return setup_partial_page(r->r_phys, r->r_thread, r->r_seg, r->r_virt);
    }
    return 0;
}

#if 1 /* def __64BIT__ */
static void map_catch(int sig, siginfo_t *info, ucontext_t *context)
#else
static void map_catch(int sig, int code, struct sigcontext *scp)
#endif
{
    static volatile int count;
    sigset_t t;
    /*
     * The documentation says this is not correct but it seems to work for now.
     */
#if 1 /* def __64BIT__ */
    long paddr = (long)info->si_addr;

    {
	long *l = (long *)info;
	int i;

	DEBUG_PRINTF(("map_catch: info=0x%s context=0x%s\n",
		      P(info), P(context)));
	for (i = sizeof(siginfo_t)/sizeof(long); --i >= 0; ++l)
	    DEBUG_PRINTF(("map_catch: 0x%s: 0x%s\n",
			  P((long)l - (long)info), P(*l)));
	l = (long *)context;
	for (i = sizeof(ucontext_t)/sizeof(long); --i>= 0; ++l)
	    DEBUG_PRINTF(("map_catch: 0x%s: 0x%s\n",
			  P((long)l - (long)context), P(*l)));
	DEBUG_PRINTF(("map_catch: iar=0x%s\n",
		      P(context->uc_mcontext.jmp_context.iar)));
    }
#else
    long paddr = scp->sc_jmpbuf.jmp_context.except[0];
#endif

    DEBUG_PRINTF(("map_catch: paddr:0x%s\n", P(paddr)));

    if (++count >= 10) {
	fflush(stdout);
	fprintf(stderr, "Recursive SIGSEGV padd: %s\n", P(paddr));
	fail(1);
    }

    sigemptyset(&t);
    sigaddset(&t, SIGSEGV);
    if (sigprocmask(SIG_UNBLOCK, &t, (sigset_t *)0) < 0)
	perror("sigprocmask");
    
    if (!map_addr(paddr)) {
	--count;
	DEBUG_PRINTF(("map_catch: %s return happy\n", P(paddr)));
	return;
    }

    if (map_jmp_ptr) {
	--count;
	DEBUG_PRINTF(("map_catch: longjmp with paddr=%s to %s\n",
		      P(paddr), P(map_jmp_ptr)));
	longjmp(map_jmp_ptr, paddr);	/* WAS vaddr */
    }

    fflush(stdout);
    fprintf(stderr, "\nmap_catch: Can not map: paddr=%s\n", P(paddr));
    fail(1);
}

int map_init(void)
{
    struct sigaction s;
    long first_map;
    int i;
    struct rmap *r;
    

    s.sa_handler = (void (*)())map_catch;
    sigemptyset(&s.sa_mask);

#ifdef SA_SIGINFO
    s.sa_flags = SA_SIGINFO;
#else
    s.sa_flags = 0;
#endif

    if (sigaction(SIGSEGV, &s, (struct sigaction *)0) < 0) {
	perror("sigaction");
	return 1;
    }

    /*
     * Make reverse lookup for the program's data space.  There is an
     * assumption here that USER_START will be less than map_top.  If
     * it is not, then rmap_find will fail and we will need to regroup
     * and rething a few things.
     */
    rmap_fill(USER_START,
	      USER_END - USER_START,
	      REAL_STAGE,
	      0,
	      0,
	      USER_START);

    /*
     * This will eventually be used for the paging table of thread
     * slot -1 which is the page table used for any thread slot that
     * is not in the dump.  We must do this before calling init_dump
     * since init_dump uses map_top.
     */
    map_top = (long) mmap((void *)0,
			  sizeof(initial_stage_t),
			  PROT_READ|PROT_WRITE,
			  MAP_VARIABLE|MAP_ANONYMOUS,
			  -1,
			  0);
    if ((long)map_top == -1) {
	fprintf(stderr, "mmap of stage0 failed\n");
	return 1;
    }
    DEBUG_PRINTF(("map_init: map_top initialized to %s\n",
		  P(map_top)));
    first_map = map_top;		/* can not set page_table[-1] yet */
    map_top += sizeof(initial_stage_t);
    rmap_fill(first_map,
	      sizeof(initial_stage_t),
	      INITIAL_STAGE,
	      -1,
	      0,
	      0);
    r = rmap_find((long)first_map);
    r->r_mapped = 1;

    /*
     * init_dump discovers and sets thread_max.
     */
    if (init_dump())
	return 1;

    DEBUG_PRINTF(("thread_max=%d\n", thread_max));
    page_table = smalloc((size_t)(thread_max + 1) * sizeof(page_table[0]));
    ++page_table;			/* allow for -1 entry */
    page_table[-1] = (initial_stage_t *)first_map;

    /*
     * Make reverse lookup for initial stage entries.
     */
    for (i = 0; i < thread_max; ++i) {
	if (t_map[i]) {
	    page_table[i] = (initial_stage_t *) map_top;

	    rmap_fill(map_top,
		      sizeof(initial_stage_t),
		      INITIAL_STAGE,
		      i,
		      0,
		      0);

	    map_top += sizeof(initial_stage_t);
	} else
	    page_table[i] = (initial_stage_t *)first_map;
    }

    no_page_start = map_top;
    map_top += sizeof(initial_stage_t);
    no_page_end = map_top;
    DEBUG_PRINTF(("map_init: no_page_start = %s-%s\n",
		  P(no_page_start), P(no_page_end)));

    return setup_thread(-1);
}

static int xmap_compare(struct rmap *ra, struct rmap *rb)
{
    int diff;

    if (ra->r_stage != rb->r_stage)
	return (ra->r_stage > rb->r_stage) ? 1 : -1;
    if (ra->r_seg != rb->r_seg)
	return (ra->r_seg > rb->r_seg) ? 1 : -1;
    if (ra->r_virt != rb->r_virt)
	return (ra->r_virt > rb->r_virt) ? 1 : -1;
    return 0;
}

static int rmap_hash(struct rmap *r)
{
    unsigned long temp;
    int index;
    int bits;
    
    temp = (unsigned long)r->r_stage ^ (unsigned long)r->r_seg ^
	(unsigned long)r->r_virt;
    index = 0;
    for (bits = sizeof(long) * 8; bits > 0; bits -= rmap_shift) {
	index ^= (temp % rmap_max);
	temp >>= rmap_shift;
    }

    DEBUG_PRINTF(("rmap_hash: %s s=%s a=%s max=%d returning %d\n",
		  s_strings[r->r_stage], P(r->r_seg), P(r->r_virt),
		  rmap_max, index));
    return index;
}

/*
 * Because the rmap's can be reallocated, this routine returns an
 * index.  Index's should be used if they are going to be saved for
 * any length of time.
 *
 * If size is 0, we do not look for a free rmap.  Otherwise, we look
 * for a free rmap of the same size (which will always be the size of
 * a stage).  If we allocate a new rmap, we also modify map_top here
 * so that r_phys and r_psize are set when this routine returns
 */
static struct rmap *rmap_alloc(int size)
{
    struct rmap *r;

    if (size)
	while (rmap_first_free < rmap_used)
	    if ((r = rmap + rmap_first_free++)->r_freed) {
		if (r->r_psize != size) {
		    fprintf(stderr, "rmap_alloc: r_psize:%d != size:%d\n",
			    P(r->r_psize), P(size));
		    exit(1);
		}
		r->r_freed = 0;
		return r;
	    }

    if (rmap_used == rmap_max) {
	if (!rmap_max) {
	    rmap_shift = 12;
	    rmap_max = 1 << rmap_shift;

	    rmap = smalloc(rmap_max * sizeof(*rmap));
	    rmap_hash_table = smalloc(rmap_max*sizeof((*rmap_hash_table)[0]));
	} else {
	    int i;
	    size_t r_old = rmap_max * sizeof(*rmap);
	    size_t r_new = (rmap_max = (1 << ++rmap_shift)) * sizeof(*rmap);
	    size_t h_new = rmap_max * sizeof((*rmap_hash_table)[0]);

	    rmap = srealloc(rmap, r_new, r_old);
	    free(rmap_hash_table);
	    rmap_hash_table = smalloc(h_new);

	    for (i = 0; i < rmap_used; ++i)
		add_to_hash(rmap + i);
	    
	    DEBUG_PRINTF(("rmap's increased to %d\n", rmap_max));
	}
    }
    r = rmap + rmap_used++;
    rmap_first_free = rmap_used;

    if (size) {
	r->r_phys = map_top;
	r->r_psize = size;
	map_top += size;
	r->r_phys_set = 1;
    }
    return r;
}

static void rmap_free(struct rmap *r)
{
    int index = r - rmap;

    if (r->r_freed) {
	fflush(stdout);
	fprintf(stderr, "rmap_free: r_freed set for %s\n", P(r));
	exit(1);
    }
    if (r->r_mapped) {
	fflush(stdout);
	fprintf(stderr, "rmap_free: r_mapped set for %s\n", P(r));
	exit(1);
    }
    if (r->r_initialized) {
	fflush(stdout);
	fprintf(stderr, "rmap_free: r_initialized set for %s\n", P(r));
	exit(1);
    }

    r->r_freed = 1;
    if (rmap_first_free > index)
	rmap_first_free = index;
}

static void rmap_fill(unsigned long phys,
		      unsigned long psize,
		      enum stages stage,
		      int thread,
		      unsigned long seg,
		      unsigned long virt)
{
    struct rmap *rp;
    int hash;

    if (debug_mask & QUICK_RMAP_BIT)
	DEBUG_PRINTF(("rmap_fill: r=%s:%s %13s %4d\n"
		      "           v=%s seg=%s\n",
		      P(phys), P(psize), s_strings[stage], thread, P(virt),
		      P(seg)));

    rp = rmap_alloc(0);

    rp->r_phys = phys;
    rp->r_psize = psize;
    rp->r_phys_set = 1;

    rp->r_stage = stage;
    rp->r_thread = thread;
    rp->r_virt = virt;
    rp->r_mapped = stage == REAL_STAGE;

    rp->r_seg = seg;
    rp->r_initialized = 1;
    add_to_hash(rp);
}

static struct rmap *xmap_find(enum stages stage,
			      unsigned long seg,
			      unsigned long virt)
{
    struct rmap *rp;
    struct rmap r;
    int hash;

    r.r_stage = stage;
    r.r_seg = seg;
    r.r_virt = virt;
    hash = rmap_hash(&r);
    for (rp = (*rmap_hash_table)[hash];
	 rp && xmap_compare(&r, rp);
	 rp = rp->r_hash);
    DEBUG_PRINTF(("xmap_find: %s s=%s a=%s %sfound\n",
		  s_strings[stage], P(seg), P(virt),
		  rp ? "" : "not "));
    return rp;
}

static long allocate_map(int thread,
			 enum stages stage,
			 unsigned long size,
			 unsigned long seg,
			 unsigned long virt)
{
    long result;
    struct rmap *r = xmap_find(stage, seg, virt);

    if (r) {
	DEBUG_PRINTF(("allocate_map: reuse %s %s %s\n",
		      P(virt), P(seg), s_strings[stage]));
	return r->r_phys;
    }

    r = rmap_alloc(sizeof(initial_stage_t));
    r->r_stage = stage;
    r->r_thread = thread;
    r->r_virt = virt;
    r->r_mapped = stage == REAL_STAGE;

    r->r_initialized = 1;
    r->r_seg = seg;
    add_to_hash(r);

    return r->r_phys;
}

static struct dump_entry *starting_d(unsigned long addr)
{
    int low, high;

    low = 0;
    high = dump_entry_cnt - 2;
    while (low <= high) {
	int mid = (low + high) / 2;
	struct dump_entry *de = dump_entries + mid;

	if (!de->de_isreal && (addr > de->de_virt.dv_virt))
	    low = mid + 1;
	else
	    high = mid - 1;
    }
    return dump_entries + low;
}

#define IN_BOS_RANGE(addr, top) \
    ((bos_addr_start <= addr && addr < bos_addr_end) || \
     (bos_addr_start < top && top <= bos_addr_end) || \
     (addr <= bos_addr_start && bos_addr_end <= top))

#define IN_USER_RANGE(addr, top) \
    ((USER_START <= addr && addr < USER_END) || \
     (USER_START < top && top <= USER_END) || \
     (addr <= USER_START && USER_END <= top))

/*
 * This use to be in setup_thread a way back ago
 */
#if 0 /* removed while working on 64 bit stuff */
	/*
	 * #### This is a quick fix to the problem when a thread is
	 * not in kernel space and we switch to it, then segment
	 * register 0 can be something beside 0.  In that case, we can
	 * not get access to the normal kernel segment 0 and things
	 * fail rapidly.  So we force all threads to have segment
	 * register 0 set to 0.
	 *
	 * This may not be needed now with the first_segval and
	 * only_one_segval changes.
	 */
	if (!(addr & 0xf0000000))
	    segval = 0;
#endif

static int setup_thread(int thread)
{
    initial_stage_t *s = page_table[thread];
    unsigned long segval = -thread;
    unsigned long addr = 0;
    unsigned long skip = (((unsigned long)-1) >> STAGE_BITS) + 1;
    int stage = INITIAL_STAGE + 1;
    char *routine = "setup_thread";
    int mult_segs = skip > SEGSIZE;

    DEBUG_PRINTF(("%s: %s p=%s t=%d\n",
		  routine, s_strings[stage], P(s), thread));
    DEBUG_PRINTF(("%s: s=%s a=%s k=%s\n",
		  routine, P(segval), P(addr), P(skip)));

    if (mult_segs)
	return mult_seg_setup((struct stage0 *)s, thread, segval,
			      addr, skip, stage);
    if (((unsigned long long)skip << STAGE_BITS) > SEGSIZE)
	return span_seg_setup((struct stage0 *)s, thread, segval,
			      addr, skip, stage);
    fflush(stdout);
    fprintf(stderr, "%s: confused\n", routine);
    return 0;
}

static int setup_stage(struct stage0 *s,
		       int thread,
		       unsigned long segval,
		       unsigned long addr,
		       unsigned long skip,
		       enum stages stage)
{
    char *routine = "setup_stage";
    int mult_segs = skip > SEGSIZE;

    DEBUG_PRINTF(("%s: %s p=%s t=%d\n",
		  routine, s_strings[stage], P(s), thread));
    DEBUG_PRINTF(("%s: s=%s a=%s k=%s\n",
		  routine, P(segval), P(addr), P(skip)));

    if (mult_segs)
	return mult_seg_setup((struct stage0 *)s, thread, segval,
			      addr, skip, stage);
    if (((unsigned long long)skip << STAGE_BITS) > SEGSIZE)
	return span_seg_setup((struct stage0 *)s, thread, segval,
			      addr, skip, stage);
    fflush(stdout);
    fprintf(stderr, "%s: confused\n", routine);
    return 0;
}

/*
 * Each pte in these stages span multiple segments.
 */
static int mult_seg_setup(struct stage0 *s,
			  int thread,
			  unsigned long segval,
			  unsigned long addr,
			  unsigned long skip,
			  enum stages stage)
{
    char *routine = "mult_seg_setup";
    unsigned long top;
    int i;

    for (i = 0; i < STAGE_SIZE; ++i)
	s->pte[i] = (void *)no_page_start;

    for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	struct dump_entry *first_d, *d;
	int one_hit = 0;

	top = addr + skip;

	if (IN_USER_RANGE(addr, top)) {
	    s->pte[i] = (void *)allocate_map(thread, stage, skip, 0, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

	if (IN_BOS_RANGE(addr, top)) {
	    s->pte[i] = (void *)allocate_map(thread, stage, skip,
					     bos_segval, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s bos\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

	first_d = d = starting_d(addr);

	DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
		      routine, P(addr), P(top), P(segval)));
	DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
		      strlen(routine), "found",
		      P(d->de_virt.dv_virt),
		      P(d->de_virt.dv_virt+d->de_len),
		      P(d->de_virt.dv_segval)));

	/*
	 * The for loop makes sure that d points to a valid entry,
	 * it is not a real segment, and that addr <= dv_virt < top.
	 * I will need to hook up support for real segments someday
	 * probably to make the vmm people happy.
	 */
	for (; (d < (dump_entries + dump_entry_cnt) &&
		!d->de_isreal &&
		d->de_virt.dv_virt < top); ++d) {

	    /* Flag that says we have at least one page in this range */
	    one_hit = 1;

	    /*
	     * If we have the same segval, just continue.  Otherwise,
	     * we set up a map using the segval passed in and we are
	     * done.
	     */
	    if (first_d->de_virt.dv_segval == d->de_virt.dv_segval ||
		d->de_virt.dv_segval == 0)
		continue;
		    
	    s->pte[i] = (void *)allocate_map(thread,
					     stage,
					     skip,
					     segval,
					     addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s multi\n",
			  routine, P(s), i, P(s->pte[i])));
	    goto cont_loop;
	}

	/*
	 * If we found at least one entry and we get here, all the
	 * entries in the range must have the same segval so we
	 * allocate a map using its segval.
	 */
	if (one_hit) {
	    s->pte[i] =  (void *)allocate_map(thread,
					      stage,
					      skip,
					      first_d->de_virt.dv_segval,
					      addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s single\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

    cont_loop: ;

    }
    DEBUG_PRINTF(("%s: return 0\n", routine));
    return 0;
}

/*
 * Each pte in these stages are within a segment but the entire stage
 * spans more than one segment.
 */
static int span_seg_setup(struct stage0 *s,
			  int thread,
			  unsigned long segval,
			  unsigned long addr,
			  unsigned long skip,
			  enum stages stage)
{
    char *routine = "span_seg_setup";
    unsigned long addr_save = addr;
    unsigned long segval_addr;
    unsigned long top;
    struct rmap *rmaps[STAGE_SIZE];
    int pass;
    int i;

    for (pass = 0; pass < 2; ++pass) {
	addr = addr_save;

	for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	    struct dump_entry *first_d, *d;
	    int one_hit = 0;
	    struct rmap *r;

	    top = addr + skip;

	    if (IN_USER_RANGE(addr, top)) {
		if (pass == 0) {
		    s->pte[i] = (void *)allocate_map(thread, stage,
						     skip, 0,
						     addr);
		    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
				  routine, P(s), i, P(s->pte[i])));
		}
		continue;
	    }

	    if (IN_BOS_RANGE(addr, top)) {
		if (pass == 0) {
		    s->pte[i] = (void *)allocate_map(thread, stage, skip,
						     bos_segval, addr);
		    DEBUG_PRINTF(("%s: %s[%d]=%s bos\n",
				  routine, P(s), i, P(s->pte[i])));
		}
		continue;
	    }

	    first_d = d = starting_d(addr);

	    DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
			  routine, P(addr), P(top), P(segval)));
	    DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
			  strlen(routine), "found",
			  P(d->de_virt.dv_virt),
			  P(d->de_virt.dv_virt+d->de_len),
			  P(d->de_virt.dv_segval)));

	    for (; (d < (dump_entries + dump_entry_cnt) &&
		    !d->de_isreal &&
		    d->de_virt.dv_virt < top); ++d) {
		one_hit = 1;

		if (first_d) {
		    if (first_d->de_virt.dv_segval == d->de_virt.dv_segval ||
			d->de_virt.dv_segval == 0)
			continue;
		    
		    /*
		     * In the first pass at this point, we know that
		     * this pte is not going to be a simple case of a
		     * single segment id or no page at all.  So we
		     * create an rmap entry without the segment
		     * register and the initialized bit is off.
		     */
		    if (pass == 0) {
			r = rmap_alloc(sizeof(initial_stage_t));
			r->r_stage = stage;
			r->r_thread = thread;
			r->r_virt = addr;
			r->r_mapped = stage == REAL_STAGE;

			s->pte[i] = (struct stage1 *)r->r_phys;
			DEBUG_PRINTF(("%s: %s[%d]=%s\n",
				      routine, P(s), i, P(s->pte[i])));
			goto cont_loop;
		    }

		    /*
		     * In the second pass, we check to see if is the
		     * first time we get here for this segment.  If it
		     * is, we need to determine if another rmap entry
		     * already has a valid segment id and use
		     * it. Otherwise, we call addr2seg.
		     */
		    if (segval == -thread ||
			(segval_addr / SEGSIZE) != (addr / SEGSIZE)) {
			long addr_tmp;
			int found = 0;
			long j;

			/*
			 * Walk up the pte's within the same segment
			 * looking for one that has r_true_segval set.
			 */
			for (j = i+1, addr_tmp = addr + skip;
			     (addr / SEGSIZE) == (addr_tmp / SEGSIZE);
			     addr_tmp += addr, ++j) {

			    if (s->pte[j] == (struct stage1 *)no_page_start)
				continue;

			    if (!(r = rmap_find((long)s->pte[j]))) {
				fflush(stdout);
				fprintf(stderr, "%s: rmap_find(1) failed "
					"for %s[%d]=%s\n",
					routine, P(s), j, P(s->pte[j]));
				exit(1);
			    }
			    if (r->r_true_segval) {
				segval = r->r_seg;
				segval_addr = addr & ~(SEGSIZE - 1);
				found = 1;
				DEBUG_PRINTF(("%s: true segval "
					      "found: %s\n",
					      routine, P(segval)));
				break;
			    }
			}

			if (!found) {
			    segval_addr = addr & ~(SEGSIZE - 1);
			    segval = get_addr2seg(segval_addr);
			    DEBUG_PRINTF(("%s: addr2seg(%s) "
					  "returned %s\n", routine,
					  P(addr), P(segval)));
			}
		    }

		    /*
		     * If the segval we want is actually what first_d,
		     * we reset d to first_d and fall though.
		     */
		    if (first_d->de_virt.dv_segval == segval)
			d = first_d;
		    first_d = 0;
		}

		if (d->de_virt.dv_segval == segval) {
		    if (!(r = rmap_find((long)s->pte[i]))) {
			fflush(stdout);
			fprintf(stderr, "%s: rmap_find(2) failed "
				"for %s[%d]=%s\n",
				routine, P(s), i, P(s->pte[i]));
			exit(1);
		    }
		    r->r_initialized = 1;
		    r->r_true_segval = 1;
		    r->r_seg = segval;
		    add_to_hash(r);
		    DEBUG_PRINTF(("%s: %s segval changed to %s\n",
				  routine, s->pte[i], segval));
		    goto cont_loop;
		}
	    }

	    if (pass == 0 && one_hit  && first_d) {
		s->pte[i] =  (void *)allocate_map(thread,
						  stage,
						  skip,
						  first_d->de_virt.dv_segval,
						  addr);
		DEBUG_PRINTF(("%s: %s[%d]=%s single\n",
			      routine, P(s), i, P(s->pte[i])));
		continue;
	    }

	    if (!one_hit) {
		if (pass == 0)
		    s->pte[i] = (struct stage1 *)no_page_start;
		DEBUG_PRINTF(("%s: %s[%d] set to no_page_start\n",
			      routine, P(s), i));
		continue;
	    }

	    if (!(r = rmap_find((long)s->pte[i]))) {
		fflush(stdout);
		fprintf(stderr, "%s: rmap_find(3) failed for %s[%d]=%s\n",
			routine, P(s), i, P(s->pte[i]));
		exit(1);
	    }

	    if (!r->r_initialized) {
		s->pte[i] = (struct stage1 *)no_page_start;
		rmap_free(r);
		DEBUG_PRINTF(("%s: %s[%d] reset to no_page_start\n",
			      routine, P(s), i));
	    }

	cont_loop: ;

	}
    }

    DEBUG_PRINTF(("%s: return 0\n", routine));
    return 0;
}

/*
 * In this routine, we have to watch out for partial pages in the dump
 * because skip will be PAGESIZE.
 *
 * mult_segs is never true.  Right now, I've put the code it to make
 * the code easy to compare between the three different setup_*
 * functions.
 */
static int setup_final(struct final_stage *s,
		       int thread,
		       unsigned long segval,
		       unsigned long addr,
		       unsigned long skip,
		       enum stages stage)
{
    char *routine = "setup_final";
    unsigned long top;
    int mult_segs = skip > SEGSIZE;
    unsigned long segval_addr = addr & ~(SEGSIZE - 1);
    int i;

    DEBUG_PRINTF(("%s: %s p=%s t=%d\n",
		  routine, s_strings[stage], P(s), thread));
    DEBUG_PRINTF(("%s: s=%s a=%s k=%s\n",
		  routine, P(segval), P(addr), P(skip)));

    for (i = 0; i < STAGE_SIZE; ++i)
	s->pte[i] = no_page_start;

    for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	struct dump_entry *first_d, *d;
	int one_hit = 0;

	top = addr + skip;

	if (IN_USER_RANGE(addr, top)) {
	    s->pte[i] = addr;
	    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

#if 0
	/*
	 * We do not do this.  If a page exists in the dump, it will
	 * get mapped normally below.
	 */
	if (IN_BOS_RANGE(addr, top)) {
	    s->pte[i] = (void *)allocate_map(thread, stage, skip,
					     bos_segval, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s bos\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}
#endif

	first_d = d = starting_d(addr);

	DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
		      routine, P(addr), P(top), P(segval)));
	DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
		      strlen(routine), "found", P(d->de_virt.dv_virt),
		      P(d->de_virt.dv_virt+d->de_len),
		      P(d->de_virt.dv_segval)));

	for (; (d < (dump_entries + dump_entry_cnt) &&
		!d->de_isreal &&
		d->de_virt.dv_virt < top); ++d) {

	    one_hit = 1;

	    if (first_d) {
		if (first_d->de_virt.dv_segval == d->de_virt.dv_segval)
		    continue;
		    
		if (mult_segs) {
		    s->pte[i] = allocate_map(thread,
					     stage,
					     skip,
					     segval,
					     addr);
		    DEBUG_PRINTF(("%s: %s[%d]=%s multi\n",
				  routine, P(s), i, P(s->pte[i])));
		    goto cont_loop;
		}

		if (segval == -thread ||
		    (segval_addr / SEGSIZE) != (addr / SEGSIZE)) {
		    segval_addr = addr & ~(SEGSIZE - 1);
		    segval = get_addr2seg(segval_addr);
		    DEBUG_PRINTF(("%s: addr2seg(%s) returned %s\n",
				  routine, P(addr), P(segval)));
		}

		if (first_d->de_virt.dv_segval == segval)
		    d = first_d;
		first_d = 0;
	    }

	    if (d->de_virt.dv_segval == segval) {
		/*
		 * Note that because lower addresses are ordered first
		 * and larger chunks are also ordered first, if the
		 * first thing we hit that is within our range is not
		 * a full page, then we have only a bunch of partial
		 * pages.
		 */
		if (d->de_len == skip)
		    s->pte[i] = (long)d->de_dump;
		else
		    s->pte[i] = allocate_map(thread, PARTIAL_PAGE, PAGESIZE,
					     segval, addr);
		DEBUG_PRINTF(("%s: %s[%d]=%s\n",
			      routine, P(s), i, P(s->pte[i])));
		goto cont_loop;
	    }
	}

	if (one_hit  && first_d) {
	    if (first_d->de_len == skip)
		s->pte[i] = (long)first_d->de_dump;
	    else
		s->pte[i] = allocate_map(thread, PARTIAL_PAGE, PAGESIZE,
					 segval, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s single\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

    cont_loop: ;

    }
    DEBUG_PRINTF(("%s: return\n", routine));
    return 0;
}

static int setup_partial_page(unsigned long s,
			      int thread,
			      unsigned long segval,
			      unsigned long addr)
{
    struct dump_entry *d = starting_d(addr);
    long top = addr + PAGESIZE;

    DEBUG_PRINTF(("setup_partial_page: p=%s t=%d s=%s a=%s\n",
		  P(s), thread, P(segval), P(addr)));

    while (d->de_virt.dv_virt < top) {
	if (d->de_virt.dv_segval == segval)
	    bcopy((void *)d->de_dump,
		  (void *)(s + (d->de_virt.dv_virt - addr)),
		  d->de_len);
	d++;
    }
    DEBUG_PRINTF(("setup_partial_page: return\n"));
    return 0;
}

static int de_compare(void *a, void *b)
{
    struct dump_entry *da = (struct dump_entry *)a;
    struct dump_entry *db = (struct dump_entry *)b;

    if (da->de_isreal != db->de_isreal)
	return (da->de_isreal) ? 1 : -1;
    if (da->de_isreal) {
	if (da->de_real == db->de_real)
	    return 0;
	return (da->de_real > db->de_real) ? 1 : -1;
    }
    if (da->de_virt.dv_virt != db->de_virt.dv_virt)
	return (da->de_virt.dv_virt > db->de_virt.dv_virt) ? 1 : -1;
    if (da->de_virt.dv_segval != db->de_virt.dv_segval)
	return (da->de_virt.dv_segval > db->de_virt.dv_segval) ? 1 : -1;
    return db->de_len - da->de_len; /* larger one first */
}

static int init_dump(void)
{
    struct stat64 stat_buf;
    char *cur_pos;
    int i;
    int cnt = 0;

    quick_dump = 1;
    if ((dump_fd = open64(dumpname, O_RDWR)) < 0) {
	perror(dumpname);
	return 1;
    }
    if (fstat64(dump_fd, &stat_buf) < 0) {
	perror("fstat");
	return 1;
    }
    dump_file = mmap((void *)map_top, stat_buf.st_size, PROT_READ|PROT_WRITE,
		     MAP_FIXED|MAP_FILE, dump_fd, 0);
    if ((long)dump_file == -1L) {
	perror("mmap");
	return 1;
    }
    dump_file_end = dump_file + stat_buf.st_size;
    map_top += (stat_buf.st_size + PAGESIZE - 1L) & ~(PAGESIZE - 1L);

    for (i = 0; i < 2; ++i) {
	cnt = 0;

	cur_pos = dump_file;
	if (!VALID_DMP_MAGIC(((struct cdt *)cur_pos)->cdt_magic))
	    cur_pos += 512;
	while (cur_pos < dump_file_end &&
	       VALID_DMP_MAGIC(((struct cdt *)cur_pos)->cdt_magic)) {
	    int num_entries = NUM_ENTRIES((struct cdt *)cur_pos);
	    int index;

#ifndef __64BIT__
	    if (ISDMPVR((struct cdt*)cur_pos)) {
		struct cdt_vr *cdt = (struct cdt_vr *)cur_pos;

		if ((cur_pos += cdt->cdt_len) > dump_file_end)
		    break;


		for (index = 0;
		     (cur_pos < dump_file_end) && (index < num_entries);
		     ++index) {
		    struct cdt_entry_vr *e = cdt->cdt_entry + index;

		    if (ISADDRREAL(e)) {
			unsigned long long addr = (unsigned long)e->d_realaddr;
			size_t len = e->d_len;
			unsigned long bit_entries = BITMAPSIZE(addr, len);
			unsigned long pages = NPAGES(addr, len);
			bitmap_t *bitmap = (bitmap_t *)cur_pos;
			int bit;

			cur_pos += (bit_entries * sizeof(bitmap_t));
			if (cur_pos > dump_file_end) {
			    if (i == 0)
				fprintf(stderr, "%s is empty for short dump\n",
					cdt->cdt_name);
			    break;
			}

			for (bit = 0; bit < pages; ++bit) {
			    int size = PAGESIZE - (addr % PAGESIZE);

			    if (size > len)
				size = len;

			    /*
			     * Page is in the dump
			     */
			    if (ISBITMAP(bitmap, bit)) {
				if (i == 1) {
				    dump_entries[cnt].de_isreal = 1;
				    dump_entries[cnt].de_len = size;
				    dump_entries[cnt].de_real = addr;
				    dump_entries[cnt].de_dump = cur_pos;
				}
				++cnt;
				if ((cur_pos += size) > dump_file_end) {
				    if (i == 0)
					fprintf(stderr, "%s is not complete\n",
						cdt->cdt_name);
				    break;
				}
			    }
			    addr += size;
			    len -= size;
			}
		    } else {
			unsigned long addr = (unsigned long)e->d_ptr_v;
			size_t len = e->d_len;
			unsigned long bit_entries = BITMAPSIZE(addr, len);
			unsigned long pages = NPAGES(addr, len);
			bitmap_t *bitmap = (bitmap_t *)cur_pos;
			int bit;

			cur_pos += (bit_entries * sizeof(bitmap_t));
			if (cur_pos > dump_file_end) {
			    if (i == 0)
				fprintf(stderr, "%s is empty for short dump\n",
					cdt->cdt_name);
			    break;
			}

			for (bit = 0; bit < pages; ++bit) {
			    int size = PAGESIZE - (addr % PAGESIZE);

			    if (size > len)
				size = len;

			    /*
			     * Page is in the dump
			     */
			    if (ISBITMAP(bitmap, bit)) {
				if (i == 1) {
				    dump_entries[cnt].de_isreal = 0;
				    dump_entries[cnt].de_len = size;
				    dump_entries[cnt].de_virt.dv_virt = addr;
				    dump_entries[cnt].de_virt.dv_segval =
					e->d_segval_v;
				    dump_entries[cnt].de_dump = cur_pos;
				    if (size == PAGESIZE)
					rmap_fill((unsigned long)cur_pos, size,
						  REAL_STAGE, 0, e->d_segval_v,
						  addr);
				}
				++cnt;
				if ((cur_pos += size) > dump_file_end) {
				    if (i == 0)
					fprintf(stderr, "%s is not complete\n",
						cdt->cdt_name);
				    break;
				}
			    }
			    addr += size;
			    len -= size;
			}
		    }
		}
	    } else
#endif
	    {
		struct cdt *cdt = (struct cdt *)cur_pos;

		if ((cur_pos += cdt->cdt_len) > dump_file_end)
		    break;

		for (index = 0; index < num_entries; ++index) {
		    struct cdt_entry *e = cdt->cdt_entry + index;
		    unsigned long addr = (unsigned long)e->d_ptr;
		    size_t len = e->d_len;
		    unsigned long bit_entries = BITMAPSIZE(addr, len);
		    unsigned long pages = NPAGES(addr, len);
		    bitmap_t *bitmap = (bitmap_t *)cur_pos;
		    int bit;
		    int t_slot;
		    char namebuf[32];

		    cur_pos += (bit_entries * sizeof(bitmap_t));
		    if (cur_pos > dump_file_end) {
			if (i == 0)
			    fprintf(stderr, "%s is empty due to short dump\n",
				    cdt->cdt_name);
			break;
		    }

		    if (!strcmp(cdt->cdt_name, "bos") &&
			!strcmp(e->d_name, "kernel")) {
			bos_segval = e->d_segval;
			bos_addr_start = (long)e->d_ptr;
			bos_addr_end = bos_addr_start + e->d_len;
			DEBUG_PRINTF(("init_dump: bos_segval=%s"
				      " bos_addr_start=%s"
				      " bos_addr_end=%s\n",
				      P(bos_segval),
				      P(bos_addr_start),
				      P(bos_addr_end)));
		    }

		    if (!strcmp(cdt->cdt_name, "thrd")) {
			int idx;
			char buf[8];

			sscanf(e->d_name, "%d%s", &idx, buf);
			if (i == 0) {
			    if (thread_max < idx)
				thread_max = idx;
			} else {
			    t_map[idx] = 1;
			}
		    }

		    for (bit = 0; bit < pages; ++bit) {
			int size = PAGESIZE - (addr % PAGESIZE);

			if (size > len)
			    size = len;

			/*
			 * Page is in the dump
			 */
			if (ISBITMAP(bitmap, bit)) {
			    if (i == 1) {
				dump_entries[cnt].de_isreal = 0;
				dump_entries[cnt].de_len = size;
				dump_entries[cnt].de_virt.dv_virt = addr;
				dump_entries[cnt].de_virt.dv_segval =
				    e->d_segval;
				dump_entries[cnt].de_dump = cur_pos;
				if (size == PAGESIZE)
				    rmap_fill((unsigned long)cur_pos, size,
					      REAL_STAGE, 0, e->d_segval,
					      addr);
			    }
			    ++cnt;
			    if ((cur_pos += size) > dump_file_end) {
				if (i == 0)
				    fprintf(stderr, "%s is not complete\n",
					    cdt->cdt_name);
				break;
			    }
			}
			addr += size;
			len -= size;
		    }
		}
	    }
	}
	if (!cnt) {
	    fprintf(stderr, "%s: Invalid dump\n", progname);
	    return 1;
	}
	if (i == 0 && cnt) {
	    dump_entries = mmap((void *)map_top,
				cnt * sizeof(struct dump_entry),
				PROT_READ|PROT_WRITE,
				MAP_FIXED|MAP_ANONYMOUS,
				-1, 0);
	    if ((long)dump_entries == -1L) {
		perror("mmap2");
		return 1;
	    }

	    map_top += ((cnt * sizeof(struct dump_entry) + PAGESIZE - 1) &
			~(PAGESIZE - 1));
	    ++thread_max;
	    t_map = smalloc(thread_max);
	}
    }

    dump_entry_cnt = cnt;
    qsort(dump_entries, cnt, sizeof(struct dump_entry), de_compare);
    DEBUG_PRINTF(("init_dump: dump of dump_entries\n"));
    for (i = 0; i < cnt; ++i) {
	DEBUG_PRINTF(("%04x %s ", dump_entries[i].de_len,
		      P(dump_entries[i].de_dump)));
	if (dump_entries[i].de_isreal)
	    DEBUG_PRINTF(("r=%s\n", P(dump_entries[i].de_real)));
	else
	    DEBUG_PRINTF(("v=%s s=%s\n",
			  P(dump_entries[i].de_virt.dv_virt),
			  P(dump_entries[i].de_virt.dv_segval)));
    }
    quick_dump = 0;
    DEBUG_PRINTF(("init_dump return 0\n"));
    return 0;
}

/*
 * Not implemented yet -- may never be.  Use to return the start and length of
 * of the cdt_entry whose cdt_name was t_name and d_name was d_name.
 */
int return_range(char *t_name, char *d_name, void **startp, int *lenp)
{
    return 0;
}

/*
 * No longer needed I think
 */
int purge_all_pages(void)
{
}

/*
 * No longer needed I think
 */
int purge_user_pages(void)
{
    
}

/*
 * Called by pseudo C code
 */
int open_dump(char *path)
{
}

void trace_mappings(void)
{
    struct stage0 *stage0;
    struct stage1 *stage1;
    struct stage2 *stage2;
    struct final_stage *final;
    void *v = page_table[THREAD_SLOT];
    long local_v = (long)last_v;
    int shift_mult;
    int index;

    fprintf(stderr, "THREAD_SLOT is %d\n", THREAD_SLOT);
    fprintf(stderr, "last_v is 0x%s\n", P(last_v));
    fprintf(stderr, "last_f is 0x%s\n", P(last_f));
    fprintf(stderr, "map_top is 0x%s\n", P(map_top));

    if (sizeof(long) == 8) {
	shift_mult = 3;
	stage0 = (struct stage0 *)v;
    } else {
	shift_mult = 1;
	stage2 = (struct stage2 *)v;
    }

    for (index = 0; shift_mult >= 0; ++index, --shift_mult) {
	unsigned long real;
	struct rmap *r;
	int index;
	int shift;
	int size;

	r = rmap_find((long)v);
	fprintf(stderr, "page at %s is a %s and is ", P(v),
		s_strings[r->r_stage]);

	if (!r) {
	    if (no_page_start <= (long)v && (long)v < no_page_end)
		fprintf(stderr, "no page\n");
	    else
		fprintf(stderr, "not allocated\n");
	    return;
	}

	if (!r->r_mapped) {
	    fprintf(stderr, "allocated but not mapped\n");
	    return;
	}
	fprintf(stderr, "allocated and mapped\n");

	shift = PGSHIFT+ (STAGE_BITS*shift_mult);
	index = (local_v >> shift) & STAGE_MASK;

	switch (r->r_stage) {
	case STAGE0:
	    size = sizeof(struct stage0);
	    v = stage1 = stage0->pte[index];
	    break;
	case STAGE1:
	    size = sizeof(struct stage1);
	    v = stage2 = stage1->pte[index];
	    break;
	case STAGE2:
	    size = sizeof(struct stage2);
	    v = final  = stage2->pte[index];
	    break;
	case FINAL_STAGE:
	    size = sizeof(struct final_stage);
	    v = (void *)(real = final->pte[index]);
	    break;
	case PARTIAL_PAGE: return;
	case REAL_STAGE:   return;
	}

	fprintf(stderr, "size:%d index:%d index*size:%s new:%s\n",
		size, index, P(index * size), P(v));
    }
}

long get_addr2seg(long addr)
{
    static char *addr2seg_name = "addr2seg";
    static cnode addr2seg_cnode;
    static cnode fcall;
    static cnode_list arg_list;
    static symptr addr2seg_sym;
    symptr stemp = name2userdef_all(addr2seg_name);
    long ret;
    expr *exp;

    if (!stemp)				/* should never happen */
	return 0;

    if (exp = arg_list.cl_cnode.c_expr)
	exp->e_l = addr;
    else if (sizeof(long) == 4)
	mk_const(ns_inter, &arg_list.cl_cnode, addr, TP_LONG);
    else
	mk_const(ns_inter, &arg_list.cl_cnode, addr, TP_LONG_64);

    if (stemp != addr2seg_sym) {
	(void) mk_ident(&addr2seg_cnode, addr2seg_name);
	mk_fcall(&fcall, &addr2seg_cnode, &arg_list);
    }
    DEBUG_PRINTF(("get_addr2seg: calling with addr=%s\n", P(addr)));
    ret = l_fcall(fcall.c_expr);
    DEBUG_PRINTF(("get_addr2seg: return with addr=%s\n", P(ret)));
    return ret;
}
