#include "mm.h"

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"

static void shader() { printf("\033[37;41m"); }
static void unshader() { printf("\033[0m"); }
static void print_allocation_block(void *bp) {
  shader();
  printf("| %ld/%ld |  %ld  | %ld ", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)),
         GET_SIZE(HDRP(bp)) - 2 * WORDSIZE, GET_SIZE(FTRP(bp)));
  unshader();
}
static void print_free_block(void *bp) {
  printf("| %ld/%ld |  %ld  | %ld ", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)),
         GET_SIZE(HDRP(bp)) - 2 * WORDSIZE, GET_SIZE(FTRP(bp)));
}

static void print_chunk() {
  void *bp;
  Heap curent = top;
  int index = 1;

  printf("\n\n");

  while (curent) {
    printf("HEAP %d\n", index);
    bp = curent->heap;

    while (GET_SIZE(HDRP(bp)) != 0) {
      if (GET_ALLOC(HDRP(bp)) == 1) {
        print_allocation_block(bp);
      } else {
        print_free_block(bp);
      }
      bp = NEXT_BLKP(bp);
    }

    printf("| 0/1 |\n");
    curent = curent->next;
    ++index;
  }
}

int main() {
  // void *block;
  // void *prev_block;
  // void *next_block;

  // print_chunk();

  // block = mm_malloc(1);
  // print_chunk();

  // mm_malloc(100);
  // print_chunk();
  // mm_free(block);
  // print_chunk();
  // mm_malloc(200);
  // print_chunk();
  // mm_malloc(300);
  // print_chunk();
  // prev_block = mm_malloc(150);
  // print_chunk();
  // block = mm_malloc(150);
  // print_chunk();
  // mm_free(prev_block);
  // print_chunk();
  // mm_free(block);
  // print_chunk();
  // block = mm_malloc(150);
  // print_chunk();
  // next_block = mm_malloc(150);
  // print_chunk();
  // mm_free(next_block);
  // print_chunk();
  // mm_free(block);
  // print_chunk();
  // prev_block = mm_malloc(150);
  // block = mm_malloc(150);
  // next_block = mm_malloc(150);
  // print_chunk();
  // mm_free(prev_block);
  // mm_free(next_block);
  // print_chunk();
  // mm_free(block);
  // print_chunk();
  // next_block = mm_malloc(1);
  // print_chunk();

  // next_block = mm_malloc(16328);
  // print_chunk();

  // mm_clean(top);

  FILE *out = fopen("mmap.test", "w+");

  fprintf(out, "adfsafsfsfsafsfsfsdf\n");
  fprintf(out, "22222222222222222222\n");
  fprintf(out, "99999999999999999999 L2222222222\n");
  long int pos = ftell(out) - 11;
  fprintf(out, "                    \n");
  fprintf(out, "88888888888888888888\n");
  fprintf(out, "66666666666666666666\n");
  fseek(out, pos, SEEK_SET);
  fprintf(out, "%-30ld", 1111111111111111112);
  fseek(out, pos, SEEK_SET);
  char str1[20];
  long int dev;
  fscanf(out, "%ld", &dev);
  printf("%-20ld sfd\n", dev);
  fseek(out, 0, SEEK_END);
  fprintf(out, "11111\n");
}