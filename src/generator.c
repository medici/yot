#include "generator.h"

#include <ctype.h>

#include "data.h"
#include "declare.h"
#include "define.h"
#include "generate_code.h"
#include "parser.h"
#include "sys.h"

#define MAX_REALX 2400
#define MAX_STRX 2400

int RH;   // normal, integer
int RHF;  // float

int realx = 0;
int strx = 0;
long int Real[MAX_REALX];
char Str[MAX_STRX];

void incR() { ++RH; }

void incFPUR() { ++RHF; }

void open() {
  PC = 0;
  RH = 0;
  RHF = 0;
}

void gdata(void) {
  if (Textseg) gc_data();
  Textseg = 0;
}

void gtext() {
  if (!Textseg) gc_text();
  Textseg = 1;
}

void gentry() { gc_entry(); }

void gstack(int n) {
  if (n) {
    gtext();
    gc_stack(n);
  }
}

void gparamters(int nofpar, Object params) {
  int i = 0;
  int j = 0;
  int f = 0;
  int offset = 0;
  char *destination;

  for (; i < nofpar; ++i) {
    if (params->type->form == FINTEGER) {
      offset -= params->type->size;
      destination = gc_relative_adr(offset);
      gc_store_reg(j, destination, params->type->size, params->type->form);
      ++j;
    } else if (params->type->form == FREAL) {
      if (params->class == CVAR) {
        offset -= params->type->size;
        // gc_store_regss(f, offset);
        destination = gc_relative_adr(offset);
        gc_store_reg(f, destination, params->type->size, params->type->form);
        ++f;
      } else {
        offset -= params->type->size;
        // gc_store_reg(j, offset);
        destination = gc_relative_adr(offset);
        gc_store_reg(j, destination, IntType->size, IntType->form);
        ++j;
      }

    } else if (params->type->form == FARRAY || params->type->form == FRECORD) {
      offset -= WORDSIZE * 2;
      destination = gc_relative_adr(offset);
      gc_store_reg(j, destination, params->type->size, params->type->form);
      destination = gc_relative_adr(offset + WORDSIZE);
      gc_store_reg(j + 1, destination, params->type->size, params->type->form);
      // gc_store_reg(j, offset);
      // gc_store_reg(j + 1, offset + WORDSIZE);
      ++j;
      ++j;
    } else if (params->type->form == FBOOLEAN || params->type->form == FCHAR) {
      if (params->class == CVAR) {
        offset -= params->type->size;
        // offset -= WORDSIZE;
        // gc_store_regb(j, offset);
        destination = gc_relative_adr(offset);
        gc_store_reg(j, destination, params->type->size, params->type->form);
      } else {
        offset -= WORDSIZE;
        // gc_store_reg(j, offset);
        destination = gc_relative_adr(offset);
        gc_store_reg(j, destination, IntType->size, IntType->form);
      }
      ++j;
    }
    params = params->next;
  }
}

void gexit(void) {
  gtext();
  gc_exit();
}

int rh() { return RH; }
int rhf() { return RHF; }

void check_regs() {
  if (RH != 0 || RHF != 0) {
    mark("reg stack", NULL);
    printf("RH %d, RHF %d\n", RH, RHF);
    RH = 0;
    RHF = 0;
  }
}

char *g_symbol(char *s) {
  static char name[TEXTLEN + 2];

  name[0] = PREFIX;
  copy_ident(&name[1], s);
  return name;
}

void graw(char *s) {
  if (NULL == Outfile) return;
  fprintf(Outfile, "%s", s);
}

void g_name(char *name) {
  graw(g_symbol(name));
  graw(":");
}

void gpublic(char *procid) { gc_public(g_symbol(procid)); }

void glab(int id) {
  if (NULL == Outfile) return;
  fprintf(Outfile, "%c%d:\n", LPREFIX, id);
}

void g_def_string(char *s, int len) {
  int i;
  gdata();
  for (i = 0; i < len; i++) {
    gc_def_byte(s[i] + 0);
  }
}

void g_def_byte(int v) {
  gdata();
  gc_def_byte(v);
}

void g_def_quad(int v) {
  gdata();
  gc_def_quad(v);
}

void g_def_IEEE754_float(long int v) { gc_def_long(v); }

// void
// gcstorlw(int n) {
//     ng("%s\t%%rax,%d(%%rbp)", "movq", n, NULL);
// }

void galign(int k) {
  gdata();
  while (k++ % INTSIZE) gc_def_byte(0);
}

void g_real() {
  if (realx > 0) {
    gdata();
    graw(g_symbol(REAL_LABEL));
    graw(":");
    fputc('\n', Outfile);
    int i = 0;
    for (; i < realx; i++) {
      gc_def_long(Real[i]);
    }
  }
}

void g_str() {
  if (strx > 0) {
    gdata();
    graw(g_symbol(STR_LABEL));
    graw(":");
    fputc('\n', Outfile);
    int i = 0;
    for (; i < strx; i++) {
      gc_def_byte(Str[i]);
    }

    int mod = WORDSIZE - strx % WORDSIZE;

    i = 0;

    for (; i < mod; i++) {
      gc_def_byte(0);
    }
  }
}

void gprelude(void) {
  gc_prelude();
  Textseg = 0;
  gtext();
}

void gpostlude(void) {
  g_real();
  g_str();
  gc_postlude();
}

int real_index(long int value) {
  Real[realx] = value;
  ++realx;

  return realx - 1;
}

void store_str(char ch) {
  Str[strx] = ch;
  ++strx;
}

void g(char *s) {
  if (NULL == Outfile) return;
  fprintf(Outfile, "\t%s\n", s);
}

void sg(char *s, char *inst, char *s2) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, s, inst, s2);
  fputc('\n', Outfile);
}
void sg2(char *s, char *inst, char *s2, char *s3) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, s, inst, s2, s3);
  fputc('\n', Outfile);
}

void sg_operator(char *operator) {
  if (NULL == Outfile) return;
  fprintf(Outfile, "\t%s\n", operator);
}

void sg_unary_operation(char *op, char *opand) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t%s", op, opand);
  fputc('\n', Outfile);
}

void sg_binary_operation(char *op, char *opand1, char *opand2) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t%s, %s", op, opand1, opand2);
  fputc('\n', Outfile);
}

void ng_unary_operation(char *op, int value) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t$%d", op, value);
  fputc('\n', Outfile);
}

void ng_binary_operation(char *op, int source, char *destination) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t$%d, %s", op, source, destination);
  fputc('\n', Outfile);
}

void ng_ternary_operation(char *op, int value, char *source,
                          char *destination) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t%d(%s), %s", op, value, source, destination);
  fputc('\n', Outfile);
}

void define_data(char *type, int value) {
  if (NULL == Outfile) return;
  fputc('\t', Outfile);
  fprintf(Outfile, "%s\t%d", type, value);
  fputc('\n', Outfile);
}

int label(void) {
  static int id = 1;

  return id++;
}

void g_cvtsi2ss(Item x) {
  x->type = RealType;
  sg_binary_operation(CVTSI2SS, gc_reg(x->r), gc_FPUReg(RHF));
  x->r = RHF;
  incFPUR();
  --RH;
}

void g_cvtss2si(Item x) {
  x->type = IntType;
  sg_binary_operation(CVTSS2SI, gc_FPUReg(x->r), gc_reg(RH));
  x->r = RH;
  incR();
  --RHF;
}

void lg_cvtss2si(Item x) {
  x->type = IntType;
  x->a = real_index(x->a);

  char *source = gc_absolute_adr(REAL_LABEL);
  sg_binary_operation(LEA, source, gc_reg(RH));
  ng_ternary_operation(CVTSS2SI, x->a * 4, gc_reg(RH), gc_reg(RH));
  x->mode = CREG;
  x->r = RH;
  incR();
}

void g_setne(Item x) {
  gc_setcc(SETNE, x->r);
  gc_movzx(x->r);
}

void g_cmp2(int reg) {
  gc_binary_value2reg(CMP, 0, reg, IntType->size, IntType->form);
}

void load_compare_expression(Item x) {
  if (x->mode == CCONST) {
    load(x);
    gc_binary_value2reg(CMP, 0, x->r, IntType->size, IntType->form);
  } else {
    if (x->a != 0) {
      Fixup(x->a);
    }

    if (x->a != 0 && x->b != 0) {
      FJump(x->b);
    }

    // g_setne(y);
    if (x->b != 0) {
      Fixup(x->b);
    }
  }
  g_setne(x);
}

Item make_item(Object y) {
  long int r;
  Item x = checked_malloc(sizeof(*x));
  x->mode = y->class;
  x->type = y->type;
  x->level = y->level;
  x->a = y->value;
  x->rdo = y->rdo;
  copy_ident(x->name, y->name);

  if (y->class == CPAR) {
    x->b = 0;
  } else if (y->class == CCONST && y->type->form == FSTRING) {
    x->b = y->level;  // length
  } else {
    x->r = y->level;
  }

  if (y->level > 0 && y->level != CurrentLevel && y->class != CCONST) {
    mark("%s not accessible ", Text);
  }

  return x;
}

Item make_const_item(int value, Type tp) {
  Item x = checked_malloc(sizeof(*x));
  x->mode = CCONST;
  x->type = tp;
  x->name[0] = '\0';
  x->a = value;
  return x;
}

Item make_real_item(float value) {
  Float f;
  f.value = value;
  Item x = checked_malloc(sizeof(*x));
  x->mode = CCONST;
  x->type = RealType;
  x->name[0] = '\0';
  x->f = value;
  x->a = IEEE754float(f);
  return x;
}

Item make_strlit_item(char *value) {
  // int lab = label(), k = 0;
  // gdata();
  // glab(lab);
  // g_def_string(Text, Value);
  // k += Value - 1;
  // gc_def_byte(0);
  // galign(k+1);

  Item x = checked_malloc(sizeof(*x));
  x->mode = CCONST;
  x->type = StringType;
  x->a = strx;
  x->b = Value;
  copy_ident(x->name, Text);

  int i = 0;
  for (; i < Value - 1; i++) {
    store_str(Text[i]);
  }

  store_str(0);

  return x;
}

void fix(long int at, long int with) {
  fseek(Outfile, at, SEEK_SET);
  fprintf(Outfile, "%-19ld", with);
}

void fixLink(long int L) {
  long int L1;
  long int lab;

  if (L != 0) {
    lab = label();
    glab(lab);
  }

  while (L != 0) {
    fseek(Outfile, L, SEEK_SET);
    fscanf(Outfile, "%ld", &L1);
    fix(L, lab);
    L = L1;
  }
  fseek(Outfile, 0, SEEK_END);
}

void fixLinkWith(long int L0, long int dst) {
  long int L1;

  while (L0 != 0) {
    fseek(Outfile, L0, SEEK_SET);
    fscanf(Outfile, "%ld", &L1);
    fseek(Outfile, L0, SEEK_SET);
    fprintf(Outfile, "%-19ld", dst);
    L0 = L1;
  }
  fseek(Outfile, 0, SEEK_END);
}

void load(Item x) {
  gtext();
  int form = x->type->form;
  if (x->mode != CREG) {
    if (x->mode == CCONST) {
      if (form == FREAL) {
        if (x->r > 0) {  // local
          gc_load_real(x->a, RH, RHF);
        } else if (x->name[0] == '\0') {  // literal
          x->a = real_index(x->a);
          gc_load_real(x->a, RH, RHF);
        } else {
          char *adr = gc_absolute_adr(x->name);
          gc_load_mm(adr, RHF, x->type->size, form);
        }
        x->r = RHF;
        incFPUR();
      } else {
        gc_load_value(x->a, RH, form);
        x->r = RH;
        incR();
      }
    } else if (x->mode == CVAR) {
      char *adr;

      if (x->r > 0) {
        adr = gc_relative_adr(x->a);
      } else {
        adr = gc_absolute_adr(x->name);
      }

      if (form == FREAL) {
        x->r = RHF;
        incFPUR();
      } else {
        x->r = RH;
        incR();
      }

      gc_load_mm(adr, x->r, x->type->size, form);
    } else if (x->mode == CREGI) {
      if (x->type->form == FREAL) {
        gc_load_rmm(x->r, RHF, x->type->size, x->type->form);
        --RH;
        incFPUR();
      } else {
        gc_load_rmm(x->r, x->r, x->type->size, x->type->form);
      }
    } else if (x->mode == CPAR) {
      char *source = gc_relative_adr(x->a);
      gc_load_mm(source, RH, IntType->size, IntType->form);
      x->r = RH;
      gc_load_rmm(x->r, x->r, x->type->size, x->type->form);
      incR();

    } else if (x->mode == CCOND) {
      fixLink(x->b);
      fixLink(x->a);
      x->r = RH;
      incR();
    } else {
      x->r = RH;
      incR();
      mark("load undefined type", NULL);
    }
    x->mode = CREG;
  }
}

void load_adr(Item x) {
  if (x->mode == CVAR) {
    if (x->r > 0) {
      x->r = RH;
      char *source = gc_relative_adr(x->a);
      sg_binary_operation(LEA, source, gc_reg(x->r));
    } else {
      x->r = RH;
      char *source = gc_absolute_adr(x->name);
      sg_binary_operation(LEA, source, gc_reg(x->r));
    }
    incR();
  } else if (x->mode == CPAR) {
    char *adr = gc_relative_adr(x->a);
    gc_load_mm(adr, x->r, x->type->size, x->type->form);
    // gc_load(x->a, x->r);
    incR();
  } else if (x->mode != CREGI) {
    mark("address error", NULL);
  }
  x->mode = CREG;
}

void load_type_tag_adr(Type t) {
  Item x = checked_malloc(sizeof(*x));
  x->mode = CVAR;
  x->a = t->len;
  x->r = 1;
  load_adr(x);
  // free(x);
}

void load_string_adr(Item x) {
  x->r = RH;
  gc_load_str(x->a, x->r);
  incR();
}

void load_cond(Item x) {
  if (x->type->form == FBOOLEAN) {
    load(x);
    gc_binary_value2reg(CMP, 0, x->r, IntType->size, IntType->form);
    x->c = NEQ - EQL;

    x->mode = CCOND;
    x->a = 0;
    x->b = 0;
    --RH;
  } else {
    mark("%s not Boolean?", x->name);
  }
}

void value_param(Item x) { load(x); }

void var_param(Item x, Type tp) {
  int xmd = x->mode;
  load_adr(x);

  if (tp->form == FRECORD) {
    if (xmd == CPAR) {
      // gc_load_integer(x->b, RH);
      gc_load_value(x->b, RH, FINTEGER);
      incR();
    } else {
      load_type_tag_adr(x->type);
    }
  }
}

void save_regs(int r) {
  int r0 = 0;
  do {
    gc_pushq(r0);
    ++r0;
  } while (r0 != r);
}

void save_fpu_regs(int r) {
  int r0 = 0;
  do {
    gc_pushq(r0 << DIVIDE | DIVIDE_MASK);
    ++r0;
  } while (r0 != r);
}

void restore_regs(int r) {
  int r0 = r;
  while (r0 != 0) {
    --r0;
    gc_popq(r0);
  }
}
void restore_fpu_regs(int r) {
  int r0 = r;
  while (r0 != 0) {
    --r0;
    gc_popq(r0 << DIVIDE | DIVIDE_MASK);
  }
}

void call(Item x, int r[2], char *name) {
  gc_call(g_symbol(name));

  Object dsc = x->type->dscobj;
  int nofpar = x->type->nofpar;

  int j;
  for (j = 0; j < nofpar; j++) {
    if (dsc->type->form == FINTEGER || dsc->type->form == FBOOLEAN ||
        dsc->type->form == FCHAR) {
      RH = RH - 1;
    } else if (dsc->type->form == FREAL) {
      if (dsc->class == CVAR) {
        RHF = RHF - 1;
      } else {
        RH = RH - 1;
      }

    } else if (dsc->type->form == FARRAY) {
      RH = RH - 2;
    } else if (dsc->type->form == FRECORD) {
      RH = RH - 2;
    }
    dsc = dsc->next;
  }
  restore_regs(r[0]);

  restore_fpu_regs(r[1]);

  if (x->type->base->form != FNOTYPE) {
    x->mode = CREG;
    if (x->type->base->form != FREAL) {
      x->r = r[0];
      RH = r[0] + 1;
    } else {
      x->r = r[1];
      RHF = r[0] + 1;
    }
  }
}

void neg(Item x) {
  int form = x->type->form;
  int size = x->type->size;

  if (form == FINTEGER) {
    if (x->mode == CCONST) {
      x->a = -x->a;
    } else {
      load(x);
      gc_load_value(0, RH, x->type->form);
      gc_binary_reg2reg(SUB, x->r, RH, size, form);
      gc_binary_reg2reg(XCHG, x->r, RH, size, form);
    }
  } else if (form == FREAL) {
    char *destination;

    if (x->mode == CCONST) {
      x->a = x->a + 0x7FFFFFFF + 1;
    } else if (x->r > 0) {
      destination = gc_relative_adr(x->a);
      sg_unary_operation(FLDS, destination);
      gc_stack(-WORDSIZE);
      destination = gc_reference(rSP);
      ng_binary_operation(MOV, 0, destination);
      sg_unary_operation(FLDS, destination);
      sg_operator(FSUBP);
      destination = gc_relative_adr(x->a);
      sg_unary_operation(FSTS, destination);
      gc_stack(WORDSIZE);
    } else {
      destination = gc_absolute_adr(x->name);
      sg_unary_operation(FLDS, destination);
      gc_stack(-WORDSIZE);
      destination = gc_reference(rSP);
      ng_binary_operation(MOV, 0, destination);
      sg_unary_operation(FLDS, destination);
      sg_operator(FSUBP);
      destination = gc_absolute_adr(x->name);
      sg_unary_operation(FSTS, destination);
      gc_stack(WORDSIZE);
    }
  }
}

int *pre_call(Item x) {
  int r = RH;
  static int arr[2];
  if (RH > 0) {
    save_regs(RH);
    RH = 0;
  }
  arr[0] = r;
  r = RHF;
  if (RHF > 0) {
    save_fpu_regs(RHF);
    RHF = 0;
  }

  arr[1] = r;

  return arr;
}

int negated(int cond) {
  if (cond % 2 == 0) {
    ++cond;
  } else {
    --cond;
  }

  return cond;
}

void setCC(Item x, int op) {
  // gc_binary_value2reg(CMP, 0, x->r, IntType->size, IntType->form);
  // x->c = NEQ - EQL;
  x->c = op;
  x->mode = CCOND;
  x->a = 0;
  x->b = 0;
}

void int_relation(int op, Item x, Item y) {
  if (x->mode == CCOND || y->mode == CCOND) {
    mark("not implemented", NULL);
  }
  load(x);
  load(y);
  gc_binary_reg2reg(CMP, y->r, x->r, IntType->size, IntType->form);
  gc_setop(op - EQL, x->r);
  --RH;
  --RH;
  setCC(x, op - EQL);
}

void real_relation(int op, Item x, Item y) {
  if (x->mode == CCOND || y->mode == CCOND) {
    mark("not implemented", NULL);
  }
  load(x);
  load(y);
  gc_binary_reg2reg(UCOMISS, y->r, x->r, RealType->size, RealType->form);
  gc_setopss(op - EQL, x->r);
  --RHF;
  --RHF;
  setCC(x, op - EQL);
}

void Fixup(long int lab) { fixLink(lab); }

long int FJump(long int lab) { return gc_jmp(lab); }

void CFJump(Item x) {
  if (x->mode != CCOND) {
    load_cond(x);
  }
  x->a = gc_jcmp(negated(x->c), x->a);
  fixLink(x->b);
}

void BJump(long int lab) { gc_jmp(lab); }

void CBJump(Item x, long int lab) {
  if (x->mode != CCOND) {
    load_cond(x);
  }
  gc_jcmp(negated(x->c), lab);
  fixLink(x->b);
  fixLinkWith(x->a, lab);
}

void For0(Item x, Item y) { load(y); }

long int For1(Item x, Item y, Item z, Item w, long int lab) {
  load(z);
  gc_binary_reg2reg(CMP, z->r, y->r, IntType->size, IntType->form);
  --RH;

  if (w->a > 0) {
    lab = gc_jcmp(GTR - EQL, lab);
  } else if (w->a < 0) {
    lab = gc_jcmp(LSS - EQL, lab);
  } else {
    mark("zero increment", NULL);
  }

  store(x, y);

  return lab;
}

void For2(Item x, Item y, Item w) {
  load(x);
  --RH;
  gc_binary_value2reg(ADD, w->a, x->r, x->type->size, x->type->form);
}

void real_op(int op, Item x, Item y) {
  int form = x->type->form;
  int size = x->type->size;
  char *oper;
  load(x);
  load(y);

  if (op == RDIV) {
    oper = DIVSS;
  } else if (op == TIMES) {
    oper = MULSS;
  } else if (op == PLUS) {
    oper = ADDSS;
  } else if (op == MINUS) {
    oper = SUBSS;
  }

  gc_binary_reg2reg(oper, y->r, x->r, size, form);
  --RHF;
  x->r = RHF - 1;
}

void add_op(int op, Item x, Item y) {
  int form = x->type->form;
  int size = x->type->size;
  char *oper;

  load(x);
  load(y);

  if (op == PLUS) {
    oper = ADD;
  } else {
    oper = SUB;
  }

  gc_binary_reg2reg(oper, y->r, x->r, size, form);

  if (y->r < x->r) {
    gc_binary_reg2reg(XCHG, x->r, y->r, size, form);
    x->r = y->r;
  }

  --RH;
}

void mul_op(Item x, Item y) {
  int form = x->type->form;
  int size = x->type->size;
  load(x);
  load(y);

  gc_mul(y->r, x->r);
  if (y->r < x->r) {
    gc_binary_reg2reg(XCHG, x->r, y->r, size, form);
    x->r = y->r;
  }
  --RH;
}

void div_op(int op, Item x, Item y) {
  int form = x->type->form;
  int size = x->type->size;
  if (op == DIV) {
    load(x);
    load(y);

    gc_div(y->r, x->r);
    if (y->r < x->r) {
      gc_binary_reg2reg(XCHG, x->r, y->r, size, form);
      x->r = y->r;
    }
    --RH;
  } else {
    load(x);
    load(y);

    gc_mod(y->r, x->r);
    if (y->r < x->r) {
      gc_binary_reg2reg(XCHG, x->r, y->r, size, form);
      x->r = y->r;
    }
    --RH;
  }
}

void store(Item x, Item y) {
  gtext();
  char *destination;
  int size, form;

  switch (x->mode) {
    case CVAR:
      if (x->r > 0) {  // local
        size = x->type->size;
        form = x->type->form;
        if (y->mode == CCONST && x->type->form != FREAL) {
          destination = gc_relative_adr(x->a);
          gc_store_value(y->a, destination, size, form);
        } else {
          load(y);
          if (y->mode == CREG) {
            char *destination;
            destination = gc_relative_adr(x->a);
            gc_store_reg(y->r, destination, size, form);
            switch (x->type->form) {
              case FBOOLEAN:
              case FINTEGER:
              case FCHAR:
              case FPOINTER:
                --RH;
                break;
              case FREAL:
                --RHF;
                break;
              default:
                --RH;
            }
          }
        }
      } else {
        load(y);
        gtext();
        destination = gc_absolute_adr(x->name);
        gc_store_reg(y->r, destination, x->type->size, x->type->form);
        if (y->type == IntType || x->type->form == FBOOLEAN ||
            x->type->form == FCHAR || x->type->form == FPOINTER) {
          --RH;
        } else if (y->type == RealType) {
          --RHF;
        }
      }
      break;
    case CREGI:
      load(y);
      gc_store_reg2rmm(y->r, x->r, x->type->size, x->type->form);
      if (x->type->form == FREAL) {
        --RHF;
      } else {
        --RH;
      }
      --RH;
      break;
    case CPAR:
      load(y);
      destination = gc_relative_adr(x->a);
      size = IntType->size;
      form = IntType->form;

      gc_load_mm(destination, RH, size, form);
      x->r = RH;
      gc_store_reg2rmm(y->r, x->r, x->type->size, x->type->form);

      if (x->type->form == FINTEGER || x->type->form == FCHAR ||
          x->type->form == FBOOLEAN) {
        --RH;
      } else if (x->type->form == FREAL) {
        --RHF;
      }

    default:
      break;
  }
}

void arr_index(Item x, Item y) {
  int lim = x->type->len;
  int size = x->type->base->size;
  if (y->mode == CCONST && lim > 0) {
    if (y->a < 0 || y->a >= lim) {
      char buf[32];
      sprintf(buf, "%ld", y->a);
      mark("%s bad index", buf);
    }

    if (x->mode == CVAR) {
      x->a = x->a + y->a * size;
    } else if (x->mode == CREGI) {
      gc_binary_value2reg(ADD, y->a * size, x->r, y->type->size, y->type->form);
    }
  } else {
    load(y);
    gc_binary_value2reg(IMUL, size, y->r, IntType->size, IntType->form);
    if (x->mode == CVAR) {
      if (x->r > 0) {
        gc_binary_value2reg(ADD, x->a, y->r, x->type->size, x->type->form);
        sg_binary_operation(ADD, rBP, gc_reg(y->r));
      } else {
        char *source = gc_absolute_adr(x->name);
        sg_binary_operation(LEA, source, gc_reg(RH));
        copy_ident(source, gc_reg(RH));
        sg_binary_operation(ADD, source, gc_reg(y->r));
      }

      x->r = y->r;
      x->mode = CREGI;
    } else if (x->mode == CPAR) {
      char *source = gc_relative_adr(x->a);
      gc_load_mm(source, RH, IntType->size, IntType->form);
      x->r = RH;
      gc_binary_reg2reg(ADD, x->r, y->r, IntType->size, IntType->form);
      x->r = y->r;
      x->mode = CREGI;
    } else if (x->mode == CREGI) {
      gc_binary_reg2reg(ADD, y->r, x->r, x->type->size, x->type->form);
      --RH;
    }
  }
}

void field(Item x, Object y) {
  if (x->mode == CVAR) {
    if (x->r > 0) {
      x->a = x->a + y->value;

    } else {
      load_adr(x);
      gc_binary_value2reg(ADD, y->value, x->r, x->type->size, x->type->form);
      x->mode = CREGI;
    }
  } else if (x->mode == CPAR) {
    load_adr(x);
    gc_binary_value2reg(ADD, y->value, x->r, x->type->size, x->type->form);
  } else if (x->mode == CREGI) {
    gc_binary_value2reg(ADD, y->value, x->r, x->type->size, x->type->form);
  }
}

// https://www.tutorialspoint.com/assembly_programming/assembly_movs_instruction.htm
void copy_string(Item x, Item y) {
  gtext();
  int len = x->type->len;
  int r = RH;
  int xmode = x->mode;

  if (r >= 3) {
    sg_unary_operation(PUSH, rCX);
    sg_unary_operation(PUSH, rDI);
    sg_unary_operation(PUSH, rSI);
    RH = RH - 3;
  } else if (r >= 2) {
    sg_unary_operation(PUSH, rDI);
    sg_unary_operation(PUSH, rSI);
    RH = RH - 2;
  } else if (r >= 1 && !(xmode == CREGI && x->r == 0)) {
    sg_unary_operation(PUSH, rDI);
    RH = RH - 1;
  }

  load_adr(x);

  if (len >= 0) {
    if (len < y->b) {
      mark("%s string too long", x->name);
    }
  }

  load_string_adr(y);
  gc_load_value(y->b, rCXI, IntType->form);

  sg_operator(CLD);
  sg_unary_operation(REP, MOVSB);

  RH = RH - 2;
  if (r >= 3) {
    sg_unary_operation(POP, rSI);
    sg_unary_operation(POP, rDI);
    sg_unary_operation(POP, rCX);
    RH = RH + 3;
  } else if (r >= 2) {
    sg_unary_operation(POP, rSI);
    sg_unary_operation(POP, rDI);
    RH = RH + 2;
  } else if (r >= 1 && !(xmode == CREGI && x->r == 0)) {
    sg_unary_operation(POP, rDI);
    RH = RH + 1;
  }

  // int lab = label();
  // Fixup(lab);
  // gc_load_rmm(y->r, RH, x->type->base->size, FCHAR);
  // gc_store_reg2rmm(RH, x->r, x->type->base->size, x->type->base->form);
  // gc_binary_value2reg(ADD, 1, y->r, y->type->size, y->type->form);
  // gc_binary_value2reg(ADD, 1, x->r, x->type->size, x->type->form);
  // gc_cmp2(RH);
  // gc_jcmp(NEQ - EQL, lab);
}

void str_to_char(Item x) {
  x->type = CharType;
  x->a = x->name[0];
}

void open_array_param(Item x) {
  load_adr(x);
  if (x->type->len >= 0) {
    gc_load_value(x->type->len, RH, FINTEGER);
    incR();
  } else {
    char *source = gc_relative_adr(x->a + WORDSIZE);
    gc_load_mm(source, RH, IntType->size, IntType->form);
    incR();
  }
}

void string_parameter(Item x) {
  load_string_adr(x);
  gc_load_value(x->b, RH, FINTEGER);
  incR();
}

int merged(long int L0, long int L1) {
  long int L2, L3;
  if (L0 != 0) {
    L3 = L0;
    do {
      L2 = L3;
      fseek(Outfile, L2, SEEK_SET);
      fscanf(Outfile, "%ld", &L3);
    } while (L3 != 0);
    fseek(Outfile, L2, SEEK_SET);
    fprintf(Outfile, "%-19ld", L1);
    L1 = L0;
  }
  fseek(Outfile, 0, SEEK_END);
  return L1;
}

void prologue(Object proc, int parblksize, int locblksize) {
  gtext();
  if (CurrentLevel == 1) {
    gpublic(proc->name);
  }
  g_name(proc->name);
  gentry();
  gstack(-locblksize);
  gparamters(proc->type->nofpar, proc->type->dscobj);
}

void epilogue(int locblksize) {
  gstack(locblksize);
  gexit();
}

void not(Item x) {
  long int t;
  if (x->mode != CCOND) {
    load_cond(x);
  }
  ng_binary_operation(XOR, 1, gc_reg(x->r));
  gc_binary_value2reg(CMP, 0, x->r, IntType->size, IntType->form);
  x->c = negated(x->c);
  t = x->a;
  x->a = x->b;
  x->b = t;
}

void or1(Item x) {
  if (x->mode != CCOND) {
    load_cond(x);
  }
  x->b = gc_jcmp(x->c, x->b);
  // if (x->a != 0) {
  //   Fixup(x->a);
  // }
  fixLink(x->a);
  x->a = 0;
}

void or2(Item x, Item y) {
  if (y->mode != CCOND) {
    load_cond(y);
  }
  x->a = y->a;
  x->b = merged(y->b, x->b);
  x->c = y->c;
}

void and1(Item x) {
  if (x->mode != CCOND) {
    load_cond(x);
  }

  x->a = gc_jcmp(negated(x->c), x->a);

  fixLink(x->b);
  // if (x->b != 0) {
  //   Fixup(x->b);
  // }
  x->b = 0;
}

void and2(Item x, Item y) {
  if (y->mode != CCOND) {
    load_cond(y);
  }
  x->a = merged(y->a, x->a);
  x->b = y->b;
  x->c = y->c;
}

void generator·dereference(Item x) {
  if (x->mode == CVAR) {
    if (x->r > 0) {
      char *adr;
      adr = gc_relative_adr(x->a);

      gc_load_mm(adr, RH, x->type->size, x->type->form);
    }
    x->r = RH;
    incR();
  }

  x->mode = CREGI;
}

// https://helloacm.com/optimized-abs-function-in-assembly/
// https://stackoverflow.com/questions/2639173/x86-assembly-abs-implementation
void Abs(Item x) {
  if (x->mode == CCONST && x->type->form == FINTEGER) {
    x->a = -x->a;
  } else {
    if (x->type->form == FREAL) {
      char *destination;
      char *source;

      if (x->mode == CVAR) {
        if (x->r > 0) {
          destination = gc_relative_adr(x->a);
        } else {
          destination = gc_absolute_adr(x->name);
        }
        sg_unary_operation(FLDS, destination);
        sg_operator(FABS);
        sg_unary_operation(FSTS, destination);
      } else if (x->mode == CCONST || x->mode == CREG) {
        load(x);
        source = gc_FPUReg(x->r);
        destination = gc_reference(rSP);
        gc_stack(-WORDSIZE);
        sg_binary_operation(MOVSS, source, destination);
        sg_unary_operation(FLDS, destination);
        sg_operator(FABS);
        sg_binary_operation(MOVSS, destination, source);
        sg_unary_operation(FSTS, destination);
        gc_stack(WORDSIZE);
      }
      x->type = RealType;
    } else {
      load(x);
      int reg = x->r;
      int size = IntType->size;
      int form = IntType->form;

      save_product_registers(reg, reg);
      gc_binary_reg2reg(MOV, reg, rAXI, size, form);

      sg_operator(CQO);

      gc_binary_reg2reg(XOR, rDXI, rAXI, size, form);
      gc_binary_reg2reg(SUB, rDXI, rAXI, size, form);
      gc_binary_reg2reg(MOV, rAXI, reg, size, form);

      restore_product_registers(reg, reg);
    }
  }
}

void Floor(Item x) {
  char *destination, *source;
  if (x->mode == CVAR) {
    if (x->r > 0) {
      destination = gc_relative_adr(x->a);
    } else {
      destination = gc_absolute_adr(x->name);
    }
    sg_unary_operation(FLDS, destination);
    sg_operator(FRNDINT);
    sg_unary_operation(FSTS, destination);
    load(x);
    sg_binary_operation(CVTSS2SI, gc_FPUReg(x->r), gc_reg(RH));

    --RHF;
    x->r = RH;
    incR();
    x->type = IntType;
  } else if (x->mode == CCONST || x->mode == CREG) {
    load(x);
    source = gc_FPUReg(x->r);
    destination = gc_reference(rSP);
    gc_stack(-WORDSIZE);
    sg_binary_operation(MOVSS, source, destination);
    sg_unary_operation(FLDS, destination);
    sg_operator(FRNDINT);
    sg_binary_operation(MOVSS, destination, source);
    sg_unary_operation(FSTS, destination);
    sg_binary_operation(CVTSS2SI, gc_FPUReg(x->r), gc_reg(RH));
    x->r = RH;
    incR();
    gc_stack(WORDSIZE);
    x->type = IntType;
    --RHF;
  }
}

void Ord(Item x) {
  // int tf = x->type->form;

  // if (tf != FCHAR && tf != FBOOLEAN && tf != FSET) {
  //     mark("Ord's parameter's type must be char, boolean or set", NULL);
  // }

  // load(x);

  int tf = x->type->form;
  if (tf == FREAL) {
    x->type = IntType;
  }
  load(x);

  if (tf == FREAL) {
    int v = 0x7FFFFFFF;
    // gc_load_uinteger(v, RH);
    gc_load_value(v, RH, IntType->form);
    // gc_and(RH, x->r);
    gc_binary_reg2reg(ANDx, RH, x->r, IntType->size, IntType->form);
  }
}

void Shift(int fct, Item x, Item y) {
  char *op;
  load(x);

  if (fct == 1) {
    op = SAR;
  }

  if (y->mode == CCONST) {
    ng_binary_operation(op, y->a % BITS, gc_reg(x->r));
  } else {
    load(y);
    save_rCX(y->r, x->r);
    gc_binary_reg2reg(MOV, y->r, rCXI, x->type->size, x->type->form);
    sg_binary_operation(op, CL, gc_reg(x->r));
    restore_rCX(y->r, x->r);
    --RH;
  }
}

void Write(int r[2]) {
  gc_align(r[0] + r[1]);
  gc_call("Write");
  gc_popq_align(r[0] + r[1]);
  restore_fpu_regs(r[1]);
  restore_regs(r[0]);
  RHF = 0;
  RH = 0;
}

void Return(int form, Item x, bool interrupt) {
  if (form != FNOTYPE) {
    load(x);
  }
  RH = 0;
  RHF = 0;
}

void generator·Brk(Item x) {
  int *r;
  int mode = x->mode;
  if (mode == CREG && x->r == 0) {
    gc_call("Brk");
  } else {
    r = pre_call(x);
    load(x);
    gc_call("Brk");
    if (r[0] > 0) {
      gc_binary_reg2reg(MOV, x->r, r[0], x->type->size, x->type->form);
      x->r = r[0];
    }
    restore_fpu_regs(r[1]);
    restore_regs(r[0]);
    x->mode = CREG;
    if (r[0] > 0) {
      RH = r[0] + 1;
    }
  }
}

void generator·Free(Item x, Item y) {
  int *r;
  r = pre_call(x);
  load(x);
  load(y);
  gc_call("Free");
  if (r[0] > 0) {
    gc_binary_reg2reg(MOV, x->r, r[0] + 1, x->type->size, x->type->form);
    x->r = r[0] + 1;
  }
  --RH;
  restore_fpu_regs(r[1]);
  restore_regs(r[0]);
}

void generator·Adr(Item x) {
  if (x->mode == CVAR || x->mode == CPAR || x->mode == CREGI) {
    load_adr(x);
  } else {
    mark("not addressable", NULL);
  }
}

void generator·Get(Item x, Item y) {
  load(x);
  x->type = y->type;
  x->mode = CREGI;
  store(y, x);
}

void generator·Put(Item x, Item y) {
  load(x);
  x->type = y->type;
  x->mode = CREGI;
  store(x, y);
}