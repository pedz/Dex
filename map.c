static char sccs_id[] = "@(#)map.c	1.6";
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

static long v2f_map[16] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static long f2v_map[16] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/*
 * Physical segments 4 and up can be used to map virtual segments.
 * 0-2 can not be used because mmap barfs.  3 is for the dump file.
 */
static int next_pseg = 4;

/*
 * right now, all the virtual segments can be used except for 3-6.  We
 * give segment 3 to the pseudo variables.
 */
static int next_vseg = 4;

p_ptr v2f(v_ptr v)
{
    unsigned long r = (long)v;
    int n = (r >> 28) & 0xf;

    if (v2f_map[n] == -1) {
	if (next_pseg == 16) {
	    fprintf(stderr, "Out of segments in v2f %d\n", n);
	    exit(1);
	}
	v2f_map[n] = next_pseg << 28;
	f2v_map[next_pseg++] = n << 28;
#if 0
	printf("v2f: setting v2f[%x] to %x\n", n, v2f_map[n]);
	printf("v2f: setting f2v[%x] to %x\n", next_pseg-1, f2v_map[next_pseg-1]);
#endif
    }
    return (p_ptr)(v2f_map[n] | ((long)v & 0x0fffffff));
}

v_ptr f2v(p_ptr p)
{
    unsigned long r = (long)p;
    int n = (r >> 28) & 0xf;

    if (f2v_map[n] == -1) {
	if (next_vseg == 6) {
	    fprintf(stderr, "Out of segments in f2v %d\n", n);
	    exit(1);
	}
	f2v_map[n] = next_vseg << 28;
	v2f_map[next_vseg++] = n << 28;
#if 0
	printf("f2v: setting f2v[%x] to %x\n", n, f2v_map[n]);
	printf("f2v: setting v2f[%x] to %x\n", next_vseg-1, v2f_map[next_vseg-1]);
#endif
    }
    return (v_ptr)(f2v_map[n] | ((long)p & 0x0fffffff));
}

static void map_catch(int sig, int code, struct sigcontext *scp)
{
    static volatile int count;
    sigset_t t;
    long paddr = scp->sc_jmpbuf.jmp_context.except[0];
    v_ptr vaddr = f2v((void *)paddr);

    if (count >= 10)
	exit(1);
    if (++count >= 10) {
	fprintf(stderr, "Recursive SIGSEGV iar: %08x padd: %08x vaddr: %08x\n",
		scp->sc_jmpbuf.jmp_context.iar, paddr, vaddr);
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
	if ((long)mmap((void *)paddr, PAGESIZE, PROT_READ|PROT_WRITE,
		       MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE, -1, 0) != paddr) {
	    fprintf(stderr, "paddr = %08x: ", paddr);
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
    fprintf(stderr, "\nCan not map: iar=%08x paddr=%08x vaddr=%08x\n",
	    scp->sc_jmpbuf.jmp_context.iar, paddr, vaddr);
    exit(1);
}

void map_init(void)
{
    struct sigaction s;

    s.sa_handler = (void (*)())map_catch;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    if (sigaction(SIGSEGV, &s, (struct sigaction *)0) < 0) {
	perror("sigaction");
	exit(1);
    }
}
