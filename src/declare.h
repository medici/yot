#ifndef DECLARE_H
#define DECLARE_H

#include "define.h"

// misc
void *checked_malloc(int len);
void copy_ident(char *name, char *s);

long int IEEE754float(Float var);

void init(void);
void enter_keyword(int symbol, char *name);


// scan
void next();
void get(void);

// error
void mark(char *s, char *a);
void fatal(char *s);

void collect(char *file);

#endif