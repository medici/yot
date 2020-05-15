#ifndef TABLE_H
#define TABLE_H
#include "define.h"

typedef struct Type *Type;
typedef struct Table *Table;
typedef struct Object *Object;

struct Object {
  char name[TEXTLEN + 1];
  int class, exno;
  int level;
  bool expo, rdo; /*exported / read-only*/
  Type type;
  Object next;
  Table dsc;
  long int value;
  Object chain;
};

struct Type {
  int form;
  int ref; /*ref is only used for import/export*/
  int mno;
  int nofpar; /*for procedures, extension level for records*/
  int len; /*for arrays, len < 0 => open array; for records: adr of descriptor*/
  int size; /*in bytes; always multiple of 4, except for Byte, Bool and Char*/
  Table dsc;
  Object typobj;
  Type base;/*for arrays, records, pointers*/
  Object dscobj;
};


#define SLOTSNUM 701

struct Table {
	Object slot[SLOTSNUM];
	Table dsc;
	Object first;
	Object last;
};

void hash_insert(Table SymbolTable, Object obj);
Object hash_search(Table SymbolTable, char *name);
Table hash_init();
void hash_free(Table SymbolTable);

#endif