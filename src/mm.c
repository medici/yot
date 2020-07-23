#include "mm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void *checked_malloc(int len) {
  void *p = malloc(len);
  if (!p) {
    fprintf(stderr, "\nRan out of memory!\n");
    exit(1);
  }
  return p;
}

static void *find_fit(size_t size) {
  void *bp;
  Heap heap = top;

  while (heap) {
    bp = heap->heap;
    while (GET_SIZE(HDRP(bp)) != 0) {
      if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= size) {
        return bp;
      }
      bp = NEXT_BLKP(bp);
    }
    heap = heap->next;
  }

  return (void *)-1;
}

static void set_allocated(void *bp, size_t size) {
  size_t extra_size = GET_SIZE(HDRP(bp)) - size;

  if (extra_size > ALIGN(1 + OVERHEAD)) {
    PUT(HDRP(bp), PACK(size, 1));
    PUT(FTRP(bp), PACK(size, 1));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(extra_size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(extra_size, 0));
  } else {
    PUT(HDRP(bp), PACK(size, 1));
    PUT(FTRP(bp), PACK(size, 1));
  }
}

static void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) {
    return bp;
  } else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  } else {
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  return bp;
}

static void *extend_heap(size_t size) {
  Heap new_heap = checked_malloc(sizeof(*new_heap));
  size_t chunk_size = CHUNK_ALIGN(size + OVERHEAD * 3);
  new_heap->heap = mmap(0, chunk_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON, -1, 0);
  printf("mmap %d\n", new_heap->heap);
  new_heap->len = chunk_size;

  if (new_heap->heap == (void *)-1) {
    return (void *)-1;
  }

  PUT(new_heap->heap, PACK(2 * WORDSIZE, 1));
  PUT(new_heap->heap + WORDSIZE, PACK(2 * WORDSIZE, 1));

  new_heap->heap += (3 * WORDSIZE);
  chunk_size -= 3 * WORDSIZE;
  PUT(HDRP(new_heap->heap), PACK(chunk_size, 0));
  PUT(FTRP(new_heap->heap), PACK(chunk_size, 0));
  PUT(HDRP(NEXT_BLKP(new_heap->heap)), PACK(0, 1));
  new_heap->heap -= (2 * WORDSIZE);

  new_heap->next = top;
  top = new_heap;

  return new_heap->heap;
}

void *mm_malloc(size_t size) {
  size_t new_size = ALIGN(size + OVERHEAD);
  void *bp = find_fit(new_size);

  if (bp == (void *)-1) {
    bp = extend_heap(new_size);
    bp = find_fit(new_size);
    if (bp == (void *)-1) {
      return (void *)-1;
    }
  }
  set_allocated(bp, new_size);
  return bp;
}

void mm_free(void *bp) {
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

int mm_clean(Heap heap) {
  while (top) {
    if (munmap(top->heap - WORDSIZE, top->len) != 0) {
      return -1;
    }
    top = top->next;
  }

  return 0;
}
