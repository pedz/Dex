static char sccs_id[] = "@(#)base_expr.c	1.7";

#include <stdio.h>
#include <setjmp.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "base_expr.h"

#define WSIZE (8 * sizeof(ularge_t))

ularge_t get_field(void *addr, int offset, int size)
{
  ularge_t *laddr = addr;
  ularge_t temp;

  laddr += (offset / WSIZE);
  offset %= WSIZE;
  if (offset + size > WSIZE) {
    fprintf(stderr, "get_field spans alignment boundry\n");
    exit(1);
  }
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

  printf("set laddr:%08x offset %d size %d val %016llx\n",
	 laddr, offset, size, val);
  laddr += (offset / WSIZE);
  offset %= WSIZE;
  if (offset + size > WSIZE) {
    fprintf(stderr, "set_field spans alignment boundry\n");
    exit(1);
  }
  printf("set new laddr:%08x offset %d\n", laddr, offset);
  temp = *laddr;
  printf("set *laddr:%016llx\n", temp);
  mask = (((ularge_t)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
  printf("set mask:%016llx\n", mask);
  temp &= ~mask;
  printf("set *laddr after mask:%016llx\n", temp);
  val <<= (WSIZE - offset - size);
  val &= mask;
  printf("set val after mask:%016llx\n", val);
  temp |= val;
  printf("set new val:%016llx\n", temp);
  *(ularge_t *)laddr = temp;
  return;
}
