static char sccs_id[] = "@(#)map.c	1.9";
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include "map.h"

/*
 * This is the second attempt at a general purpose virtual memory
 * system.  I now provide a v2f function to convert a virtual address
 * into a "possible" physical address.  It is possible because if we
 * are coming from a dump, the address might not be in the dump in
 * which case attempting to access it will give a SIGSEGV trap.
 * 
 * The scheme is key on several RS/6000 specific facts.  mmap seems to
 * work for segments 3 through C and we need to map kernel segments 0,
 * 2, B, and E.  We accomplish this by flipping the highest bit (or
 * making physical address == (virtual address ^ 0x80000000).  This
 * puts the four segments that we need to map into the range that we
 * can map them.  The pseudo variables and stack are then put in
 * virtual address 0x40000000 which makes it physical address
 * 0xC0000000.
 *
 * The scheme works by setting a handler for SIGSEGV.  The handler
 * looks at the address being accessed and fills in that page using
 * mmap from the dump.  In the case of a running system, a timer will
 * be added to flush these pages on a periodic basis so that "fresh"
 * values are retrieved from the system.
 *
 * The plan is to do a longjmp in the case the address being accessed
 * is not in the dump so that routines can simply do a setjmp and then
 * go screaming around without doing checks for 0's.
 *
 * Update: The flipping of the high bit stopped working in later
 * versions of AIX 4.3.2 so I implemented two maps v2f_map and f2v_map
 * which use the high nibble as the index to map into what the new
 * high nibble should be.  The pseudo variables are at 0x30000000.  I
 * moved the dump mmap to 0x40000000.  So the first segment available
 * is 5.  A call in map_init sets up a mapping for real segment 2 so
 * the pseudo code can get to real C variables.  The rest of the
 * virtual to physical mappings happen as they come.  map_catch will
 * look to see if a mapping already exists and if not, will set up a
 * new mapping for the segment.  With this scheme (and assume mmap
 * still craps out after segment C, we can map 7 segments from the
 * dump into the pseudo code.  In the past, 3 is all we needed.  With
 * all the new features, etc, we can run out of them under extreme
 * conditions.
 *
 * This may change but another change is to make v2f and f2v real
 * functions.  The mappings are created automatically when they are
 * called rather than from map_catch.
 *
 */

/*
 * The number of virtual segments we know about is a power of two.  It
 * is 2 raised to the power of VSEG_POWER.
 */
#ifdef __64BIT__

#define VSEG_POWER 8
#define VSEG_BITS  34

#else

#define VSEG_POWER 6
#define VSEG_BITS  32

#endif

#define VSEG_SHIFT (VSEG_BITS - (VSEG_POWER))
#define VSEG_BIT_MASK (((long long)1 << VSEG_BITS) - 1)
#define VSEGS (1<<(VSEG_POWER))
#define VSEG_MASK ((VSEGS) - 1)
#define VSEG_MASK_LO ((((long long)-1) & VSEG_BIT_MASK) >> VSEG_POWER)
#define VSEG_MASK_HI (~(VSEG_MASK_HI))

static long v2f_map[VSEGS];
static long f2v_map[VSEGS];

/*
 * Physical segments 4 and up can be used to map virtual segments.
 * 0-2 can not be used because mmap barfs.  3 is for the dump file.
 */
static int next_pseg = ((0x40000000 >> VSEG_SHIFT) & VSEG_MASK);

/*
 * right now, all the virtual segments can be used except for 3-6.  We
 * give segment 3 to the pseudo variables.
 */
static int next_vseg = ((0x40000000 >> VSEG_SHIFT) & VSEG_MASK);

p_ptr v2f(v_ptr v)
{
    unsigned long r = (long)v;
    long n = (r >> VSEG_SHIFT) & VSEG_MASK;

    if (v2f_map[n] == -1) {
	if (next_pseg == VSEGS) {
	    fprintf(stderr, "Out of segments in v2f %d\n", n);
	    exit(1);
	}
	v2f_map[n] = next_pseg << VSEG_SHIFT;
	f2v_map[next_pseg] = n << VSEG_SHIFT;
#ifdef MAP_DEBUG
	printf("v2f: map of %0*lx, map vseg %d to vseg %d\n", sizeof(long) * 2,
	       r, n, next_pseg);
	printf("v2f: setting v2f[%0*lx] to %0*lx\n",
	       sizeof(long) * 2, n << VSEG_SHIFT,
	       sizeof(long) * 2, v2f_map[n]);
	printf("v2f: setting f2v[%0*lx] to %0*lx\n",
	       sizeof(long) * 2, next_pseg << VSEG_SHIFT,
	       sizeof(long) * 2, f2v_map[next_pseg]);
#endif
	++next_pseg;
    }
#ifdef MAP_DEBUG_2
    printf("v2f: map %0*lx to %0*lx\n",
	   sizeof(long) * 2, r,
	   sizeof(long) * 2, (v2f_map[n] | ((long)v & VSEG_MASK_LO)));
#endif
    return (p_ptr)(v2f_map[n] | ((long)v & VSEG_MASK_LO));
}

v_ptr f2v(p_ptr p)
{
    unsigned long r = (long)p;
    long n = (r >> VSEG_SHIFT) & VSEG_MASK;

    if (f2v_map[n] == -1) {
	if (next_vseg == ((0x6000000 >> VSEG_SHIFT) & VSEG_MASK)) {
	    fprintf(stderr, "Out of segments in f2v %d\n", n);
	    exit(1);
	}
	f2v_map[n] = next_vseg << VSEG_SHIFT;
	v2f_map[next_vseg] = n << VSEG_SHIFT;
#ifdef MAP_DEBUG
	printf("f2v: map of %0*lx, map fseg %d to vseg %d\n",
	       sizeof(long) * 2, r, n, next_vseg);
	printf("f2v: setting f2v[%0*lx] to %0*lx\n",
	       sizeof(long) * 2, n << VSEG_SHIFT,
	       sizeof(long) * 2, f2v_map[n]);
	printf("f2v: setting v2f[%0*lx] to %0*lx\n",
	       sizeof(long) * 2, next_vseg << VSEG_SHIFT,
	       sizeof(long) * 2, v2f_map[next_vseg]);
#endif
	next_vseg++;
    }
#ifdef MAP_DEBUG_2
    printf("f2v: map %0*lx to %0*lx\n",
	   sizeof(long) * 2, r,
	   sizeof(long) * 2, (f2v_map[n] | ((long)p & VSEG_MASK_LO)));
#endif
    return (v_ptr)(f2v_map[n] | ((long)p & VSEG_MASK_LO));
}

#ifdef __64BIT__
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
#ifdef __64BIT__
    long paddr = (long)info->si_band;
#else
    long paddr = scp->sc_jmpbuf.jmp_context.except[0];
#endif
    v_ptr vaddr = f2v((void *)paddr);

#ifdef MAP_DEBUG
    printf("catch sig:%d paddr:0x%0*lx vaddr:0x%0*lx\n",
	   sig,
	   sizeof(long)*2, paddr,
	   sizeof(long)*2, vaddr);
#endif
    if (count >= 10)
	exit(1);
    if (++count >= 10) {
	fprintf(stderr, "Recursive SIGSEGV padd: %0*lx vaddr: %0*lx\n",
		sizeof(long) * 2, paddr,
		sizeof(long) * 2, vaddr);
	exit(1);
    }

    sigemptyset(&t);
    sigaddset(&t, SIGSEGV);
    if (sigprocmask(SIG_UNBLOCK, &t, (sigset_t *)0) < 0)
	perror("sigprocmask");
    
    /*
     * User pseudo segment -- just map a blank page and continue.
     */
    if (vaddr >= h_base && vaddr < h_high) {
	paddr &= ~(PAGESIZE - 1);
#ifdef MAP_DEBUG
	printf("mmap 1 0x%0*lx\n", sizeof(long) * 2, paddr);
#endif
	if ((long)mmap((void *)paddr, PAGESIZE, PROT_READ|PROT_WRITE,
		       MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE, -1, 0) != paddr) {
	    fprintf(stderr, "paddr = %0*lx: ", sizeof(long) * 2, paddr);
	    perror("mmap4");
	    exit(1);
	}
	--count;
	return;
    }
    /*
     * We attempt to map paddr to vaddr.  If this works, we return
     * happy.  Otherwise we do a longjmp.
     */
    if (!map_addr(vaddr)) {
	--count;
	return;
    }
    if (map_jmp_ptr) {
	--count;
	longjmp(map_jmp_ptr, vaddr);
    }
    fprintf(stderr, "\nCan not map: paddr=%0*lx vaddr=%0*lx\n",
	    sizeof(long) * 2, paddr,
	    sizeof(long) * 2, vaddr);
    exit(1);
}

void map_init(void)
{
    struct sigaction s;
    int i;

    s.sa_handler = (void (*)())map_catch;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &s, (struct sigaction *)0) < 0) {
	perror("sigaction");
	exit(1);
    }
    for (i = VSEGS; --i >= 0; )
	v2f_map[i] = f2v_map[i] = -1;
}
