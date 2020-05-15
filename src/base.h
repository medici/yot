#ifndef BASE_H
#define BASE_H
#include "base.h"
#include "data.h"
#include "scan.h"
#include "define.h"
#include "table.h"


Type IntType, BoolType, RealType, CharType, StringType, NoType, NilType;
Table Universe, TopScope, System;


/* class / mode*/ 
enum {
    CHEAD,
    CVAR,
    CMVAR, // module var
    CCVAR, // const var
    CPAR,
    CCOND,
    CCONST,
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