static char sccs_id[] = "@(#)base_expr.c	1.9";

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "base_expr.h"

#define DEBUG_BIT BASE_EXPR_C_BIT

#define WSIZE (8 * sizeof(ularge_t))

ularge_t get_field(void *addr, int offset, int size)
{
  ularge_t *laddr = addr;
  ularge_t temp;
  int orig_offset = offset;

  DEBUG_PRINTF(("get field addr: %0*lx, offset: %d, size: %d\n",
		sizeof(addr)*2, addr, offset, size));
  DEBUG_PRINTF(("WSIZE = %d, sizeof(ularge_t) = %d\n", WSIZE, sizeof(ularge_t)));
  laddr += (offset / WSIZE);
  offset %= WSIZE;
  DEBUG_PRINTF(("get field new laddr: %0*lx, new offset: %d\n",
	       sizeof(addr)*2, laddr, offset));
  /*
   * We cheat and move laddr by single bytes up until the entire value
   * fits into one fetch.
   */
  while (offset + size > WSIZE) {
    laddr = (ularge_t *)(((char *)laddr) + 1);
    offset -= 8;
  }
  DEBUG_PRINTF(("get field new laddr: %0*lx, new offset: %d\n",
	       sizeof(addr)*2, laddr, offset));
  temp = *laddr;
  temp >>= WSIZE - offset - size;
  temp &= (ularge_t)-1 >> (WSIZE - size);
  return temp;
}

void set_field(void *addr, int offset, int size, ularge_t val)
{
    ularge_t *laddr = addr;
    ularge_t temp;
    ularge_t mask;

    DEBUG_PRINTF(("set laddr:%s offset %d size %d val %s\n",
		  P(laddr), offset, size, P(val)));
    laddr += (offset / WSIZE);
    offset %= WSIZE;
    if (offset + size > WSIZE) {
	fprintf(stderr, "set_field spans alignment boundry\n");
	exit(1);
    }
    DEBUG_PRINTF(("set new laddr:%s offset %d\n", P(laddr), offset));
    temp = *laddr;
    DEBUG_PRINTF(("set *laddr:%s\n", P(temp)));
    mask = (((ularge_t)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
    DEBUG_PRINTF(("set mask:%s\n", P(mask)));
    temp &= ~mask;
    DEBUG_PRINTF(("set *laddr after mask:%s\n", P(temp)));
    val <<= (WSIZE - offset - size);
    val &= mask;
    DEBUG_PRINTF(("set val after mask:%s\n", P(val)));
    temp |= val;
    DEBUG_PRINTF(("set new val:%s\n", P(temp)));
    *(ularge_t *)laddr = temp;
    return;
}
