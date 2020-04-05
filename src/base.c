#include <stdlib.h>
#include <libgen.h>
#include "base.h"
#include "declare.h"

#define MaxTypTab 64

Type TypTab[MaxTypTab];

void out_type(FILE *ptr, Type tp);

Object
new_obj(int class) {
  Object new, x;
  new = checked_malloc(sizeof(*new));
  x = TopScope;

  while(x->next != NULL && strneq(x->next->name, Text)) {
     x = x->next;
  }
  
  if (x->next == NULL) {
    new = checked_malloc(sizeof(*new));
    copy_ident(new->name, Text);
    new->class = class;
    new->next = NULL;
    new->rdo = FALSE;
    new->dsc = NULL;
    x->next = new;
    return new;
  } else {
    mark("mult def: %s", Text);
    return x->next;
  }
}

Object
this_obj() {
  Object s, x;
  
  s = TopScope;
  while(1) {
    x = s->next;
    while(x != NULL && strneq(x->name, Text) && x != NULL) {
        x = x->next;
    }
    if (x != NULL) {
        return x;
    }
    if (s == Universe) {
        mark("undef: %s", Text);
        return x;
    }
    s = s->dsc;
  }
}

Object
this_import(Object mod) {
  Object obj;
  if (mod->rdo) {
    if (mod->name[0] != '\0') {
      obj = mod->dsc;
      while(obj != NULL && strneq(obj->name, Text)) {
        obj = obj->next;
      }
    } else {
      obj = NULL;
    }
  } else {
    obj = NULL;
  }
  return obj;
}

Object 
this_field(Type rec) {
  Object fld = rec->dsc;
  while (fld != NULL && strneq(fld->name, Text)) {
    fld = fld->next;
  }
  return fld;
}

void
open_scope() {
  Object s = checked_malloc(sizeof(*s));
  s->class = CHEAD;
  s->dsc = TopScope;
  s->next = NULL;
  TopScope = s;
}

void
close_scope() {
  TopScope = TopScope->dsc;
}

Object
this_module(char *name) {
  Object mod, obj, obj1;

  obj1 = TopScope;
  obj = obj1->next;

  while(obj != NULL && strneq(obj->name, name)) {
    obj1 = obj;
    obj = obj1->next;
  }

  if (obj == NULL) {
    mod = checked_malloc(sizeof(*mod));
    mod->class = CMOD;
    mod->rdo = FALSE;
    copy_ident(mod->name, name);
    mod->type = NoType;
    mod->dsc = NULL;
    obj1->next = mod;
    obj = mod;
  } else {
    mark("invalid import order: %s", name);
  }

  return obj;
}

Type
type2(int ref, int form, int size) {
	Type tp = checked_malloc(sizeof(*tp));
	tp->form = form;
	tp->size = size;
	tp->ref = ref;
	tp->base = NULL;

	return tp;
}

void
in_type(FILE *ptr, Object thismod, Type tp) {
  int class, form, nofpar, size;
  Object obj, parameter;
  fread(&form, sizeof(form), 1, ptr);
  tp->form = form;
  if (form == FARRAY) {
    tp->base = checked_malloc(sizeof(*(tp->base)));
    in_type(ptr, thismod, tp->base);
    fread(&(tp->len), sizeof(tp->len), 1, ptr);
    fread(&(tp->size), sizeof(tp->size), 1, ptr);
  } else if (form == FFUNC) {
    tp->base = checked_malloc(sizeof(*(tp->base)));
    in_type(ptr, thismod, tp->base);
    obj = NULL;
    nofpar = 0;
    fread(&class, sizeof(class), 1, ptr);
    while (class != 0) {
      parameter = checked_malloc(sizeof(*parameter));
      parameter->class = class;
      fread(&(parameter->rdo), sizeof(parameter->rdo), 1, ptr);
      parameter->type = checked_malloc(sizeof(*(parameter->type)));
      in_type(ptr, thismod, parameter->type);
      parameter->next = obj;

      obj = parameter;
      ++nofpar;
      fread(&class, sizeof(class), 1, ptr);
    }
    tp->dsc = obj;
    tp->nofpar = nofpar;
    tp->size = WORDSIZE;
  } else {
    fread(&size, sizeof(size), 1, ptr);
    tp->size = size;
  }
}

void
import(char *impid) {
  Object thismod;
  FILE *ptr;
  int class;
  char filename[TEXTLEN];
  int len = strlen(impid);
  copy_ident(filename, impid);
  filename[len] = '.';
  filename[len + 1] = 's';
  filename[len + 2] = 'f';
  filename[len + 3] = '\0';

  ptr = fopen(filename, "rb");
  thismod = this_module(basename(impid));
  thismod->rdo = TRUE;
  
  Object obj = checked_malloc(sizeof(*obj));
  fread(&class, sizeof(class), 1, ptr);
  while(class != 0) {
    obj = checked_malloc(sizeof(*obj));
    obj->class = class;
    fread(obj->name, sizeof(obj->name), 1, ptr);
    Type type = checked_malloc(sizeof(*(type)));
    in_type(ptr, obj, type);
    obj->type = type;
    
    if (obj->class == CCONST) {
      if (obj->type->form == FREAL || obj->type->form == FINTEGER) {
        fread(&(obj->value), sizeof(obj->value), 1, ptr);
      }
    }

    obj->next = thismod->dsc;
    thismod->dsc = obj;


    fread(&class, sizeof(class), 1, ptr);
  }

  fclose(ptr);
}

void
out_parameter(FILE *ptr, Object parameter, int nofpar) {
  int class;
  if (nofpar > 0) {
    out_parameter(ptr, parameter->next, nofpar - 1);
    class = parameter->class;
    fwrite(&(parameter->class), sizeof(parameter->class), 1, ptr);
    fwrite(&(parameter->rdo), sizeof(parameter->rdo), 1, ptr);
    out_type(ptr, parameter->type);
  }
}

void
out_type(FILE *ptr, Type tp) {
  int placehodler = 0;
  fwrite(&(tp->form), sizeof(tp->form), 1, ptr);

  if (tp->form == FARRAY) {
    out_type(ptr, tp->base);
    fwrite(&(tp->len), sizeof(tp->len), 1, ptr);
    fwrite(&(tp->size), sizeof(tp->size), 1, ptr);
  } else if (tp->form == FFUNC) {
    out_type(ptr, tp->base);
    out_parameter(ptr, tp->dsc, tp->nofpar);
    fwrite(&placehodler, sizeof(placehodler), 1, ptr);
  } else {
    fwrite(&(tp->size), sizeof(tp->size), 1, ptr);
  }
}

void
export(char *modid) {
  FILE *ptr;
  int placehodler = 0;
  char filename[TEXTLEN];
  int len = strlen(modid);
  copy_ident(filename, modid);
  Object obj = TopScope->next;
  strcat(filename, ".sf");
  ptr = fopen(filename, "wb");  // w for write, b for binary

  // class
  // name
  // form
  while(obj != NULL) {
    if (obj->expo) {
      // fwrite(obj, sizeof(*obj), 1, ptr); 
      // fwrite(obj->type, sizeof(*(obj->type)), 1, ptr);
      fwrite(&(obj->class), sizeof(obj->class), 1, ptr);
      fwrite(obj->name, sizeof(char), TEXTLEN + 1, ptr);
      out_type(ptr, obj->type);
      if (obj->class == CTYPE) {

      } else if (obj->class == CCONST) {
        if (obj->type->form == FREAL || obj->type->form == FINTEGER) {
          fwrite(&(obj->value), sizeof(obj->value), 1, ptr);
        }
      }
    }
    obj = obj->next;
  }
  fwrite(&placehodler, sizeof(placehodler), 1, ptr);

  fclose(ptr);
}

void
base_start() {
  TopScope = Universe;
}

void 
enter(char* name, int class, Type tp, long int n) {
    Object obj = checked_malloc(sizeof(*obj));
    obj->class = class;
    obj->value = n;
    copy_ident(obj->name, name);
    obj->type = tp;
    obj->dsc = NULL;
    obj->next = System;
    System = obj;
}

void
base_init() {
  BoolType = checked_malloc(sizeof(*BoolType));
  BoolType->form = FBOOLEAN;
  BoolType->size = 1;
  
  IntType = checked_malloc(sizeof(*IntType));
  IntType->form = FINTEGER;
  IntType->size = WORDSIZE;

  CharType = checked_malloc(sizeof(*CharType));
  CharType->form = FCHAR;
  CharType->size = 1;

  RealType = checked_malloc(sizeof(*RealType));
  RealType->form = FREAL;
  RealType->size = WORDSIZE;
  
  StringType = checked_malloc(sizeof(*StringType));
  StringType->form = FSTRING;
  StringType->size = WORDSIZE;
  
  NoType = checked_malloc(sizeof(*NoType));
  NoType->form = FNOTYPE;
  NoType->size = WORDSIZE;

  NilType = checked_malloc(sizeof(*NilType));
  NilType->form = FNILTYP;
  NilType->size = WORDSIZE;


  System = NULL;
  // type
  enter("boolean",CTYPE, BoolType, 0);
  enter("integer",CTYPE, IntType, 0);
  enter("char",CTYPE, CharType, 0);
  enter("string", CTYPE, StringType, 0);
  enter("real", CTYPE, RealType, 0);
  // stand proc
  enter("Write", CSPROC, NoType, 1);
  enter("WriteInt", CSPROC, NoType, 11);
  enter("WriteReal", CSPROC, NoType, 21);
  enter("WriteBool", CSPROC, NoType, 31);
  enter("WriteLn", CSPROC, NoType, 0);
  // stand func
  enter("Abs", CSFUNC, IntType, 1);
  enter("Floor", CSFUNC, IntType, 21);
  enter("Ord", CSFUNC, IntType, 41);
  enter("Chr", CSFUNC, CharType, 51);
  enter("Asr", CSFUNC, IntType, 82);
  TopScope = NULL;
  open_scope();
  TopScope->next = System;
  Universe = TopScope;
}