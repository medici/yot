#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
// https://en.wikipedia.org/wiki/C_dynamic_memory_allocation
// https://github.com/gaeduron/Malloc#-guideline
// https://github.com/jterrazz/42-malloc
typedef struct {
  size_t size;
  char allocated;
} block_header;

typedef struct {
  size_t size;
  int fillter;
} block_footer;

void *current_avail = NULL;
void *first_bp;
size_t current_avail_size = 0;

void *mm_malloc3(size_t size);

#define ALIGNMENT 16
#define ALIGN(size) ((size + (ALIGNMENT - 1) & ~(ALIGNMENT - 1)))
#define CHUNK_SIZE (1 << 14)
#define CHUNK_ALIGN(size) (((size) + (CHUNK_SIZE - 1)) & ~(CHUNK_SIZE - 1))
#define OVERHEAD (sizeof(block_header) + sizeof(block_footer))
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - OVERHEAD)
#define GET_SIZE(p) ((block_header *)(p))->size
#define GET_ALLOC(p) ((block_header *)(p))->allocated
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))

int mm_init() {
  sbrk(sizeof(block_header));
  first_bp = sbrk(0);

  GET_SIZE(HDRP(first_bp)) = 0;
  GET_ALLOC(HDRP(first_bp)) = 1;

  mm_malloc3(0);

  return 0;
}

void extend(size_t new_size) {
  size_t chunk_size = CHUNK_ALIGN(new_size);
  void *bp;
  bp = sbrk(chunk_size);

  GET_SIZE(HDRP(bp)) = chunk_size;
  GET_SIZE(FTRP(bp)) = chunk_size;
  GET_ALLOC(HDRP(bp)) = 0;

  GET_SIZE(HDRP(NEXT_BLKP(bp))) = 0;
  GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 1;
}

void set_allocated(void *bp, size_t size) {
  size_t extra_size = GET_SIZE(HDRP(bp)) - size;

  if (extra_size > ALIGN(1 + OVERHEAD)) {
    GET_SIZE(HDRP(bp)) = size;
    GET_SIZE(FTRP(bp)) = size;
    GET_SIZE(HDRP(NEXT_BLKP(bp))) = extra_size;
    GET_SIZE(FTRP(NEXT_BLKP(bp))) = extra_size;
    GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 0;
  }

  GET_ALLOC(HDRP(bp)) = 1;
}

void *mm_malloc(size_t size) {
  size_t newsize = ALIGN(size);
  void *p;

  if (current_avail_size < newsize) {
    sbrk(CHUNK_ALIGN(newsize));
    current_avail_size += CHUNK_ALIGN(newsize);
  }

  p = current_avail;
  current_avail += newsize;
  current_avail_size -= newsize;

  return p;
}

void *mm_malloc2(size_t size) {
  size_t newsize = ALIGN(size);
  void *p;

  if (current_avail_size < newsize) {
    current_avail = mmap(0, CHUNK_ALIGN(newsize), PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANON, -1, 0);

    current_avail_size = CHUNK_ALIGN(newsize);
  }

  p = current_avail;
  current_avail += newsize;
  current_avail_size -= newsize;

  return p;
}

void *mm_malloc3(size_t size) {
  int new_size = ALIGN(size + OVERHEAD);
  void *bp = first_bp;

  while (GET_SIZE(HDRP(bp)) != 0) {
    if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= new_size)) {
      set_allocated(bp, new_size);
      return bp;
    }
    bp = NEXT_BLKP(bp);
  }

  extend(new_size);
  set_allocated(bp, new_size);
  return bp;
}

void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) {
  } else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    GET_SIZE(HDRP(bp)) = size;
    GET_SIZE(FTRP(bp)) = size;
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    GET_SIZE(FTRP(bp)) = size;
    GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  } else {
    size += (GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp))));
    GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    GET_SIZE(FTRP(NEXT_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  }

  return bp;
}

void mm_free(void *bp) {
  GET_ALLOC(HDRP(bp)) = 0;
  coalesce(bp);
}

static void shader() { printf("\033[37;41m"); }
static void unshader() { printf("\033[0m"); }
static void print_allocation_block(void *bp) {
  shader();
  printf("| %ld/%ld |  %ld  | %ld ", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)),
         GET_SIZE(HDRP(bp)) - OVERHEAD, GET_SIZE(FTRP(bp)));
  unshader();
}
static void print_free_block(void *bp) {
  printf("| %ld/%ld |  %ld  | %ld ", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)),
         GET_SIZE(HDRP(bp)) - OVERHEAD, GET_SIZE(FTRP(bp)));
}

static void print_chunk(void *first) {
  void *bp;
  bp = first;

  while (GET_SIZE(HDRP(bp)) != 0) {
    if (GET_ALLOC(HDRP(bp)) == 1) {
      print_allocation_block(bp);
    } else {
      print_free_block(bp);
    }
    bp = NEXT_BLKP(bp);
  }

  printf("| 0/1 |\n");
}

int main() {
  void *block;
  void *prev_block;
  void *next_block;

  if (mm_init() == 0) {
    print_chunk(first_bp);

    block = mm_malloc3(1);
    print_chunk(first_bp);
    mm_malloc3(100);
    print_chunk(first_bp);
    mm_free(block);
    print_chunk(first_bp);
    mm_malloc3(200);
    print_chunk(first_bp);
    mm_malloc3(300);
    print_chunk(first_bp);
    prev_block = mm_malloc3(150);
    print_chunk(first_bp);
    block = mm_malloc3(150);
    print_chunk(first_bp);
    mm_free(prev_block);
    print_chunk(first_bp);
    mm_free(block);
    print_chunk(first_bp);
    block = mm_malloc3(150);
    print_chunk(first_bp);
    next_block = mm_malloc3(150);
    print_chunk(first_bp);
    mm_free(next_block);
    print_chunk(first_bp);
    mm_free(block);
    print_chunk(first_bp);
    prev_block = mm_malloc3(150);
    block = mm_malloc3(150);
    next_block = mm_malloc3(150);
    print_chunk(first_bp);
    mm_free(prev_block);
    mm_free(next_block);
    print_chunk(first_bp);
    mm_free(block);
    print_chunk(first_bp);
    next_block = mm_malloc3(1);
    print_chunk(first_bp);
  }

  return 0;
}
