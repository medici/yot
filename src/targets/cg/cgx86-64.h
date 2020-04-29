#ifndef GENERATOR_CODE_H
#define GENERATOR_CODE_H

#define FPU_START 16
#define DIVIDE 4
#define DIVIDE_MASK 0xF
#define DIVIDE_MASK2 0xF0

#define MOV     "movq"
#define MOVB    "movb"
#define MOVSS   "movss"
#define MOVSB   "movsb"
#define MOVZX   "movzx"

#define PUSH    "pushq"
#define POP     "popq"
#define LEA     "leaq"

#define ADD     "addq"
#define ADDSS   "addss"
#define SUB     "subq"
#define SUBSS   "subss"
#define IMUL    "imul"
#define MULSS   "mulss"
#define IDIV    "idivq"
#define DIVSS   "divss"

#define XOR     "xorq"
#define ANDx    "andq"
#define SAR     "sarq"


#define XCHG    "xchgq"
#define CQO     "cqo"
#define REP     "rep" 

#define FABS    "fabs"
#define FLDS    "flds"
#define FSTS    "fsts"
#define FRNDINT "frndint"
#define FSUBP   "fsubp"


#define CVTSS2SI "cvtss2si"
#define CVTSI2SS "cvtsi2ss"

#define rDI     "%rdi"
#define rSI     "%rsi"
#define rAX     "%rax"
#define rDX     "%rdx"
#define rCX     "%rcx"
#define CL      "%cl"
#define rDII    0
#define rSII    1
#define rAXI    6
#define rDXI    3
#define rCXI    2
#define rSP     "%rsp"
#define rBP     "%rbp"

enum {
	BYTE = 1,
	WORD = 2,
	DOUBLEWORD = 4,
	QUADWORD = 8
};

void gcdata();
void gctext();
void gcpublic(char *s);
void gcentry();
void gcexit(void);
void gcprelude(void);
void gcpostlude(void);

char* gc_FPUReg(int reg);
char* gc_reg(int reg);
char* gc_reference(char *reg);
void gc_dereference(char *reg, int form);

char* gc_absolute_adr(char *name);
char* gc_relative_adr(int lab);
void gc_load_mm(char *mm, int reg, int size, int form);
void gc_load_value(int value, int reg, int form);
void gc_load_real(int offset, int reg, int fpureg);
void gc_load_str(int offset, int reg);
void gc_load_rmm(int reg1, int reg2, int size, int form);
void gc_binary_reg2reg(char *op, int source, int destination, int size, int form);
void gc_binary_value2reg(char *op, int value, int destination, int size, int form);

void gc_store_value(int v, char *destination, int size, int form);
void gc_store_reg(int reg, char *destination, int size, int form);
void gc_store_reg2rmm(int reg2, int reg1, int size, int form);

void save_rCX(int reg2, int reg1);
void restore_rCX(int reg2, int reg1);

void gc_def_byte(int v);
void gc_def_long(long int v);
void gc_def_quad(int v);

void gcstack(int n);
void gcldlab(int id);
void gc_align(int num);
void gc_popq_align(int num);
void gc_pushq(int reg);
void gc_popq(int reg);

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

void gc_xorb(int reg2, int reg1);
void gc_mul(int reg2, int reg1);
void gc_div(int reg2, int reg1);
void gc_mod(int reg2, int reg1);

void gc_call(char *s);


#endif