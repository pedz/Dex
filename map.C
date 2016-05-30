static char sccs_id[] = "@(#)map.c	1.15";

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
#include "cdt.h"

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
 * entries created for each full page found in the dump.  I believe
 * this is because f2v does an rmap lookup and if we are doing an f2v
 * of an address in the dump file, we will need an rmap entry or we
 * will exit with an error.
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
 * weird.  This is the case where the pte's each cover less than a
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
 * file comes from base.c and is called base.o for 32 bit and base.64o
 * for 64 bit.)  addr2seg can then use pseudo-C code to dig around in
 * the kernel to find the value of the segment register for the
 * address that it is passed.
 *
 * The down fall with this is that addr2seg can page fault.  So, while
 * addr2seg is being called to resolve a page fault, a second page
 * fault can happen.  This is why span_seg_setup is "weird" or hard to
 * do.
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
 * The r_initialized bit is kept off instead of a special segment
 * value of UNKNOWN.
 *
 * We can add a bit like r_true_segval which will be set if addr2seg
 * is called.  In that case, in span_seg_setup, before addr2seg is
 * called, pte entries in the same segment can be scanned and if they
 * are initialized and true_segval is set, then we do not call
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

    unsigned long r_virt;		/* virtual address */
    int r_thread;			/* thread this is for (0 => all) */
    enum stages r_stage;		/* what stage is this */

    unsigned long r_seg;		/* segment value */
    struct rmap *r_hash;		/* hash chain */

    int r_lineno;			/* line # where added to hash */
    uint r_mapped : 1;			/* true if actually mmaped */
    uint r_hashed : 1;			/* true if in hash */
    uint r_freed : 1;			/* true if freed */
    uint r_initialized : 1;		/* true if initilized */
    uint r_phys_set : 1;		/* true if r_phys/r_psize is valid */
    uint r_true_segval : 1;
    uint r_in_addr2seg : 1;
};

struct dump_entry {
    unsigned int de_isreal : 1;
    unsigned long de_len;		/* length of entry */
    union {
	struct {
	    unsigned long dv_virt;	/* virtual address */
	    unsigned long dv_segval;	/* segment register */
	    unsigned long dv_end;	/* end of region */
	    unsigned long dv_min;	/* see note just below */
	} du_virt;
	unsigned long long du_real;	/* real address */
    } de_union;
    void *de_dump;			/* offset into dump */
};

#define de_virt   de_union.du_virt.dv_virt
#define de_segval de_union.du_virt.dv_segval
#define de_end    de_union.du_virt.dv_end
#define de_min    de_union.du_virt.dv_min
#define de_real   de_union.du_real

/*
 * Note about dv_min: The dump entries are sorted by du_virt.dv_end
 * with the smaller du_virt.dv_end addresses at the lower end of the
 * dump_entries.  After the sort, the array is traversed from high to
 * low and dv_min is set to the lowest beginning address found so far.
 *
 * When we went to look for entries, we do a binary search using
 * du_virt.dv_end to find where in the array to start.  We will know
 * that all entries below the one we start at will have ending
 * addresses lower than the lowest address we are trying to fill and
 * so can not possibly be useful to look at.  We will look at entries
 * until dv_min is above the highest address that we are trying to
 * fill which will tell us that all entries above that point start
 * above the point we are trying to fill.
 *
 * This should limit the range of entries we look at considerably.
 */

/* C++ funky prototypes to make the link work */
extern "C" p_ptr v2f(v_ptr v);
extern "C" v_ptr f2v(p_ptr p);
extern "C" long get_addr2seg(long addr);
extern "C" int purge_all_pages(void);
extern "C" int purge_user_pages(void);
extern "C" int return_range(char *t_name, char *d_name, void **startp, int *lenp);
extern "C" void trace_mappings(void);
extern "C" int open_dump(char *path);
extern "C" int map_init(void);
extern "C" char *print_field(int,...);
extern "C" void fail(int);
extern "C" void *safe_malloc(unsigned long,char*,int);
extern "C" symptr name2userdef_all(char*);
extern "C" void mk_const(name_space*,cnode*,long long,int);
extern "C" int mk_ident(cnode*,char*);
extern "C" void mk_fcall(cnode*,cnode*,cnode_list*);
extern "C" long l_fcall(expr*);
extern "C" void *safe_realloc(void*,unsigned long,unsigned long,char*,int);

/* Local Prototypes */
static struct rmap *rmap_find(long l);
static void add_to_hash(struct rmap *r, int lineno);
static int map_addr(long l);
#if 1 /* def __64BIT__ */
static void map_catch(int sig, siginfo_t *info, ucontext_t *context);
#else
static void map_catch(int sig, int code, struct sigcontext *scp);
#endif
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
static int init_dump(void);
long get_addr2seg(long addr);
static void rmap_dump(void);

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
static unsigned long bos_segval;
static unsigned long bos_addr_start;
static unsigned long bos_addr_end;
static char *s_strings[] = {
    "stage0", "stage1", "stage2", "final", "partial", "real"
};
/*
 * real_mode is true if the dump contains no entries with a segment
 * value other than DUMP_REAL_SEGVAL and DUMP_NONMEM_SEGVAL.
 */
int real_mode = 1;

int thread_slot = -1;
size_t thread_max;			/* last thread in dump */
char *t_map;
long map_top;				/* last allocated virtual address */
int fromdump = 1;			/* always from a dump for now */
void * volatile last_v;
void * volatile last_f;

#define FAST_MAP

#ifdef FAST_MAP
p_ptr v2f(v_ptr v)
{
    last_v = v;
    last_f = (void *)-1;
    return last_f = V2F_MACRO(v);
}
#else
#ifdef __64BIT__

p_ptr v2f(v_ptr v)
{
    volatile struct stage0 *S0;
    volatile struct stage1 *S1;
    volatile struct stage2 *S2;
    volatile struct final_stage *FS;
    volatile unsigned long FN;
    volatile unsigned long I0;
    volatile unsigned long I1;
    volatile unsigned long I2;
    volatile unsigned long I3;
    volatile unsigned long I4;

    last_v = v;
    last_f = (void *)-1;

    S0 = page_table[THREAD_SLOT];
    I0 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*3)) & STAGE_MASK;
    I1 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*2)) & STAGE_MASK;
    I2 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK;
    I3 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK;
    I4 = (((unsigned long)(v)) & (PAGESIZE - 1));
    S1 = S0->pte[I0];
    S2 = S1->pte[I1];
    FS = S2->pte[I2];
    FN = FS->pte[I3];
    return (p_ptr)(FN + I4);
}

#else

p_ptr v2f(v_ptr v)
{
    volatile struct stage2 *S2;
    volatile struct final_stage *FS;
    volatile unsigned long FN;
    volatile unsigned long I2;
    volatile unsigned long I3;
    volatile unsigned long I4;

    last_v = v;
    last_f = (void *)-1;

    S2 = page_table[THREAD_SLOT];
    I2 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*1)) & STAGE_MASK;
    I3 = (((unsigned long)(v)) >> PGSHIFT+(STAGE_BITS*0)) & STAGE_MASK;
    I4 = (((unsigned long)(v)) & (PAGESIZE - 1));
    FS = S2->pte[I2];
    FN = FS->pte[I3];
    return (p_ptr)(FN + I4);
}

#endif /* __64BIT__ */
#endif /* FAST_MAP */

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

/*
 * This is a "predicate" that is true if the value passed in is an
 * "invalid" segment value.  On 64 bit systems, a 0 segment value is
 * sometimes found in dumps but the bos segment value is not 0 so on
 * 64 bit systems, a 0 is "invalid".  On all systems, a -1 is also
 * invalid.
 *
 * This routine is used in searchs and basically an "invalid" segment
 * value is effectively a wild card -- it could match any segment
 * value.  The code should pick a page that has an exact matching
 * segment value.  But if there is not one with an exact match, a page
 * in (with the same address range) and an invalid (or wild card)
 * segment value is used.  (If there are more than one of these --
 * tough.)
 *
 * Had to modify this to pass in addr since a some dump table entries
 * put a segment value of 0 even if they are in a completely different
 * segment than the bos segment.  So, for a segval of 0 to be valid,
 * bos_segval must be 0 and the address must be between bos_addr_start
 * and bos_addr_end.
 */
static int invalid_segval_p(unsigned long segval, unsigned long addr)
{
    return ((segval == (unsigned long)-1) ||
	    (sizeof(segval) == 4 && segval == 0x007fffff) ||
	    (segval == 0 &&(bos_segval != 0 ||
			    (addr < bos_addr_start || addr >= bos_addr_end))));
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

static void add_to_hash(struct rmap *r, int lineno)
{
    int hash = rmap_hash(r);

    DEBUG_PRINTF(("add_to_hash: %s, %d\n", P(r), lineno));
    if (r->r_hashed) {
	fflush(stdout);
	fprintf(stderr, "add_to_hash: %s:%d already in hash from %d\n", P(r),
		lineno, r->r_lineno);
	exit(1);
    }

    /*
     * An extra bit is needed for r_hashed since it can be at the end
     * of the list and r_hash will be 0.  r_hashed is only used to
     * catch programming errors.
     */
    r->r_hashed = 1;
    r->r_lineno = lineno;
    r->r_hash = (*rmap_hash_table)[hash];
    (*rmap_hash_table)[hash] = r;
}

static int map_addr(long l)
{
    struct rmap *r;
    unsigned long skip;
    void *mmap_ret;
    enum stages stage;
    int i;

    /*
     * If we hit the page that says "the page does not exist in the
     * dump" then we just return unhappy.
     */
    if (no_page_start <= l && l < no_page_end) {
	long temp = (long)last_v;
	struct stage0 *p;
	int offset;
	int shift;

	DEBUG_PRINTF(("thread slot:%d\n", thread_slot));
	DEBUG_PRINTF(("last_v:0x%s last_f=0x%s\n", P(last_v), P(last_f)));
	p = (struct stage0 *)page_table[THREAD_SLOT];
	DEBUG_PRINTF(("page_table[%d]=0x%s\n", THREAD_SLOT, P(p)));
	for (shift = MAX_SHIFT_CNT; shift >= 0; --shift) {
	    if (!p || ((long)p >= no_page_start && (long)p < no_page_end))
		break;
	    offset = (temp >> (PGSHIFT+(STAGE_BITS*shift))) & STAGE_MASK;
	    p = (struct stage0 *)(p->pte[offset]);
	    DEBUG_PRINTF(("p[%d]=0x%s\n", offset, P(p)));
	}
	return 1;
    }

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
	    DEBUG_PRINTF(("addr2seg recursed %d %s\n", r->r_thread,
			  P(r->r_virt)));
	    return 1;
	}
	r->r_in_addr2seg = 1;
	segval = get_addr2seg(r->r_virt);
	r->r_in_addr2seg = 0;
	r->r_initialized = 1;
	r->r_true_segval = 1;
	r->r_seg = segval;
	add_to_hash(r, __LINE__);
    }

    if (debug_mask & MMAP_BIT)
	printf("mmap: 1 s=%s l=%s e=%s\n", P(r->r_phys), P(r->r_psize),
	       P(r->r_phys + r->r_psize));
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
	stage = STAGE2;
	skip = ((((unsigned long)-1) >> (STAGE_BITS * 2)) + 1);
	return setup_stage((struct stage0 *)r->r_phys, r->r_thread, r->r_seg,
			   r->r_virt, skip, stage);

    case STAGE2:
	if (INITIAL_STAGE == STAGE2)
	    return setup_thread(r->r_thread);
	stage = FINAL_STAGE;
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
    long paddr;

    /*
     * The documentation says this is not correct but it seems to work for now.
     */
#if 1 /* def __64BIT__ */
    /*
     * This keeps changing.  It use to be this:
     paddr = (long)info->si_addr;
     */

    paddr = context->uc_mcontext.jmp_context.except[0];

#else
    long paddr = scp->sc_jmpbuf.jmp_context.except[0];
#endif

    DEBUG_PRINTF(("map_catch: last_v:0x%s thread:%d paddr:0x%s\n",
		  P(last_v), thread_slot, P(paddr)));
    if (stmt_stack[cur_stmt_index]) {
	struct stmt *s = statements + stmt_stack[cur_stmt_index];

	if (s)
	    DEBUG_PRINTF(("map_catch: File %s line %d\n",
			  s->stmt_file, s->stmt_line));
    }

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


    s.sa_handler = (void (*)(int))map_catch;
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
     * and rethink a few things.
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
    if (debug_mask & MMAP_BIT)
	printf("mmap: 2 s=%s l=%s e=%s\n", P(0), P(sizeof(initial_stage_t)),
	       P(sizeof(initial_stage_t)));
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

    if (real_mode) {
      thread_max = 524288;
      bos_addr_start = 0;
      bos_addr_end = 256 * 1024 * 1024; // 256M
    }

    DEBUG_PRINTF(("thread_max=%d\n", thread_max));
    page_table = (initial_stage_t **)smalloc((size_t)(thread_max + 1) * sizeof(page_table[0]));
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

	    rmap = (struct rmap *)smalloc(rmap_max * sizeof(*rmap));
	    rmap_hash_table = (struct rmap *(*)[])
		smalloc(rmap_max*sizeof((*rmap_hash_table)[0]));
	    DEBUG_PRINTF(("rmap initialized to %d\n", rmap_max));
	} else {
	    size_t r_old = rmap_max * sizeof(*rmap);
	    size_t r_new = (rmap_max = (1 << ++rmap_shift)) * sizeof(*rmap);
	    size_t h_new = rmap_max * sizeof((*rmap_hash_table)[0]);
	    struct rmap *r, *r_end;

	    r = rmap = (struct rmap *)srealloc(rmap, r_new, r_old);
	    r_end = r + rmap_used;
	    free(rmap_hash_table);
	    rmap_hash_table = (struct rmap *(*)[])smalloc(h_new);

	    for ( ; r < r_end; ++r)
		if (r->r_hashed) {
		    r->r_hashed = 0;
		    add_to_hash(r, __LINE__);
		}

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
    if (r->r_hashed) {
	fflush(stdout);
	fprintf(stderr, "rmap_free: r_hashed set for %s\n", P(r));
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

static void rmap_debug(void)
{
    DEBUG_PRINTF(("rmap is out of whack!!\n"));
}

static void rmap_fill(unsigned long phys,
		      unsigned long psize,
		      enum stages stage,
		      int thread,
		      unsigned long seg,
		      unsigned long virt)
{
    static unsigned long last_phys;
    struct rmap *rp;
    int hash;

    if (debug_mask & QUICK_RMAP_BIT)
	DEBUG_PRINTF(("rmap_fill: r=%s:%s %13s %4d\n"
		      "           v=%s seg=%s\n",
		      P(phys), P(psize), s_strings[stage], thread, P(virt),
		      P(seg)));

    if (phys < last_phys)
	rmap_debug();
    last_phys = phys;

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
    add_to_hash(rp, __LINE__);
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
    add_to_hash(r, __LINE__);

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

	if (!de->de_isreal && addr > de->de_end)
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

static int setup_thread(int thread)
{
    initial_stage_t *s = page_table[thread];
    unsigned long segval = -thread;
    unsigned long addr = 0;
    unsigned long skip = (((unsigned long)-1) >> STAGE_BITS) + 1;
    enum stages stage = INITIAL_STAGE_P1;
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
    struct dump_entry *d_end = dump_entries + dump_entry_cnt;
    unsigned long top;
    int i;

    for (i = 0; i < STAGE_SIZE; ++i)
	s->pte[i] = (struct stage1 *)no_page_start;

    for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	struct dump_entry *first_d = 0, *d;
	int one_hit = 0;

	top = addr + skip;

	if (IN_USER_RANGE(addr, top)) {
	    s->pte[i] = (struct stage1 *)allocate_map(thread, stage, skip, 0, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

	if (IN_BOS_RANGE(addr, top)) {
	    s->pte[i] = (struct stage1 *)allocate_map(thread, stage, skip,
					     bos_segval, addr);
	    DEBUG_PRINTF(("%s: %s[%d]=%s bos\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

	d = starting_d(addr);

	DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
		      routine, P(addr), P(top), P(segval)));
	DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
		      strlen(routine), "found",
		      P(d->de_virt),
		      P(d->de_virt+d->de_len),
		      P(d->de_segval)));

	/*
	 * The for loop makes sure that d points to a valid entry,
	 * it is not a real segment, and that addr <= dv_virt < top.
	 * I will need to hook up support for real segments someday
	 * probably to make the vmm people happy.
	 */
	for ( ; d < d_end && d->de_min < top; ++d) {
	    if (d->de_isreal ||
		(d->de_virt > top) ||
		(d->de_end < addr))
		continue;

	    if (!one_hit)
		first_d = d;

	    /* Flag that says we have at least one page in this range */
	    one_hit = 1;

	    /*
	     * If we have the same segval, just continue.  Otherwise,
	     * we set up a map using the segval passed in and we are
	     * done.
	     */
	    if (invalid_segval_p(first_d->de_segval, addr))
		first_d = d;

	    if (invalid_segval_p(d->de_segval, addr) ||
		first_d->de_segval == d->de_segval)
		continue;

	    s->pte[i] = (struct stage1 *)allocate_map(thread,
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
	    s->pte[i] =  (struct stage1 *)allocate_map(thread,
					      stage,
					      skip,
					      first_d->de_segval,
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
    struct dump_entry *d_end = dump_entries + dump_entry_cnt;
    unsigned long addr_save = addr;
    unsigned long segval_addr = addr & ~(SEGSIZE - 1);
    unsigned long top;
    struct rmap *rmaps[STAGE_SIZE];
    int pass;
    int i;

    for (pass = 0; pass < 2; ++pass) {
	addr = addr_save;

	for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	    struct dump_entry *first_d = 0, *d;
	    int one_hit = 0;
	    struct rmap *r;

	    top = addr + skip;

	    if (IN_USER_RANGE(addr, top)) {
		if (pass == 0) {
		    s->pte[i] = (struct stage1 *)allocate_map(thread, stage,
						     skip, 0,
						     addr);
		    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
				  routine, P(s), i, P(s->pte[i])));
		}
		continue;
	    }

	    if (IN_BOS_RANGE(addr, top)) {
		if (pass == 0) {
		    s->pte[i] = (struct stage1 *)allocate_map(thread, stage, skip,
						     bos_segval, addr);
		    DEBUG_PRINTF(("%s: %s[%d]=%s bos\n",
				  routine, P(s), i, P(s->pte[i])));
		}
		continue;
	    }

	    d = starting_d(addr);

	    DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
			  routine, P(addr), P(top), P(segval)));
	    DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
			  strlen(routine), "found",
			  P(d->de_virt),
			  P(d->de_virt+d->de_len),
			  P(d->de_segval)));

	    for ( ; d < d_end && d->de_min < top; ++d) {
		if (d->de_isreal ||
		    (d->de_virt > top) ||
		    (d->de_end < addr))
		    continue;

		if (!one_hit)
		    first_d = d;

		one_hit = 1;

		if (first_d) {

		    if (invalid_segval_p(first_d->de_segval, addr))
			first_d = d;

		    if (invalid_segval_p(d->de_segval, addr) ||
			(first_d->de_segval == d->de_segval))
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
			     addr_tmp += skip, ++j) {

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
		    if (first_d->de_segval == segval)
			d = first_d;
		    first_d = 0;
		}

		if (d->de_segval == segval) {
		    if (!(r = rmap_find((long)s->pte[i]))) {
			fflush(stdout);
			fprintf(stderr, "%s: rmap_find(2) failed "
				"for %s[%d]=%s\n",
				routine, P(s), i, P(s->pte[i]));
			exit(1);
		    }
		    if (r->r_hashed) {
			if (!r->r_initialized ||
			    !r->r_true_segval ||
			    r->r_seg != segval) {
			    fflush(stdout);
			    fprintf(stderr, "span_seg_setup: rehash with different values");
			    exit(1);
			}
		    } else {
			r->r_initialized = 1;
			r->r_true_segval = 1;
			r->r_seg = segval;
			add_to_hash(r, __LINE__);
		    }
		    DEBUG_PRINTF(("%s: %s segval changed to %s\n",
				  routine, P(s->pte[i]), P(segval)));
		    goto cont_loop;
		}
	    }

	    DEBUG_PRINTF(("%s: pass:%d one_hit:%d first_d:%d\n",
			  routine, pass, one_hit, first_d != 0));

	    /*
	     * Only one real segval in this range
	     */
	    if (pass == 0 && one_hit  && first_d) {
		s->pte[i] =  (struct stage1 *)allocate_map(thread,
						  stage,
						  skip,
						  first_d->de_segval,
						  addr);
		DEBUG_PRINTF(("%s: %s[%d]=%s single\n",
			      routine, P(s), i, P(s->pte[i])));
		continue;
	    }

	    /*
	     * No pages at all in this range
	     */
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
 */
static int setup_final(struct final_stage *s,
		       int thread,
		       unsigned long segval,
		       unsigned long addr,
		       unsigned long skip,
		       enum stages stage)
{
    char *routine = "setup_final";
    struct dump_entry *d_end = dump_entries + dump_entry_cnt;
    unsigned long top;
    unsigned long segval_addr = addr & ~(SEGSIZE - 1);
    int i;

    DEBUG_PRINTF(("%s: %s p=%s t=%d\n",
		  routine, s_strings[stage], P(s), thread));
    DEBUG_PRINTF(("%s: s=%s a=%s k=%s\n",
		  routine, P(segval), P(addr), P(skip)));

    for (i = 0; i < STAGE_SIZE; ++i)
	s->pte[i] = no_page_start;

    for (i = 0; i < STAGE_SIZE; ++i, addr = top) {
	struct dump_entry *first_d = 0, *d, first_partial;
	int one_hit = 0;
	int partial_hit = 0;

	top = addr + skip;

	if (IN_USER_RANGE(addr, top)) {
	    s->pte[i] = addr;
	    DEBUG_PRINTF(("%s: %s[%d]=%s user\n",
			  routine, P(s), i, P(s->pte[i])));
	    continue;
	}

	d = starting_d(addr);

	DEBUG_PRINTF(("%s: v=%s-%s s=%s\n",
		      routine, P(addr), P(top), P(segval)));
	DEBUG_PRINTF(("%*s: v=%s-%s s=%s\n",
		      strlen(routine), "found", P(d->de_virt),
		      P(d->de_virt+d->de_len),
		      P(d->de_segval)));

	/*
	 * starting_d finds a place to start searching through the
	 * dump_entry and we stop when the minimum address in the
	 * entry is above the last address we are looking for (top).
	 */
	for ( ; d < d_end && d->de_min < top; ++d) {
	    if (d->de_isreal ||
		(d->de_virt > top) ||
		(d->de_end < addr))
		continue;

	    /*
	     * first_d is our first hit and it will get refined to our
	     * preferred hit if we are forced to pick a "non-obvious"
	     * match, i.e. a wildcard segment register or a partial
	     * page.
	     */
	    if (!one_hit)
		first_d = d;

	    /*
	     * We've had at least one hit so we can map something
	     * somehow
	     */
	    one_hit = 1;

	    /* If this is an obvious match, then take it and go */
	    if (!invalid_segval_p(segval, addr) && segval != -thread &&
		d->de_segval == segval &&
		d->de_virt <= addr &&
		(d->de_virt + d->de_len) >= top) {
		s->pte[i] = (long)d->de_dump +
			(addr - d->de_virt);
		DEBUG_PRINTF(("%s: xxx de_segval=%s virt=%s len=%s top=%s\n",
			      routine, P(d->de_segval), P(d->de_virt),
			      P(d->de_len), P(top)));
		DEBUG_PRINTF(("%s: %s[%d]=%s match\n",
			      routine, P(s), i, P(s->pte[i])));
		goto cont_loop;
	    }

	    if (first_d) {
		DEBUG_PRINTF(("%s: first_d: true\n", routine));

		/*
		 * If first_d is a wildcard segment register than the
		 * current d is at leat as valid.  We know neither are
		 * a parfect match.
		 */
		if (invalid_segval_p(first_d->de_segval, addr)) {
		    first_d = d;
		    DEBUG_PRINTF(("%s: first_d: set first_d to d\n", routine));
		}

		/*
		 * Since I don't fully understand all the gunk below,
		 * I'm going to add in a very specific case that I need.
		 * If d's segval is now a wildcard and its a full page,
		 * it must be better than first_d.
		 */
		if (invalid_segval_p(d->de_segval, addr) &&
		    d->de_virt <= addr &&
		    (d->de_virt + d->de_len) >= top) {
		    first_d = d;
		    DEBUG_PRINTF(("%s: Setting first_d to d special case\n"));
		    continue;
		}

		/*
		 * I don't get this.  This is saying that if the
		 * current d is a wildcard and we had a match on the
		 * segment register before, then this can't possibly
		 * be as good.  But thats not true.  The match before
		 * could be a partial page.
		 */
		if (invalid_segval_p(d->de_segval, addr) ||
		    (first_d->de_segval == d->de_segval)) {
		    DEBUG_PRINTF(("%s: first_d continue de_segval=%s\n",
				  routine, P(d->de_segval)));
		    continue;
		}

		/*
		 * I don't remember what segval == -thread implies.
		 */
		if (segval == -thread ||
		    (segval_addr / SEGSIZE) != (addr / SEGSIZE)) {
		    segval_addr = addr & ~(SEGSIZE - 1);
		    segval = get_addr2seg(segval_addr);
		    DEBUG_PRINTF(("%s: first_d: addr2seg(%s) returned %s\n",
				  routine, P(addr), P(segval)));
		}

		/*
		 * If (I guess) the new segval now matches first_d's
		 * segment val, we set d back to where we started and
		 * repeat our search.  I guess, if we come in with
		 * segval equal to -thread, we eventually call
		 * addr2seg to get the segment value we are really
		 * looking for.  All this seems to make a LOT of
		 * assumptions in that the first_d->de_segval is not a
		 * wildcard, etc.
		 */
		if (first_d->de_segval == segval) {
		    d = first_d;
		    DEBUG_PRINTF(("%s: first_d: set d to first_d\n", routine));
		}

		first_d = 0;
		DEBUG_PRINTF(("%s: first_d: set to 0\n", routine));
	    }

	    if (d->de_segval == segval) {
		if (d->de_len >= skip) {
		    s->pte[i] = (long)d->de_dump + (addr - d->de_virt);
		    DEBUG_PRINTF(("%s: %s[%d]=%s frog\n",
				  routine, P(s), i, P(s->pte[i])));
		    goto cont_loop;
		}
		DEBUG_PRINTF(("setting partial_hit\n"));
		partial_hit = 1;
	    }
	}

	if (one_hit && (first_d || partial_hit)) {
	    if (first_d->de_len >= skip) {
		s->pte[i] = (long)first_d->de_dump +
		    (addr - first_d->de_virt);
		DEBUG_PRINTF(("%s: %s[%d]=%s single\n",
			      routine, P(s), i, P(s->pte[i])));
	    } else {
		s->pte[i] = allocate_map(thread, PARTIAL_PAGE, PAGESIZE,
					 segval, addr);
		DEBUG_PRINTF(("%s: %s[%d]=%s partial\n",
			      routine, P(s), i, P(s->pte[i])));

	    }
	    continue;
	}

    cont_loop: ;

    }
    DEBUG_PRINTF(("%s: return\n", routine));
    return 0;
}

/*
 * If things are working right (giggle), then we get here only if we
 * are positive that we have only partial pages for this particular
 * page and not a full page.
 */
static int setup_partial_page(unsigned long s,
			      int thread,
			      unsigned long segval,
			      unsigned long addr)
{
    struct dump_entry *d = starting_d(addr);
    struct dump_entry *d_end = dump_entries + dump_entry_cnt;
    long top = addr + PAGESIZE;

    DEBUG_PRINTF(("setup_partial_page: p=%s t=%d s=%s a=%s\n",
		  P(s), thread, P(segval), P(addr)));

    for ( ; d < d_end && d->de_min < top; ++d) {
	if (d->de_isreal ||
	    (d->de_virt > top) ||
	    (d->de_end < addr))
	    continue;

	if (d->de_segval == segval) {
	    if (d->de_len > PAGESIZE) {
		fflush(stdout);
		fprintf(stderr, "Should not have set up partial page\n");
		fprintf(stderr, "s=%s segval=%s addr=%s\n", P(s), P(segval),
			P(addr));
		fprintf(stderr, "virt=%s len=%s end=%s\n",
			P(d->de_virt), P(d->de_len),
			P(d->de_end));
		fail(1);
	    }
	    bcopy((void *)d->de_dump,
		  (void *)(s + (d->de_virt - addr)),
		  d->de_len);
	}
    }
    DEBUG_PRINTF(("setup_partial_page: return\n"));
    return 0;
}

/*
 * The dump entries are sorted by ending address as the first key and
 * starting address as the second key.
 */
static int de_compare_end(const void *a, const void *b)
{
    struct dump_entry *da = (struct dump_entry *)a;
    struct dump_entry *db = (struct dump_entry *)b;

    if (da->de_isreal != db->de_isreal)
	return (da->de_isreal) ? 1 : -1;

    if (da->de_isreal) {
	unsigned long long da_end = da->de_real + da->de_len - 1;
	unsigned long long db_end = db->de_real + db->de_len - 1;

	return ((db_end == da_end) ?
		((db->de_real == da->de_real) ?
		 0 :
		 (da->de_real > db->de_real) ?
		 1 :
		 -1) :
		(da_end > db_end) ?
		1 :
		-1);
    }

    return ((db->de_end == da->de_end) ?
	    ((db->de_virt == da->de_virt) ?
	     0 :
	     (da->de_virt > db->de_virt) ?
	     1 :
	     -1) :
	    (da->de_end > db->de_end) ?
	    1 :
	    -1);
}

/*
 * This can set up to three dump entries.  An entry for a partial page
 * on the front of the range, an entry for a sequence of full pages,
 * and an entry for a partial page at the end of the range.  It also
 * sets up an rmap if needed for the range of full pages.  If the
 * entry is for a real range, only one entry is set up.
 */
static void setup_dump_entry(int *inmap,
			     int *cntp,
			     int i,
			     int isreal,
			     unsigned long total_size,
			     unsigned long long real,
			     unsigned long virt,
			     unsigned long segval,
			     char *pos)
{
    int cnt = *cntp;
    unsigned long part_size;

    if (*inmap) {
	if (isreal) {
	    if (i == 1) {
		dump_entries[cnt].de_isreal = isreal;
		dump_entries[cnt].de_len = total_size;
		dump_entries[cnt].de_dump = pos;
		dump_entries[cnt].de_real = real;
	    }
	    ++cnt;
	} else {

	    if (segval != DUMP_NONMEM_SEGVAL)
		real_mode = 0;

	    /* Partial page at front of range */
	    if (virt & (PAGESIZE - 1)) {
		if (i == 1) {
		    part_size = PAGESIZE - (virt & (PAGESIZE - 1));
		    if (part_size > total_size)
			part_size = total_size;

		    dump_entries[cnt].de_isreal = isreal;
		    dump_entries[cnt].de_len = part_size;
		    dump_entries[cnt].de_dump = pos;
		    dump_entries[cnt].de_virt = virt;
		    dump_entries[cnt].de_segval = segval;
		    dump_entries[cnt].de_end = virt + part_size - 1;

		    total_size -= part_size;
		    pos += part_size;
		    virt += part_size;
		}
		++cnt;
	    }

	    /* Sequence of full pages */
	    if (total_size >= PAGESIZE) {
		if (i == 1) {
		    part_size = total_size & ~(PAGESIZE - 1);

		    dump_entries[cnt].de_isreal = isreal;
		    dump_entries[cnt].de_len = part_size;
		    dump_entries[cnt].de_dump = pos;
		    dump_entries[cnt].de_virt = virt;
		    dump_entries[cnt].de_segval = segval;
		    dump_entries[cnt].de_end = virt + part_size - 1;


		    rmap_fill((unsigned long)pos, part_size, REAL_STAGE,
			      0, segval, virt);

		    total_size -= part_size;
		    pos += part_size;
		    virt += part_size;
		}
		++cnt;
	    }

	    /* Partial page at end of range */
	    if (total_size) {
		if (i == 1) {
		    dump_entries[cnt].de_isreal = isreal;
		    dump_entries[cnt].de_len = total_size;
		    dump_entries[cnt].de_dump = pos;
		    dump_entries[cnt].de_virt = virt;
		    dump_entries[cnt].de_segval = segval;
		    dump_entries[cnt].de_end = virt + total_size - 1;

		    total_size -= total_size;
		    pos += total_size;
		    virt += total_size;
		}
		++cnt;
	    }
	}
	*cntp = cnt;
	*inmap = 0;
    }
}

static int init_dump(void)
{
    struct stat64 stat_buf;
    char *cur_pos;
    unsigned long min;
    struct dump_entry *d;
    int i;
    int cnt = 0;

    quick_dump = 1;
    if ((dump_fd = open64(dumpname, O_RDONLY)) < 0) {
	perror(dumpname);
	return 1;
    }
    if (fstat64(dump_fd, &stat_buf) < 0) {
	perror("fstat");
	return 1;
    }
    if (debug_mask & MMAP_BIT)
	printf("mmap: 3 s=%s l=%s e=%s\n", P(map_top),
	       P(stat_buf.st_size),
	       P(map_top + stat_buf.st_size));
    if ((sizeof(long) == 4) && stat_buf.st_size > 2LL * 1024 * 1024 * 1024)
	stat_buf.st_size = 2LL * 1024 * 1024 * 1024 - 1;
    dump_file = (char *)mmap((void *)map_top, stat_buf.st_size, PROT_READ,
			     MAP_SHARED|MAP_FIXED|MAP_FILE, dump_fd, 0);

    if ((long)dump_file == -1L) {
	fprintf(stderr, "st_size is %d st_size is %lld\n", sizeof(stat_buf.st_size),
		stat_buf.st_size);
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
	while (cur_pos < dump_file_end) {
	    class CDT *cdt;
	    class CDT_32 cdt_32;
	    class CDT_64 cdt_64;
	    class CDT_vr cdt_vr;
	    class CDT_u32 cdt_u32;
	    class CDT_u64 cdt_u64;
	    class CDT_ras  cdt_ras;
	    class CDT_uras cdt_uras;
	    char *header_name;

	    switch (((struct cdt *)cur_pos)->cdt_magic) {
	    case DMP_MAGIC_32:
		cdt_32.header_setup(cur_pos);
		cdt = & cdt_32;
		break;

	    case DMP_MAGIC_VR:
		cdt_vr.header_setup(cur_pos);
		cdt = & cdt_vr;
		break;

	    case DMP_MAGIC_64:
		cdt_64.header_setup(cur_pos);
		cdt = & cdt_64;
		break;

	    case DMP_MAGIC_U32:
		cdt_u32.header_setup(cur_pos);
		cdt = & cdt_u32;
		break;

	    case DMP_MAGIC_U64:
		cdt_u64.header_setup(cur_pos);
		cdt = & cdt_u64;
		break;

	    case DMP_MAGIC_RAS:
		cdt_ras.header_setup(cur_pos);
		cdt = & cdt_ras;
		break;

	    case DMP_MAGIC_RAS_U:
		cdt_uras.header_setup(cur_pos);
		cdt = & cdt_uras;
		break;

	    case DMP_MAGIC_UD32:
	    case DMP_MAGIC_UD64:
		fprintf(stderr, "Out of sync with magic equal to %08x\n",
			((struct cdt *)cur_pos)->cdt_magic);
		goto loop_end;
	    case DMP_MAGIC_END:
	    case 0:
		goto loop_end;

	    default:
		fprintf(stderr, "Exiting with magic equal to %08x\n",
			((struct cdt *)cur_pos)->cdt_magic);
		goto loop_end;
	    }
	    header_name = cdt->header_name();

	    for (cur_pos = cdt->first_group(cur_pos);
		 cdt->more_groups();
		 cur_pos = cdt->next_group(cur_pos)) {
		for (cur_pos = cdt->first_entry(cur_pos);
		     cdt->more_entrys();
		     cur_pos = cdt->next_entry(cur_pos)) {

		    char *name = cdt->entry_name();
		    size_t sizeof_name = cdt->entry_name_size();
		    u_longlong_t addr = (u_longlong_t) cdt->entry_addr();
		    size_t sizeof_addr = cdt->entry_addr_size();
		    u_longlong_t segval = cdt->entry_segval();
		    size_t sizeof_segval = cdt->entry_segval_size();
		    size_t len = cdt->entry_len();
		    int isreal = cdt->entry_isreal();
		    unsigned long bit_entries = BITMAPSIZE(addr, len);
		    unsigned long pages = NPAGES(addr, len);
		    bitmap_t *bitmap = (bitmap_t *)cur_pos;
		    int bit;

		    int t_slot;
		    char namebuf[32];
		    int inmap;
		    unsigned long total_size;
		    unsigned long long start_addr;
		    char *start_pos;

		    cur_pos += (bit_entries * sizeof(bitmap_t));
		    if (cur_pos > dump_file_end) {
			if (i == 0)
			    fprintf(stderr, "%s is empty due to short dump\n",
				    name);
			goto loop_end;
		    }

		    if (!strcmp(header_name, "bos") &&
			!strcmp(name, "kernel")) {
			bos_segval = segval;
			bos_addr_start = (long)addr;
			bos_addr_end = bos_addr_start + len;
			DEBUG_PRINTF(("init_dump: bos_segval=%s"
				      " bos_addr_start=%s"
				      " bos_addr_end=%s\n",
				      P(bos_segval),
				      P(bos_addr_start),
				      P(bos_addr_end)));
		    }

		    if (!strcmp(header_name, "thrd")) {
			int idx;
			char buf[8];

			sscanf(name, "%d%s", &idx, buf);
			if (i == 0) {
			    if (thread_max < idx)
				thread_max = idx;
			} else {
			    t_map[idx] = 1;
			}
		    }

		    inmap = 0;
		    total_size = 0;

		    for (bit = 0; bit < pages; ++bit) {
			int size = PAGESIZE - (addr % PAGESIZE);

			if (size > len)
			    size = len;

			/*
			 * Page is in the dump
			 */
			if (ISBITMAP(bitmap, bit)) {
			    if (!inmap) {
				start_addr = addr;
				start_pos = cur_pos;
				total_size = 0;
				inmap = 1;
			    }
			    total_size += size;
			    if ((cur_pos += size) > dump_file_end) {
				if (i == 0)
				    fprintf(stderr, "%s is not complete 0\n",
					    name);
				goto loop_end;
			    }
			} else {
			    setup_dump_entry(&inmap, &cnt, i, isreal,
					     total_size, start_addr,
					     (long)start_addr,
					     segval, start_pos);
			}
			addr += size;
			len -= size;
		    }
		    setup_dump_entry(&inmap, &cnt, i, isreal, total_size,
				     start_addr, (long)start_addr, segval,
				     start_pos);
		}
	    }
	}
    loop_end:
	printf("i = %d; real_mode = %d\n", i, real_mode);
	if (!cnt) {
	    fprintf(stderr, "%s: Invalid dump\n", progname);
	    return 1;
	}

	/*
	 * First pass we allocate space for dump_entries and t_map.
	 */
	if (i == 0 && cnt) {
	    if (debug_mask & MMAP_BIT)
		printf("mmap: 4 s=%s l=%s e=%s\n", P(map_top),
		       P(cnt * sizeof(struct dump_entry)),
		       P(map_top + cnt * sizeof(struct dump_entry)));
	    dump_entries = (struct dump_entry *)
		mmap((void *)map_top, cnt * sizeof(struct dump_entry),
		     PROT_READ|PROT_WRITE, MAP_FIXED|MAP_ANONYMOUS,
		     -1, 0);
	    if ((long)dump_entries == -1L) {
		perror("mmap2");
		return 1;
	    }

	    map_top += ((cnt * sizeof(struct dump_entry) + PAGESIZE - 1L) &
			~(PAGESIZE - 1L));
	    ++thread_max;
	    t_map = (char *)smalloc(thread_max);
	}
    }

    atexit(rmap_dump);

    dump_entry_cnt = cnt;
    qsort(dump_entries, cnt, sizeof(struct dump_entry), de_compare_end);
    min = -1;
    for (d = dump_entries + cnt; --d >= dump_entries; ) {
	if (d->de_isreal)
	    continue;

	if (min > d->de_virt)
	    min = d->de_virt;
	d->de_min = min;
    }
    DEBUG_PRINTF(("init_dump: dump of dump_entries\n"));
    for (i = 0; i < cnt; ++i) {
	DEBUG_PRINTF(("%s ", P(dump_entries[i].de_dump)));
	if (dump_entries[i].de_isreal)
	    DEBUG_PRINTF(("r=%s len=%s\n",
			  P(dump_entries[i].de_real),
			  P(dump_entries[i].de_len)));
	else {
	    DEBUG_PRINTF(("v=%s s=%s",
			  P(dump_entries[i].de_virt),
			  P(dump_entries[i].de_segval)));
	    DEBUG_PRINTF((" min=%s e=%s\n",
			  P(dump_entries[i].de_min),
			  P(dump_entries[i].de_end)));
	}
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
    printf("in purge_all_pages\n");
    return 0;
}

/*
 * No longer needed I think
 */
int purge_user_pages(void)
{
    printf("in purge_user_pages\n");
    return 0;
}

/*
 * Called by pseudo C code
 */
int open_dump(char *path)
{
    return 0;
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

static void rmap_dump(void)
{
    int i = 0;
    struct rmap *r = rmap;
    struct rmap *r_end = r + rmap_used;

    DEBUG_PRINTF(("rmap dump\n"));
    DEBUG_PRINTF(("number %*s %*s %*s %*s thread    stage mfiptx\n",
		  sizeof(long)*2, "phys",
		  sizeof(long)*2, "size",
		  sizeof(long)*2, " seg",
		  sizeof(long)*2, "virt"));
    for ( ; r < r_end; ++r, ++i)
	DEBUG_PRINTF(("%6d %s %s %s %s %6d %8s %d%d%d%d%d%d\n",
		      i, P(r->r_phys), P(r->r_psize), P(r->r_seg),
		      P(r->r_virt), r->r_thread, s_strings[r->r_stage],
		      r->r_mapped, r->r_freed, r->r_initialized,
		      r->r_phys_set, r->r_true_segval, r->r_in_addr2seg));
}
