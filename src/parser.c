#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base.h"
#include "data.h"
#include "declare.h"
#include "define.h"
#include "generator.h"
#include "printt.h"
#include "scan.h"
#include "sys.h"

typedef struct PointerBase *PointerBase;

struct PointerBase {
  char name[TEXTLEN + 1];
  Type type;
  PointerBase next;
};

int exno;
int dc;

Object dummy;

PointerBase pointerBaseList;

Item expression();
Type type();
void param_list(Item x);

void stat_if() {
  // if-predicate
  // if-actions

  // while {
  //  if-fix
  //  if-predicate
  //  if-actions
  // }
  // if-else {
  //   if-fix
  //   if-actions
  // }
}

void increment_level(int n) { CurrentLevel += n; }

void check(int sym, char *msg) {
  if (Symbol == sym) {
    get();
  } else {
    mark(msg, Text);
  }
}

Object qualident() {
  Object obj = this_obj();

  get();

  if (obj == NULL) {
    mark("undef %s", Text);
    return dummy;
  }

  if (Symbol == PERIOD && obj->class == CMOD) {
    get();
    if (Symbol == IDENT) {
      obj = this_import(obj);
      get();
      if (obj == NULL) {
        mark("undef", NULL);
        return dummy;
      }
    } else {
      mark("%s identifier expected", Text);
      return dummy;
    }
  }

  return obj;
}

void check_bool(Item x) {
  if (x->type->form != FBOOLEAN) {
    mark("%s not boolean", x->name);
    x->type = BoolType;
  }
}

void check_integer(Item x) {
  if (x->type->form != FINTEGER) {
    mark("%s not integer", x->name);
    x->type = IntType;
  }
}

void check_real(Item x) {
  if (x->type->form != FREAL) {
    if (x->name[0] != '\0') {
      mark("%s not real", x->name);
    } else {
      char buf[32];
      sprintf(buf, "%Le", x->f);
      mark("%s not real", buf);
    }
    x->type = RealType;
  }
}

void check_const(Item x) {
  if (x->mode != CCONST) {
    mark("%s not a constant", x->name);
  }
  x->mode = CCONST;
}

void check_read_only(Item x) {
  if (x->rdo) {
    mark("%s read-only", x->name);
  }
}

bool check_export() {
  if (Symbol == TIMES) {
    get();
    if (CurrentLevel != 0) {
      mark("remove asterisk '%s'", Text);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

bool is_extension(Type t0, Type t1) {
  return (t0 == t1) || ((t1 != NULL) && is_extension(t0, t1->base));
}

Item convert_literal_int_to_real(Item x) {
  if (x->mode == CCONST && x->type->form == FINTEGER) {
    return make_real_item(x->a);
  }
  return x;
}

Item convert_literal_real_to_int(Item x) {
  if (x->mode == CCONST && x->type->form == FINTEGER) {
    x = make_real_item(x->a);
  }
  return x;
}

Item convert_int_to_real(Item x) {
  int form = x->type->form;

  if (form == FINTEGER) {
    if (x->mode == CCONST) {
      char buf[32];
      sprintf(buf, "%ld", x->a);
      mark("%s not real", buf);
      x = convert_literal_int_to_real(x);
    } else if (x->mode == CREG) {
      mark("expression not real", NULL);
      g_cvtsi2ss(x);
    } else if (x->mode == CVAR) {
      mark("%s not real", x->name);
      load(x);
      g_cvtsi2ss(x);
    }
  }

  return x;
}

Item convert_real_to_int(Item x) {
  int form = x->type->form;
  if (form == FREAL) {
    if (x->mode == CCONST) {
      char buf[32];
      sprintf(buf, "%Le", x->f);
      mark("%s not integer", buf);
      lg_cvtss2si(x);
    } else if (x->mode == CREG) {
      mark("expression not integer", NULL);
      g_cvtss2si(x);
    } else if (x->mode == CVAR) {
      mark("%s not integer", x->name);
      load(x);
      g_cvtss2si(x);
    }
  }

  return x;
}

void selector(Item x) {
  Item y;
  Object obj;
  while (Symbol == LBRAK || Symbol == PERIOD) {
    if (Symbol == LBRAK) {
      do {
        get();
        y = expression();
        if (x->type->form == FARRAY) {
          check_integer(y);
          arr_index(x, y);
          x->type = x->type->base;
        } else {
          mark("%s not an array", x->name);
        }
      } while (Symbol == COMMA);
      check(RBRAK, "no ]");
    } else if (Symbol == PERIOD) {
      get();
      if (Symbol == IDENT) {
        if (x->type->form == FPOINTER) {
          generator·dereference(x);
          x->type = x->type->base;
        }
        if (x->type->form == FRECORD) {
          obj = this_field(x->type);
          if (obj != NULL) {
            field(x, obj);
            x->type = obj->type;
          } else {
            mark("%s undef", Text);
          }
          get();
        }
      } else {
        mark("%s ident?", Text);
      }
    }
  }
}

Item stand_func(int fct, Type restyp) {
  Item x, y;
  int n, npar;

  check(LPAREN, "no (");
  npar = fct % 10;
  fct = fct / 10;

  x = expression();
  n = 1;

  while (Symbol == COMMA) {
    get();
    y = expression();
    ++n;
  }

  check(RPAREN, "no )");

  if (n == npar) {
    if (fct == 0) {
      if (x->type->form == FINTEGER || x->type->form == FREAL) {
        Abs(x);
        restyp = x->type;
      } else {
        mark("%s bad type", x->name);
      }
    } else if (fct == 2) {  // floor
      check_real(x);
      Floor(x);
    } else if (fct == 4) {  // Ord
      if (x->type->form <= FFUNC) {
        Ord(x);
      } else {
        mark("%s bad type", x->name);
      }
    } else if (fct == 5) {
      check_integer(x);
      Ord(x);
    } else if (fct == 8) {  // Asr
      check_integer(y);
      if (x->type->form == FINTEGER || x->type->form == FSET) {
        Shift(fct - 7, x, y);
        restyp = x->type;
      } else {
        mark("%s bad type", x->name);
      }
    } else if (fct == 17) {
      generator·Adr(x);
    } else if (fct == 18) {
      if (x->mode == CTYPE) {
        x = make_const_item(x->type->size, IntType);
      } else {
        mark("must be type", NULL);
      }
    } else if (fct == 30) {
      check_integer(x);
      generator·Brk(x);
    } else if (fct == 31) {
      check_integer(x);
      check_integer(y);
      generator·Free(x, y);
    }
    x->type = restyp;
  } else {
    mark("wrong nof params", NULL);
  }
  return x;
}

Item factor() {
  if (Symbol == IDENT) {
    Item x;

    Object obj = qualident();

    if (obj->class == CSFUNC) {
      x = stand_func(obj->value, obj->type);
    } else {
      x = make_item(obj);
      selector(x);

      if (Symbol == LPAREN) {
        get();
        if (x->type->form == FFUNC && x->type->base->form != FNOTYPE) {
          int *r = pre_call(x);
          param_list(x);
          call(x, r, x->name);
          x->type = x->type->base;
        } else {
          mark("%s not a function", x->name);
          param_list(x);
        }
      }
    }

    return x;
  } else if (Symbol == STRING) {
    Item x = make_strlit_item(Text);
    get();
    return x;
  } else if (Symbol == LPAREN) {
    get();
    Item x = expression();
    check(RPAREN, "not )");
    return x;
  } else if (Symbol == NOT) {
    Item x;
    get();
    x = factor();
    check_bool(x);
    // load_compare_expression(x);
    not(x);
    return x;
  } else if (Symbol == INTEGER) {
    get();
    return make_const_item(Value, IntType);
  } else if (Symbol == REAL) {
    get();
    return make_real_item(RValue);
  } else if (Symbol == CHAR) {
    get();
    return make_const_item(Value, CharType);
  } else if (Symbol == NIL) {
    get();
    return make_const_item(0, NilType);
  } else if (Symbol == FLS) {
    get();
    return make_const_item(0, BoolType);
  } else if (Symbol == TRU) {
    get();
    return make_const_item(1, BoolType);
  } else {
    mark("%s not a factor", Text);
    return make_const_item(0, IntType);
  }
}

Item term() {
  Item x, y;
  int form;
  int op;

  x = factor();
  form = x->type->form;
  while (Symbol >= TIMES && Symbol <= AND) {
    op = Symbol;
    get();
    if (op == TIMES) {
      if (form == FINTEGER) {
        y = factor();
        if (y->type->form == FREAL) {
          x = convert_int_to_real(x);
          form = x->type->form;
          check_real(x);
          real_op(op, x, y);
        } else {
          check_integer(y);
          mul_op(x, y);
        }
      } else if (form == FREAL) {
        y = factor();
        y = convert_int_to_real(y);
        check_real(y);
        real_op(op, x, y);
      } else {
        mark("%s bad type", x->name);
      }
    } else if (op == DIV || op == MOD) {
      x = convert_real_to_int(x);
      y = factor();
      y = convert_real_to_int(y);
      div_op(op, x, y);
    } else if (op == RDIV) {
      x = convert_int_to_real(x);
      form = x->type->form;
      y = factor();
      y = convert_int_to_real(y);
      real_op(op, x, y);
    } else {
      check_bool(x);
      and1(x);
      y = factor();
      check_bool(y);
      and2(x, y);
    }
  }

  // if (op == AND) {
  //     Fixup(x->a);
  // }

  return x;
}

Item simple_expression() {
  Item x, y;
  int op;
  if (Symbol == MINUS) {
    get();
    x = term();
    neg(x);
  } else if (Symbol == PLUS) {
    get();
    x = term();
  } else {
    x = term();
  }

  while (Symbol >= PLUS && Symbol <= OR) {
    op = Symbol;
    get();
    if (op == OR) {
      or1(x);
      check_bool(x);
      y = term();
      check_bool(y);
      or2(x, y);
    } else if (x->type->form == FINTEGER) {
      y = term();
      if (y->type->form == FREAL) {
        x = convert_int_to_real(x);
        check_real(x);
        real_op(op, x, y);
      } else {
        check_integer(y);
        add_op(op, x, y);
      }
    } else if (x->type->form == FREAL) {
      y = term();
      y = convert_int_to_real(y);
      check_real(y);
      real_op(op, x, y);
    }
  }

  return x;
}

Item expression() {
  int rel;
  Item x = simple_expression();

  Item y;
  if (Symbol >= EQL && Symbol <= GTR) {
    rel = Symbol;
    get();
    y = simple_expression();

    if (x->type == y->type) {
      if (x->type->form == FINTEGER || x->type->form == FCHAR) {
        int_relation(rel, x, y);
      } else if (x->type->form == FREAL) {
        real_relation(rel, x, y);
      } else if (x->type->form == FBOOLEAN) {
        if (rel <= NEQ) {
          int_relation(rel, x, y);
        } else {
          mark("only = or #", NULL);
        }
      } else {
        mark("illegal comparison", NULL);
      }
    } else if ((x->type->form == FPOINTER && y->type->form == FNILTYPE) ||
               (y->type->form == FPOINTER && x->type->form == FNILTYPE)) {
      if (rel <= NEQ && rel >= EQL) {
        int_relation(rel, x, y);
      } else {
        mark("only = or #", NULL);
      }
    } else if (x->type->form == FCHAR && y->type->form == FSTRING &&
               y->b == 2) {
      str_to_char(y);
      int_relation(rel, x, y);
    } else if (y->type->form == FCHAR && x->type->form == FSTRING &&
               x->b == 2) {
      str_to_char(x);
      int_relation(rel, x, y);
    } else {
      printf("no type\n");
    }

    x->type = BoolType;
  }

  return x;
}

bool compare_types(Type t0, Type t1, bool varpar) {
  return (t0->form == t1->form && t0->form != FARRAY) ||
         (t0->form == FRECORD && t1->form == FRECORD && is_extension(t0, t1)) ||
         (!varpar && (t0->form == FPOINTER && t1->form == FPOINTER &&
                      is_extension(t0->base, t1->base)));
}

void parameter(Object par) {
  bool varpar;
  Item x;

  x = expression();
  if (par != NULL) {
    varpar = par->class == CPAR;
    if (compare_types(par->type, x->type, varpar)) {
      if (!varpar) {
        value_param(x);
      } else {
        if (!par->rdo) {
          check_read_only(x);
        }
        var_param(x, par->type);
      }
    } else if (x->type->form == FARRAY && par->type->form == FARRAY &&
               x->type->base->form == par->type->base->form &&
               par->type->len < 0) {
      if (!par->rdo) {
        check_read_only(x);
      }
      open_array_param(x);
    } else if (x->type->form == FSTRING && par->type->form == FARRAY &&
               par->type->base->form == FCHAR && par->type->len < 0) {
      string_parameter(x);
    } else {
      // value_param(x);
      mark("incompatible parameters", NULL);
    }
  }
}

void param_list(Item x) {
  int n = 0;
  Object par;
  // if (x->type->dsc == NULL) {
  //     par = checked_malloc(sizeof(*par));
  //     x->type->dsc = par;
  // } else {
  par = x->type->dscobj;
  //}

  if (Symbol != RPAREN) {
    parameter(par);
    n = 1;
    while (Symbol == COMMA) {
      check(COMMA, ", missing");
      if (par != NULL) {
        par = par->next;
      }
      ++n;
      parameter(par);
    }
    check(RPAREN, ") missing");

  } else {
    get();
  }

  if (n < x->type->nofpar) {
    mark("too few params", NULL);
  } else if (n > x->type->nofpar) {
    mark("too many params", NULL);
  }
}

void stand_proc(int pno) {
  int nap, npar;  // nof actual/formal parameters
  Item x, y, z;

  npar = pno % 10;
  pno = pno / 10;
  if (npar == 0) {
    if (pno == 0) {
      // if (Symbol == LPAREN) {
      //     get();
      //     check(RPAREN, "no )");
      // }
      // call_write_ln();
    }
  } else {
    check(LPAREN, "no (");
    x = expression();
    nap = 1;
    if (Symbol == COMMA) {
      get();
      y = expression();
      nap = 2;
    }
    check(RPAREN, "no )");

    if (npar == nap) {
      if (pno == 0) {
        int *r = pre_call(x);
        value_param(x);
        Write(r);
      } else if (pno == 10) {
        check_integer(x);
        generator·Get(x, y);
      } else if (pno == 11) {
        check_integer(x);
        generator·Put(x, y);
      }
      // else if (pno == 1) {
      //     int* r = pre_call(x);
      //     check_integer(x);
      //     value_param(x);
      //     call_write_int(r);
      // } else if (pno == 2) {
      //     int* r = pre_call(x);
      //     check_real(x);
      //     value_param(x);
      //     call_write_real(r);
      // } else if(pno == 3) {
      //     int* r = pre_call(x);
      //     check_bool(x);
      //     value_param(x);
      //     call_write_bool(r);
      // }
    }
  }
}

void stat_sequence() {
  Object obj;
  Item x, y;
  while (Symbol <= SEMICOLON) {
    if (!((Symbol >= IDENT && Symbol <= FOR) || Symbol >= SEMICOLON)) {
      mark("%s statement expected", Text);
      do {
        get();
      } while (Symbol < IDENT);
    }
    if (Symbol == IDENT) {
      obj = qualident();

      x = make_item(obj);
      if (x->mode == CSPROC) {
        stand_proc(obj->value);
      } else {
        selector(x);
        if (Symbol == BECOMES) {
          check_read_only(x);
          get();
          y = expression();
          if (x->type->form == FINTEGER) {
            y = convert_real_to_int(y);
            store(x, y);
          } else if (x->type->form == FREAL) {
            y = convert_int_to_real(y);
            store(x, y);
          } else if (x->type->form == FBOOLEAN) {
            // load_compare_expression(y);
            store(x, y);
          } else if (x->type->form == FCHAR && y->type->form == x->type->form) {
            store(x, y);
          } else if (x->type->form == FARRAY && x->type->base->form == FCHAR &&
                     y->type->form == FSTRING) {
            copy_string(x, y);
          } else if (x->type->form == FCHAR && y->type->form == FSTRING &&
                     y->b == 2) {
            str_to_char(y);
            store(x, y);
          } else if (x->type->form == FFUNC) {
            printf("sss\n");
          } else if (x->type->form == FPOINTER) {
            compare_types(x->type, y->type, FALSE);
            store(x, y);
          } else {
            mark("Illegal expression", NULL);
          }

        } else if (Symbol == LPAREN) {
          get();
          int *r = pre_call(x);
          param_list(x);
          call(x, r, x->name);
        } else if (Symbol == EQL) {
          mark("= should be :=", NULL);
          get();
          y = expression();
        } else if (x->type->form == FFUNC) {
          if (x->type->nofpar > 0) {
            mark("%s missing parameters", x->name);
          }

          if (x->type->base->form == FNOTYPE) {
            int *r = pre_call(x);
            call(x, r, x->name);
          }
        }
      }
    } else if (Symbol == IF) {
      long int L = 0;

      get();
      x = expression();
      check_bool(x);
      CFJump(x);
      check(THEN, "no then");

      stat_sequence();

      while (Symbol == ELSIF) {
        L = FJump(L);
        Fixup(x->a);

        get();
        x = expression();
        check_bool(x);
        CFJump(x);
        check(THEN, "no then");

        stat_sequence();
      }
      if (Symbol == ELSE) {
        L = FJump(L);
        Fixup(x->a);

        get();
        stat_sequence();
      } else {
        Fixup(x->a);
      }
      fixLink(L);
      check(END, "no end");
    } else if (Symbol == REPEAT) {
      get();
      long int L0 = label();
      glab(L0);
      stat_sequence();
      if (Symbol == UNTIL) {
        get();
        x = expression();
        check_bool(x);
        CBJump(x, L0);
      } else {
        mark("missing until", NULL);
      }
    } else if (Symbol == WHILE) {
      get();
      long int L0 = label();
      glab(L0);
      x = expression();
      check_bool(x);
      CFJump(x);
      check(DO, "no do");
      stat_sequence();
      BJump(L0);
      while (Symbol == ELSIF) {
        get();
        Fixup(x->a);
        x = expression();
        check_bool(x);
        CFJump(x);
        check(DO, "no do");
        stat_sequence();
        BJump(L0);
      }
      Fixup(x->a);
      check(END, "no end");
    } else if (Symbol == FOR) {
      get();
      if (Symbol == IDENT) {
        Item z, w;
        obj = qualident();
        x = make_item(obj);
        check_integer(x);
        check_read_only(x);
        long int L0, L1;
        if (Symbol == BECOMES) {
          get();
          y = expression();
          check_integer(y);
          For0(x, y);
          L0 = label();
          L1 = 0;
          check(TO, "no to");
          z = expression();
          check_integer(z);
          obj->rdo = TRUE;
          if (Symbol == BY) {
            get();
            w = expression();
            check_const(w);
            check_integer(w);
          } else {
            w = make_const_item(1, IntType);
          }
          check(DO, "no do");
          // Fixup(L0);
          glab(L0);
          L1 = For1(x, y, z, w, L1);
          stat_sequence();
          check(END, "no end");
          For2(x, y, w);
          BJump(L0);
          Fixup(L1);
          obj->rdo = FALSE;
        }
      } else {
        mark("identifier expected", NULL);
      }
    }
    check_regs();
    if (Symbol == SEMICOLON) {
      get();
    } else if (Symbol < SEMICOLON) {
      mark("missing semicolon?", NULL);
    }
  }
}

Type formal_type(char *typename) {
  Object obj;
  if (Symbol == IDENT) {
    obj = qualident();
    if (obj->class == CTYPE) {
      return obj->type;
    } else {
      mark("%s type?", typename);
      return IntType;
    }
  } else if (Symbol == ARRAY) {
    get();
    check(OF, "of ?");
    Type tp = checked_malloc(sizeof(*tp));
    tp->form = FARRAY;
    tp->len = -1;
    tp->size = 2 * WORDSIZE;
    tp->base = formal_type(Text);
    return tp;
  }

  return NoType;
}

Object ident_list(int class) {
  Object obj;
  Object first;

  if (Symbol == IDENT) {
    first = new_obj(class);
    get();
    first->expo = check_export();
    while (Symbol == COMMA) {
      get();
      if (Symbol == IDENT) {
        obj = new_obj(class);
        get();
        obj->expo = check_export();
      } else {
        mark("%s ident? ", Text);
      }
    }
    if (Symbol == COLON) {
      get();
    } else {
      mark("%s :?", Text);
    }
  }
  return first;
}

void array_type(Type tp) {
  Item x;
  int len;

  x = expression();

  if (x->mode == CCONST && x->type->form == FINTEGER && x->a >= 0) {
    len = x->a;
  } else {
    len = 1;
    mark("%s not a valid length", Text);
  }

  if (Symbol == OF) {
    get();
    tp->base = type();
    if (tp->base->form == FARRAY && tp->base->len < 0) {
      mark("%s dyn array not allowed", Text);
    }
  } else if (Symbol == COMMA) {
    get();
    tp->base = checked_malloc(sizeof(*(tp->base)));
    array_type(tp->base);
  } else {
    mark("%s missing of", Text);
    tp->base = IntType;
  }
  tp->size = (len * tp->base->size + (WORDSIZE - 1)) / WORDSIZE * WORDSIZE;
  tp->form = FARRAY;
  tp->len = len;
}

void record_type(Type typ) {
  int offset = 0, n, off;
  Object bot, obj, obj0, new;
  Type tp;

  typ->form = FNOTYPE;
  typ->base = NULL;
  typ->nofpar = 0;
  typ->dsc = hash_init();
  bot = NULL;

  while (Symbol == IDENT) {
    n = 0;
    obj = bot;
    while (Symbol == IDENT) {
      obj0 = obj;
      while (obj0 != NULL && strneq(obj0->name, Text)) {
        obj0 = obj0->next;
      }
      if (obj0 != NULL) {
        mark("%s mult def", obj0->name);
      }
      new = checked_malloc(sizeof(*new));
      copy_ident(new->name, Text);
      new->class = CFLD;
      new->next = obj;
      obj = new;
      ++n;
      get();
      new->expo = check_export();
      if (Symbol != COMMA && Symbol != COLON) {
        mark("comma expected", NULL);
      } else if (Symbol == COMMA) {
        get();
      }
    }
    check(COLON, "colon expected");
    tp = type();
    if (tp->form == FARRAY && tp->len < 0) {
      mark("dyn array not allowed", NULL);
    }
    if (tp->size > 1) {
      offset = (offset + (WORDSIZE - 1)) / WORDSIZE * WORDSIZE;
    }
    offset = offset + n * tp->size;
    off = offset;
    obj0 = obj;
    while (obj0 != bot) {
      obj0->type = tp;
      obj0->level = 0;
      off = off - tp->size;
      obj0->value = off;
      hash_insert(typ->dsc, obj0);
      obj0 = obj0->next;
    }
    bot = obj;
    if (Symbol == SEMICOLON) {
      get();
    } else if (Symbol != END) {
      mark("; or end", NULL);
    }
  }
  typ->form = FRECORD;
  typ->dscobj = bot;
  typ->size = (offset + (WORDSIZE - 1)) / WORDSIZE * WORDSIZE;
}

void fp_section(int *adr, int *nofpar, Table table) {
  Object obj, first;
  Type tp = IntType;
  int parsize;
  int class;
  bool rdo;

  if (Symbol == VAR) {
    get();
    class = CPAR;
  } else {
    class = CVAR;
  }
  first = ident_list(class);

  tp = formal_type(Text);

  rdo = FALSE;

  if (class == CVAR && tp->form >= FARRAY) {
    class = CPAR;
    rdo = TRUE;
  }

  if ((tp->form == FARRAY && tp->len < 0) || tp->form == FRECORD) {
    parsize = 2 * WORDSIZE; /*open array or record, needs second word for length
                               or type tag*/
  } else if (class == CPAR) {
    parsize = WORDSIZE;
  } else {
    parsize = tp->size;
  }

  obj = first;

  while (obj != NULL) {
    ++(*nofpar);
    obj->class = class;
    obj->type = tp;
    obj->rdo = rdo;
    obj->level = CurrentLevel;
    *adr += parsize;
    obj->value = -*adr;
    hash_insert(table, obj);
    obj = obj->next;
  }
}

void func_type(Type ptype, int *parblksize) {
  Object obj, first = NULL;
  int size;
  int nofpar;

  ptype->base = NoType;
  size = *parblksize;
  nofpar = 0;
  ptype->dsc = hash_init();

  if (Symbol == LPAREN) {
    get();
    if (Symbol == RPAREN) {
      get();
    } else {
      fp_section(&size, &nofpar, ptype->dsc);
      while (Symbol == SEMICOLON) {
        get();
        fp_section(&size, &nofpar, ptype->dsc);
      }
      check(RPAREN, "%s )?");
    }

    if (Symbol == COLON) {
      get();
      if (Symbol == IDENT) {
        obj = qualident();
        ptype->base = obj->type;
        if (!(obj->class == CTYPE && obj->type->form <= FFUNC)) {
          mark("illegal function type: %s", Text);
        }
      } else {
        mark("type identifier expected, but get %s", Text);
      }
    }
  }

  ptype->nofpar = nofpar;
  *parblksize = (size + WORDSIZE - 1) / WORDSIZE * WORDSIZE;
}

void check_rec_level(int level) {
  if (level != 0) {
    mark("ptr base must be global", NULL);
  }
}

Type type() {
  Object obj, first;
  Item x;
  Type tp;
  PointerBase pointerBase;

  obj = checked_malloc(sizeof(*obj));

  if (Symbol == IDENT) {
    obj = qualident();

    if (obj->class == CTYPE) {
      tp = obj->type;
      if (obj->type != NULL && obj->type->form != FNOTYPE) {
        tp = obj->type;
      }
    } else {
      mark("%s not a type or undefined", Text);
    }
  } else if (Symbol == ARRAY) {
    tp = checked_malloc(sizeof(*tp));
    get();
    array_type(tp);
  } else if (Symbol == RECORD) {
    tp = checked_malloc(sizeof(*tp));
    get();
    record_type(tp);
    check(END, "no end");
  } else if (Symbol == POINTER) {
    get();
    check(TO, "no to");
    tp = checked_malloc(sizeof(*tp));
    tp->form = FPOINTER;
    tp->size = WORDSIZE;
    tp->base = IntType;
    if (Symbol == IDENT) {
      obj = this_obj();
      if (obj != NULL) {
        if (obj->class == CTYPE &&
            (obj->type->form == FRECORD || obj->type->form == FNOTYPE)) {
          check_rec_level(obj->level);
          tp->base = obj->type;
        } else if (obj->class == CMOD) {
          mark("external base type not implemented", NULL);
        } else {
          mark("no valid base type", NULL);
        }
      } else {
        /*enter into list of forward references to be fixed in Declarations*/
        check_rec_level(CurrentLevel);
        pointerBase = checked_malloc(sizeof(*pointerBase));
        copy_ident(pointerBase->name, Text);
        pointerBase->type = tp;
        pointerBase->next = pointerBaseList;
        pointerBaseList = pointerBase;
      }
      get();
    } else {
      tp->base = type();
      if (tp->base->form != FRECORD || tp->base->typobj == NULL) {
        mark("must point to named record", NULL);
      }
      check_rec_level(CurrentLevel);
    }
  } else {
    mark("%s illegal type", Text);
  }

  return tp;
}

int declarations(int locblksize) {
  Object obj, first;
  Item x;
  Type tp;

  first = checked_malloc(sizeof(*first));
  tp = checked_malloc(sizeof(*tp));

  get();
  if (Symbol == CONST) {
    get();
    while (Symbol == IDENT) {
      obj = new_obj(CCONST);
      get();
      obj->expo = check_export();
      obj->rdo = TRUE;
      if (Symbol == EQL) {
        get();
      } else {
        mark("%s = ?", Text);
      }
      x = expression();
      if (x->mode == CCONST) {
        obj->level = x->b;
        obj->value = x->a;
        if (x->type->form == FSTRING && x->b == 2) {
          str_to_char(x);
          obj->value = x->a;
        } else if (x->type == RealType) {
          obj->level = CurrentLevel;
          obj->value = real_index(obj->value);
        }
        obj->type = x->type;
      } else {
        mark("%s expression not constant", x->name);
        obj->type = IntType;
      }
      check(SEMICOLON, "; missing");
    }
  }
  if (Symbol == TYPE) {
    get();
    while (Symbol == IDENT) {
      obj = new_obj(CTYPE);
      get();
      obj->expo = check_export();
      if (Symbol == EQL) {
        get();
      } else {
        mark("%s = ?", Text);
      }
      tp = type();
      obj->type = tp;
      obj->level = CurrentLevel;
      if (tp->typobj == NULL) {
        tp->typobj = obj;
      }

      check(SEMICOLON, "; missing");
    }
  }

  if (Symbol == VAR) {
    get();
    while (Symbol == IDENT) {
      first = ident_list(CVAR);

      tp = type();
      obj = first;

      while (obj != NULL) {
        obj->type = tp;
        obj->level = CurrentLevel;
        // if (tp->size > 1) {
        //   locblksize = (locblksize + (WORDSIZE - 1)) / WORDSIZE * WORDSIZE;
        // }
        locblksize += obj->type->size;
        obj->value = -locblksize;
        if (obj->expo) {
          obj->exno = exno;
          ++exno;
        }
        obj = obj->next;
      }
      if (Symbol == SEMICOLON) {
        get();
      } else {
        mark("%s ;?", Text);
      }
    }
  }

  locblksize = (locblksize + (WORDSIZE - 1)) / WORDSIZE * WORDSIZE;

  return locblksize;
}

void module_declarations() {
  Object obj, first;
  Item x;
  Type tp;
  PointerBase pointerBase;

  pointerBaseList = NULL;

  first = checked_malloc(sizeof(*first));
  tp = checked_malloc(sizeof(*tp));

  if (Symbol == CONST) {
    get();
    gdata();
    while (Symbol == IDENT) {
      obj = new_obj(CCONST);

      get();
      obj->expo = check_export();
      obj->rdo = TRUE;
      if (Symbol == EQL) {
        get();
      } else {
        mark("%s = ?", Text);
      }
      x = expression();
      if (x->mode == CCONST) {
        if (x->type->form == FSTRING && x->b == 2) {
          str_to_char(x);
        }
        obj->value = x->a;
        obj->level = x->b;
        obj->type = x->type;
      } else {
        mark("%s expression not constant", x->name);
        obj->type = IntType;
      }
      if (obj->expo) {
        gpublic(obj->name);
        g_name(obj->name);
        if (x->type->form == FINTEGER) {
          g_def_quad(obj->value);
        } else if (x->type->form == FREAL) {
          g_def_IEEE754_float(obj->value);
        } else if (x->type->form == FBOOLEAN || x->type->form == FCHAR) {
          g_def_byte(obj->value);
        }
      } else {
        if (x->type->form == FREAL) {
          g_name(obj->name);
          g_def_IEEE754_float(obj->value);
        }
      }

      check(SEMICOLON, "; missing");
    }
  }

  if (Symbol == TYPE) {
    get();
    while (Symbol == IDENT) {
      obj = new_obj(CTYPE);
      get();
      obj->expo = check_export();
      if (Symbol == EQL) {
        get();
      } else {
        mark("%s = ?", Text);
      }
      tp = type();
      obj->type = tp;
      obj->level = CurrentLevel;
      if (tp->typobj == NULL) {
        tp->typobj = obj;
      }
      if (tp->form == FRECORD) {
        /*check whether this is base of a pointer type; search and fixup*/
        pointerBase = pointerBaseList;
        while (pointerBase != NULL) {
          if (streq(obj->name, pointerBase->name)) {
            pointerBase->type->base = obj->type;
          }
          pointerBase = pointerBase->next;
        }
      }
      check(SEMICOLON, "; missing");
    }
  }
  if (Symbol == VAR) {
    get();
    gdata();
    while (Symbol == IDENT) {
      first = ident_list(CVAR);
      tp = type();
      obj = first;
      while (obj != NULL) {
        obj->type = tp;
        obj->level = CurrentLevel;

        if (obj->expo) {
          gpublic(obj->name);
          obj->exno = exno;
          ++exno;
        }
        g_name(obj->name);
        if (tp->form == FINTEGER) {
          g_def_quad(obj->value);
        } else if (tp->form == FREAL) {
          g_def_IEEE754_float(obj->value);
        } else if (tp->form == FBOOLEAN || tp->form == FCHAR) {
          g_def_byte(obj->value);
        } else if (tp->form == FRECORD) {
          int len = tp->size / WORDSIZE;
          int i;
          for (i = 0; i < len; i++) {
            g_def_quad(0);
          }
        } else if (tp->form == FARRAY) {
          int len = tp->len * tp->base->size / WORDSIZE;
          int i;
          for (i = 0; i < len; i++) {
            g_def_quad(0);
          }
        } else if (tp->form == FPOINTER) {
          g_def_quad(obj->value);
        }

        obj = obj->next;
      }
      if (Symbol == SEMICOLON) {
        get();
      } else {
        mark("%s ;?", Text);
      }
    }
  }

  pointerBase = pointerBaseList;
  while (pointerBase != NULL) {
    if (pointerBase->type->base->form == FINTEGER) {
      mark("undefined pointer base of", NULL);
    }
    pointerBase = pointerBase->next;
  }
}

void func_decl() {
  const int marksize = 8;
  Object proc, obj;
  Type tp;
  char procid[TEXTLEN + 1];
  int locblksize = 0, parblksize = 0;
  bool interrupt;
  Item x;

  interrupt = FALSE;
  get();
  if (Symbol == IDENT) {
    copy_ident(procid, Text);
    proc = new_obj(CCONST);

    get();
    if (interrupt) {
      parblksize = marksize * 3;
    } else {
      parblksize = 0;
    }
    tp = checked_malloc(sizeof(*tp));
    tp->form = FFUNC;
    tp->size = WORDSIZE;
    copy_ident(proc->name, procid);
    proc->type = tp;
    proc->value = -1;
    proc->level = CurrentLevel;
    proc->expo = check_export();

    if (proc->expo) {
      proc->exno = exno;
      ++exno;
    }

    open_scope();
    increment_level(1);
    tp->base = NoType;
    func_type(tp, &parblksize);
  }

  locblksize = parblksize;
  proc->type->dscobj = TopScope->first;
  if (Symbol == SEPARATOR) {
    locblksize = declarations(locblksize);

    proc->value = PC;
    proc->type->dscobj = TopScope->first;
    while (Symbol == FUNC) {
      func_decl();
      if (Symbol == SEMICOLON) {
        get();
      } else {
        mark("%s ;?", Text);
      }
    }

    proc->type->dscobj = TopScope->first;
    tp->dscobj = TopScope->first;

    if (Symbol == SEPARATOR) {
      get();
    } else {
      mark("%s |?", Text);
    }
  }

  prologue(proc, parblksize, locblksize);
  // gtext();
  // if(CurrentLevel == 1) {
  //     gpublic(procid);
  // }
  // g_name(procid);
  // gentry();
  // gstack(-locblksize);

  if (Symbol != END) {
    stat_sequence();
  }

  if (Symbol == RETURN) {
    get();
    x = expression();
    if (tp->base == NoType) {
      mark("%s is not a function", proc->name);
    } else if (!compare_types(tp->base, x->type, FALSE)) {
      mark("%s wrong result type", x->name);
    }
  } else if (tp->base->form != FNOTYPE) {
    mark("function without result", NULL);
    tp->base = NoType;
  }

  Return(tp->base->form, x, interrupt);

  epilogue(locblksize);
  // gstack(locblksize);
  // gexit();
  close_scope();
  increment_level(-1);
  check(END, "%s end?");
  if (Symbol == END) {
    get();
  }
}

void module() {
  char modid[TEXTLEN];
  char impid[TEXTLEN];
  Object objdsc = NULL;
  open();
  get();
  if (Symbol == MODULE) {
    get();
    base_start();

    open_scope();
    copy_ident(modid, CurrentDirectory);
    strcat(modid, Text);
    if (Symbol == IDENT) {
      get();
    } else {
      mark("identifier expected", NULL);
    }
    exno = 1;
    dc = 0;
    CurrentLevel = 0;
    if (Symbol == IMPORT) {
      get();
      while (Symbol == IDENT) {
        char ofile[TEXTLEN];
        copy_ident(impid, CurrentDirectory);
        copy_ident(ofile, CurrentDirectory);
        strcat(impid, Text);
        strcat(ofile, Text);
        strcat(ofile, ".o");
        if (access(ofile, F_OK) != 0) {
          copy_ident(impid, SCCDIR);
          copy_ident(ofile, SCCDIR);
          strcat(impid, "/lib/");
          strcat(ofile, "/lib/");
          strcat(impid, Text);
          strcat(ofile, Text);
          strcat(ofile, ".o");

          if (access(ofile, F_OK) != 0) {
            mark("%s not exist", ofile);
            exit(0);
          }
        } else {
          collect(strdup(ofile));
        }

        get();
        import(impid);
        if (Symbol == COMMA) {
          get();
        } else if (Symbol == IDENT) {
          mark("comma missing", NULL);
        }
      }

      check(SEMICOLON, "no ;");
    }

    module_declarations();

    while (Symbol == FUNC) {
      func_decl();
      if (Symbol == SEMICOLON) {
        get();
      } else {
        mark("%s ;?", Text);
      }
    }

    check(END, "no module end");
    if (Symbol != PERIOD) {
      mark("period missing", NULL);
    }
    export(modid);

    // todo free
    pointerBaseList = NULL;
  }
}

void parser_init() {
  dummy = checked_malloc(sizeof(*dummy));
  dummy->class = CVAR;
  dummy->type = IntType;
}