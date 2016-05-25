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
    unsigned long vpage;

    vmker.stoibits = 0x10;
    printf("vmker.stoibits = 0x%x\n", vmker.stoibits);
    vmker.stoimask = (1 << vmker.stoibits) - 1;
    printf("vmker.stoimask = 0x%x\n", vmker.stoimask);
    vmker.stoibits_1tb = 0xB;
    printf("vmker.stoibits_1tb = 0x%x\n", vmker.stoibits_1tb);
    vmker.stoimask_1tb = (1 << vmker.stoibits_1tb) - 1;
    printf("vmker.stoimask_1tb = 0x%x\n", vmker.stoimask_1tb);

    printf("esid fun for 0x%lx\n", eaddr);
    printf("esid is 0x%lx\n", esid);
    printf("sidx is 0x%lx\n", sidx);
    if (IS_1TB_KERNEL_ESID(esid)) {
	printf("1tb esid\n");
	sid = ESID2VSID_1TB(esid);
	printf("sid is 0x%lx\n", sid);
    } else {
	printf("not 1tb esid\n");
	sid = ESID2VSID(esid);
	printf("sid is 0x%lx\n", sid);
    }
    if (sid == INVLSID) {
	printf("Invalid sid\n");
	return 0;
    }
    vpage = (eaddr & VSID2SOFFSET(sid)) >> 12;
    printf("vpage = 0x%lx\n", vpage);
    /* printf("IS_1TB_SCB(%lx) = 0x%lx\n", sidx, IS_1TB_SCB(sidx)); */
    /* printf("_ITOS_1TB(%lx)  = 0x%lx\n", sidx, _ITOS_1TB(sidx)); */
    /* printf("_ITOS_256M(%lx) = 0x%lx\n", sidx, _ITOS_256M(sidx)); */
    return 0;
}
