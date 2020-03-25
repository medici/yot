#ifndef GENERATOR_H
#define GENERATOR_H

#include "data.h"


int PC;

typedef struct Item *Item;

struct Item {
    int mode;
    int level;
    Type type;
    long int a;
    int b;
    int c;
    int r;
    bool rdo;
    char name[TEXTLEN + 1];
    long double f;
};

int label(void);

void open();
void gentry();
void gstack(int n);
void gexit();

void gtext();
void gdata(void);
int rh();
int rhf();
void check_regs();
void g_name(char *name);
char* g_symbol(char *s);
void gpublic(char *procid);
void glab(int id);

void galign(int k); 
void gcstorlw(int n);
void lg(char *s, char *inst, int n);
void lg2(char *s, int n, char *inst);
void graw(char *s);
void g();
void sg(char *s, char *inst, char *s2);
void sg2(char *s, char *inst, char *s2, char *s3);
void gprelude();
void gpostlude();
void ng(char *s, char *inst, int n, char *reg);
void ng2(char *s, char *inst, int v, int offset);
void ngmove(int reg, int v);
void g_def_string(char *s, int len);
void g_def_byte(int v);
void g_def_quad(int v);
void g_def_IEEE754_float(long int v);
void g_cvtsi2ss(Item x);
void g_cvtss2si(Item x);
void lg_cvtss2si(Item x);
void g_setne(Item x);
void g_cmp2(int reg);
void load_compare_expression(Item x);
Item make_item(Object y);
Item make_const_item(int value, Type tp);
Item make_real_item(float value);
Item make_strlit_item(char *value);
void neg(Item x);
int* pre_call(Item x);
void int_relation(int op, Item x, Item y);
void real_relation(int op, Item x, Item y);
void Fixup(int lab);
void FJump();
void CFJump(Item x);
void CBJump(Item x, int lab);
void Fixup(int lab);
void For0(Item x, Item y);
void For1(Item x, Item y, Item z, Item w, int lab);
void For2(Item x, Item y, Item w);
void add_op(int op, Item x, Item y);
void mul_op(Item x, Item y);
void div_op(int op, Item x, Item y);
void real_op(int op, Item x, Item y);
void call(Item x, int r[2], char *name);
void load(Item x);
void store(Item x, Item y);
void open_array_param(Item x);
void string_parameter(Item x);
void copy_string(Item x, Item y);
void str_to_char(Item x);
void arr_index(Item x, Item y);
void field(Item x, Object y);
void prologue(Object proc, int parblksize, int locblksize);
void epilogue(int locblksize);
void not(Item x);
void or1(Item x);
void or2(Item x, Item y);
void and1(Item x);
void and2(Item x, Item y);

void Abs(Item x);
void Floor(Item x);
void Ord(Item x);
void Shift(int fct, Item x, Item y);
void Write(int r[2]);
void call_write_int(int r[2]);
void call_write_real(int r[2]);
void call_write_bool(int r[2]);
void call_write_ln();

#endif