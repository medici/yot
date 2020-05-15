#include <stdlib.h>
#include <stdio.h>
#include "table.h"
#include "declare.h"
#include "data.h"


int
hash(char *name) {
	unsigned int value = 0;
	unsigned int index = 1;

	for(; *name; ++name) {
		value += (*name) * index;
		index *= 10;
	}

	return value % SLOTSNUM;
}

void
hash_insert(Table SymbolTable, Object obj) {
	unsigned int index = hash(obj->name);

	Object chain = SymbolTable->slot[index];

	obj->chain = chain;

	SymbolTable->slot[index] = obj;
}

Object
hash_search(Table SymbolTable, char *name) {
	if (SymbolTable == NULL) {
		return NULL;
	}
	unsigned int index = hash(name);

	Object chain = SymbolTable->slot[index];
	Object obj = NULL;

	for(; chain; chain = chain->chain) {
		if(streq(name, chain->name)) {
			obj = chain;
			break;
		}
	}

	return obj;
}

// Object
// hash_pop() {
// 	int index = hash(SymbolTable->top->name);

// 	Object chain = SymbolTable->slot[index];
// 	Object obj = chain->next;

// 	SymbolTable->slot[index] = obj;

// 	SymbolTable->top = chain->prevtop;

// 	return chain;
// }


// static struct Object marksym = {"<mark>"};

// void
// hash_begin_scope() {
// 	hash_insert(&marksym);
// }

// void
// hash_end_scope() {
// 	Object obj = hash_pop();
// 	while(obj != &marksym) {
// 	 	free(obj);
// 		obj = hash_pop();
// 	}

// 	free(obj);
// }

void
hash_free(Table SymbolTable) {
	int i = 0, j = 0;
	Object obj, chain;

	for(; i < SLOTSNUM; i++) {
		chain = SymbolTable->slot[i];

		while (chain) {
			obj = chain;
			chain = chain->chain;
			free(obj);
		}
	}

	free(SymbolTable);
}

Table
hash_init() {
	Table SymbolTable = checked_malloc(sizeof(*SymbolTable));
	SymbolTable->dsc = NULL;
	SymbolTable->first = NULL;
	SymbolTable->last = NULL;

	int i = 0;
	for(; i < SLOTSNUM; i++) {
		SymbolTable->slot[i] = NULL;
	}

	return SymbolTable;
}