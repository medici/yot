#ifndef MM_H
#define MM_H
#include <unistd.h>

#include "sys.h"

#ifdef ENVIRONMENT64

#define CHUNKSIZE (1 << 14)
typedef unsigned long block;

#else

#define CHUNKSIZE (1 << 12)
typedef unsigned int block;

#endif
#define CHUNK_ALIGN(size) (((size) + (CHUNKSIZE - 1)) & ~(CHUNKSIZE - 1))
#define GET(p) (*(block *)(p))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define ALIGNMENT (WORDSIZE * 2)
#define OVERHEAD (WORDSIZE * 2)
#define ALIGN(size) ((size + (ALIGNMENT - 1) & ~(ALIGNMENT - 1)))
#define PACK(size, alloc) ((size) | (alloc))
#define PUT(p, val) (*(block *)(p) = (val))
#define HDRP(bp) ((char *)(bp)-WORDSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - OVERHEAD)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))

typedef struct Heap *Heap;
struct Heap {
  void *heap;
  Heap next;
  size_t len;
};

Heap top;

void *mm_malloc(size_t size);
void mm_free(void *bp);
int mm_clean(Heap heap);

#endif