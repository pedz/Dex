static char sccs_id[] = "@(#)map.c	1.1";
#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "dex.h"
#include "dmap.h"

/*
 * This is a general purpose virtual memory system.  Given a "virtual"
 * address, it will provide the contents with that address.  I do not
 * believe I am going to provide a virtual address to physical address
 * mapping.  The contents are provided by a routine with an obvious
 * calling sequence.  This routine is specified on a per virtual page
 * bases.  Currently there are two routines.  One for dex's address
 * space and one for crash dumps.  dex's address space consumes
 * 0x80000000 in the virtual space.  (Eventually, this address will be
 * settable by a command line argument.)  So to the interpreter, a
 * pseudo variable will have the address of 0x8000000 and up.  This
 * will get remapped into an address within dex.
 * 
 * Currently we fill pages which do not exist with 0's and we do this
 * silently.  I haven't put much thought into that problem.
 */

struct page {
    struct page *p_next;
    v_ptr p_addr;
    m_func p_f;
};
struct page *hash[128];
#define HASH_SIZE (sizeof(hash) / sizeof(hash[0]))

static int v_hash(v_ptr addr)
{
    unsigned long w = ((unsigned long)addr) >> PGSHIFT;

    w ^= w >> (32 - PGSHIFT);
    return w % HASH_SIZE;
}

static struct page *v_find(v_ptr addr)
{
    struct page *r;

    for (r = hash[v_hash(addr)];
	 r && r->p_addr != addr;
	 r = r->p_next);
    return r;
}

m_type v_read(p_ptr buf, v_ptr addr, m_size size)
{
    struct page *p;
    m_func f;
    v_ptr base_addr = (v_ptr)((unsigned long)addr & PAGE_MASK);

    if (p = v_find(base_addr))
	f = p->p_f;
    else
	f = read_dump;
    (*f)(buf, addr, size, 0);
}

m_type v_write(p_ptr buf, v_ptr addr, m_size size)
{
    struct page *p;
    v_ptr base_addr = (v_ptr)((unsigned long)addr & PAGE_MASK);

    if (p = v_find(base_addr))
	(*p->p_f)(buf, addr, size, 1);
}

m_type v_mkpage(v_ptr addr, m_func f)
{
    struct page *p;
    v_ptr base_addr = (v_ptr)((unsigned long)addr & PAGE_MASK);
    int h;

    if (p = v_find(base_addr))
	return;
    p = new(struct page);
    p->p_addr = base_addr;
    p->p_f = f;
    h = v_hash(base_addr);
    p->p_next = hash[h];
    hash[h] = p;
}
