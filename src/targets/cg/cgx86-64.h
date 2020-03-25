#ifndef GENERATOR_CODE_H
#define GENERATOR_CODE_H

#define FPU_START 16
#define DIVIDE 4
#define DIVIDE_MASK 0xF
#define DIVIDE_MASK2 0xF0

void gcdata();
void gctext();
void gcpublic(char *s);
void gcentry();
void gcexit(void);
void gcprelude(void);
void gcpostlude(void);
void gc_def_char(int c);
void gc_def_byte(int v);
void gcstack(int n);
void gcldlab(int id);
void gc_def_quad(int v);
void gc_align(int num);
void gc_popq_align(int num);
void gc_pushq(int reg);
void gc_popq(int reg);
void gc_cvtsi2ss(int rh, int rhf);
void gc_cvtss2si(int rhf, int rh);
void lgc_cvtss2si(int lab, int rh);
void gc_flds(int offset);
void gc_fldt(int offset);
void gc_flds2(char *name);
void gc_fldt2(char *name);
void gc_flds3(int lab);
void gc_fldt3(int lab);
void gc_frndint();
void gc_fsts(int offset);
void gc_fstls(int offset);
void gc_fsts2(char *name);
void gc_fstl2(char *name);
void gc_fsts3(int reg);
void gc_fstl3();
void gc_fabs();
void gc_movzx(int reg);
void gc_setne(int reg);
void gc_setop(int cond, int reg);
void gc_setopss(int cond, int reg);
void gc_cmp(int reg2, int reg1);
void gc_cmp2(int reg);
void gc_cmp3(int reg);
void gc_ucomiss(int reg2, int reg1);
void gc_jmp(int lab);
int gc_jcmp(int cond, int lab);
void gc_mov(int reg2, int reg1);
void gc_shrl(int bit, int reg);
void gc_xor(int reg);
void gc_xor2(int reg2, int reg1);
void gc_xorb(int reg2, int reg1);
void gc_add(int reg2, int reg1);
void gc_add2(int v, int reg1);
void gc_sub(int reg2, int reg1);
void gc_mul(int reg2, int reg1);
void gc_mul2(int v, int reg);
void gc_div(int reg2, int reg1);
void gc_mod(int reg2, int reg1);
void gc_leaq(int offset, int reg);
void gc_leaq2(int lab, int reg);
void gc_leaq3(char *name, int reg);
void gc_addss(int reg2, int reg1);
void gc_subss(int reg2, int reg1);
void gc_mulss(int reg2, int reg1);
void gc_divss(int reg2, int reg1);
void gc_and(int reg2, int reg1);
void gc_xchgq(int reg1, int reg2);
void gc_shift(int op, int count, int reg);
void gc_load(int offset, int reg);
void gc_load_integer(int v, int reg);
void gc_load_uinteger(unsigned int v, int reg);
void gc_load_imm(int reg2, int reg1);
void gc_load_immsb(int reg2, int reg1);
void gc_load_imm2(int reg2, int reg1);
void gc_load_offset(int reg1);
void gc_load_offset2(int reg2, int reg1);
void gc_loadss(int offset, int reg);
void gc_loadsb(int offset, int reg);
void gc_loadb(int reg2, int reg1);
void gc_store_integer_literal(int v, int offset);
void gc_store_byte_literal(int v, int offset);
void gc_load_globl(char *name, int rh);
void gc_load_globlss(char *name, int rhf);
void gc_load_globlsb(char *name, int rh);
void gc_store_reg(int reg, int v);
void gc_store_reg2(int reg2, int reg1);
void gc_store_regss(int reg, int v);
void gc_store_regsb(int reg, int v);
void gc_storeb(int reg2, int reg1);
void gc_store_reg2globl(int reg, char *name);
void gc_store_reg2globlss(int reg, char *name);
void gc_store_reg2globlb(int reg, char *name);
void gc_store_reg2imm(int reg2, int reg1);
void gc_store_regss2imm(int reg2, int reg1);
void gc_store_bool_literal(int v, char*name);
void gc_call(char *s);
void gc_call2(char *s);

void gc_byte_literal(int reg, int v);
void gc_integer_literal(int reg, int v);
void gc_real_literal(int reg, int v);
void gc_long_int(long int v);
#endif