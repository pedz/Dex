#ifndef _KERNEL
#define _KERNEL
#endif

#ifndef KERNEL
#define KERNEL
#endif

#ifdef __64BIT__
#define __XCOFF64__
#endif

#include <stdio.h>
#include <vmm/vmdefs.h>

struct vmkerdata vmker;

int use_1tb_aliases = 1;

long esidfun(unsigned long eaddr)
{
    unsigned long esid = eaddr >> L2SSIZE;
    unsigned long sidx = ESIDTOSIDX(esid);
    unsigned long sid = 0xfbfb;

    printf("esid fun for %lx\n", eaddr);
    printf("esid is %lx\n", esid);
    printf("sidx is %lx\n", sidx);
    if (IS_1TB_KERNEL_ESID(esid))
	sid = ESID2VSID_1TB(esid);
    else
	sid = ESID2VSID(esid);
    printf("sid is %lx\n", sid);
    return 0;
}
