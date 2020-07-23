#ifndef DEFINE_H
#define DEFINE_H

#include <string.h>

typedef char bool;

#ifndef SCCDIR
#define SCCDIR "."
#endif

#define streq(s1, s2) (strcmp(s1, s2) == 0)
#define strneq(s1, s2) (strcmp(s1, s2) != 0)

#define TEXTLEN 512

#define EXIT_FAILURE 1

#define TRUE 1
#define FALSE 0

#define KEYWORD 34
#define MAXINT 2147483647

#define PREFIX 'Y'
#define LPREFIX 'L'

#define FP 29

#define MAXEXP 38

typedef struct Keyword Keyword;

struct Keyword {
  int symbol;
  char id[12];
};

typedef union {
  float value;
  struct {
    // Order is important.
    // Here the members of the union data structure
    // use the same memory (32 bits).
    // The ordering is taken
    // from the LSB to the MSB.
    unsigned int mantissa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;

  } raw;
} Float;

#endif