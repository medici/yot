#ifndef BASE_H
#define BASE_H
#include "base.h"
#include "data.h"
#include "scan.h"
#include "define.h"

typedef struct Object *Object;
typedef struct Type *Type;

struct Object {
  char name[TEXTLEN + 1];
  int class, exno;
  int level;
  bool expo, rdo; /*exported / read-only*/
  Type type;
  Object next;
  Object dsc;
  long int value;
};

struct Type {
  int form;
  int ref; /*ref is only used for import/export*/
  int mno;
  int nofpar; /*for procedures, extension level for records*/
  int len; /*for arrays, len < 0 => open array; for records: adr of descriptor*/
  int size; /*in bytes; always multiple of 4, except for Byte, Bool and Char*/
  Object dsc;
  Object typobj;
  Type base;/*for arrays, records, pointers*/
};


Type IntType, BoolType, RealType, CharType, StringType, NoType, NilType;
Object Universe, TopScope, System;


/* class / mode*/ 
enum {
    CHEAD,
    CVAR,
    CMVAR, // module var
    CCVAR, // const var
    CPAR,
    CCOND,
    CCONST,
    CSTRING,
    CFLD,
    CTYPE,
    CSFUNC,
    CSPROC,
    CPROC,
    CLAB,
    CREG,
    CMOD,
    CREGI
};
/* form */ 
enum {
    FBYTE,
    FBOOLEAN,
    FCHAR,
    FINTEGER,
    FREAL,
    FSET,
    FPOINTER,
    FNILTYP,
    FNOTYPE,
    FFUNC,
    FSTRING,
    FARRAY,
    FRECORD
};

void base_start();
Object new_obj(int class);
Object this_obj();
Object this_import(Object mod);
Object this_field(Type rec);
void export(char *modid);
void import(char *impid);
void open_scope();
void close_scope();
void base_init();


#endif