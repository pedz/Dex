static char sccs_id[] = "@(#)map.c	1.3";
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
 */

static void map_catch(int sig, int code, struct sigcontext *scp)
{
    static volatile int count;
    sigset_t t;
    long paddr = scp->sc_jmpbuf.jmp_context.except[0];
    v_ptr vaddr = f2v(paddr);

    if (count >= 5)
	exit(1);
    if (++count >= 5) {
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
	if ((long)mmap(paddr, PAGESIZE, PROT_READ|PROT_WRITE,
		       MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE, -1, 0) != paddr) {
	    perror("mmap");
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
