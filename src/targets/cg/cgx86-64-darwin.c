#include "generate_code.h"
#include "generator.h"
#include "declare.h"
//https://wiki.cdot.senecacollege.ca/wiki/X86_64_Register_and_Instruction_Quick_Start
// https://home.deec.uc.pt/~jlobo/tc/artofasm/ch14/ch144.htm#HEADING4-5
// https://code.woboq.org/userspace/glibc/sysdeps/ieee754/dbl-64/s_rint.c.html
// https://docs.oracle.com/cd/E19455-01/806-3773/index.html

char regs[][13] = {
    "%rdi", "%rsi", "%rcx", "%rdx", "%r8", "%r9",
    "%rax", "%r10", "%r11", "%rbx",  "%r12", "%r13", "%r14", "%r15"
};

char regs_32[][13] = {
    "%edi", "%esi", "%ecx", "%edx", "%r8d", "%r9d",
    "%eax", "%r10d", "%r11d", "%ebx", "%r12d", "%r13d", "%r14d", "%r15d"
};

char regs_byte[][13] = {
    "%dil", "%sil", "%cl", "%dl", "%r8b", "%r9b",
    "%al", "%r10b", "%r11b", "%bl", "%r12b", "%r13b", "%r14b", "%r15b"
};

char FPURegs[][7] = {
    "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
};

void
gc_data() {
    sg_operator(DATA_LABEL);
}

void
gc_text() {
    sg_operator(TEXT_LABEL);
}

void gc_prelude(void)	{ }
void gc_postlude(void)	{ }

int
gc_stack_align(int size) {
    if (size % (WORDSIZE * 2) != 0) {
        if (size < 0) {
            size -= WORDSIZE;
        } else {
            size += WORDSIZE;
        }
    }
    return size;
}

void
gc_stack(int n)	{
    n = gc_stack_align(n);
    ng_binary_operation(ADD, n, rSP);
}

char* 
gc_FPUReg(int reg) {
    static char adr[512 + 8];

    sprintf(adr, "%s", FPURegs[reg]);

    return adr;
}

char* 
gc_reg(int reg) {
    static char adr[512 + 8];

    sprintf(adr, "%s", regs[reg]);

    return adr;
}

char*
gc_reference(char *reg) {
    static char adr[512 + 8];

    sprintf(adr, "(%s)", reg);

    return adr;
}

void 
gc_dereference(char *reg, int form) {
    char *op = MOV;

    if (form == FREAL) {
        op = MOVSS;
    }

    sg_binary_operation(op, gc_reference(reg), reg);
}

char*
gc_absolute_adr(char *name) {
    static char adr[512 + 8];

    sprintf(adr, "%c%s(%%rip)", PREFIX, name);

    return adr;
}

char*
gc_relative_adr(int lab) {
    static char adr[64];

    sprintf(adr, "%d(%%rbp)", lab);

    return adr;
}

void
gc_load_mm(char *mm, int reg, int size, int form) {
    char *op = MOV;
    char *destination = regs[reg];

    if (size == BYTE) {
        op = MOVB;
        destination = regs_byte[reg];
    }

    if (form == FREAL) {
        op = MOVSS;
        destination = FPURegs[reg];
    }

    sg_binary_operation(op, mm, destination);
}

void
gc_load_value(int value, int reg, int form) {
    char *op = MOV;
    char *destination = regs[reg];

    if (form == FREAL) {
        op = MOVSS;
        destination = FPURegs[reg];
    }

    ng_binary_operation(op, value, destination);
}

void
gc_binary_reg2reg(char *op, int source, int destination, int size, int form) {
    char *from = regs[source];
    char *to = regs[destination];
    if (size == BYTE) {
        from = regs_byte[source];
        to = regs_byte[destination];
    }

    if (form == FREAL) {
        from = FPURegs[source];
        to = FPURegs[destination];
    }
    sg_binary_operation(op, from, to);
}

void
gc_binary_value2reg(char *op, int value, int destination, int size, int form) {
    char *to = regs[destination];

    if (size == BYTE) {
        to = regs_byte[destination];
    }

    if (form == FREAL) {
        to = FPURegs[destination];
    }

    ng_binary_operation(op, value, to);
}

void
gc_load_real(int offset, int reg, int fpureg) {
    char *source = gc_absolute_adr(REAL_LABEL);
    sg_binary_operation(LEA, source, regs[reg]);
    ng_ternary_operation(MOVSS, offset * 4, regs[reg], FPURegs[fpureg]);
}

void
gc_load_str(int offset, int reg) {
    char *source = gc_absolute_adr(STR_LABEL);
    sg_binary_operation(LEA, source, regs[reg]);
    ng_ternary_operation(LEA, offset, regs[reg], regs[reg]);
}


void
gc_load_rmm(int reg1, int reg2, int size, int form) {
    char source[64];
    char *op = MOV;
    char *destination = regs[reg2];

    sprintf(source, "(%s)", regs[reg1]);

    if (size == BYTE) {
        op = MOVB;
        destination = regs_byte[reg2];
        if (reg1 != reg2) {
            sg_binary_operation(XOR, regs[reg2], regs[reg2]);
        }
       
    }

    if (form == FREAL) {
        op = MOVSS;
        destination = FPURegs[reg2];
    }

    sg_binary_operation(op, source, destination);

    if (size == BYTE && reg1 == reg2) {
        sg_binary_operation(MOVZX, regs_byte[reg2], regs[reg2]);
    }
}

void
gc_store_value(int v, char *destination, int size, int form) {
    char *op = MOV;

    if (form == FREAL) {
        op = MOVSS;
    } else if (size == BYTE) {
        op = MOVB;
    }

    ng_binary_operation(op, v, destination);
}

void
gc_store_reg(int reg, char *destination, int size, int form) {
    char *op = MOV, *source = regs[reg];

    if (size == BYTE) {
        op = MOVB;
        source = regs_byte[reg];
    } else if (form == FREAL) {
        op = MOVSS;
        source = FPURegs[reg];
    }

    sg_binary_operation(op, source, destination);
}

void
gc_store_reg2rmm(int reg2, int reg1, int size, int form) {
    char *op = MOV, *source = regs[reg2], *destination = gc_reference(regs[reg1]);

    if (size == BYTE) {
        op = MOVB;
        source = regs_byte[reg2];
    } else if (form == FREAL) {
        op = MOVSS;
        source = FPURegs[reg2];
    }

    sg_binary_operation(op, source, destination);
}

void
gc_def_byte(int v) { 
    define_data(BYTE_LABEL, v);
}

void
gc_def_long(long int v) {
    define_data(LONG_LABEL, v);
}

void
gc_def_quad(int v) { 
    define_data(QUAD_LABEL, v);
}

void
gc_align(int num) {
    if (num % 2 == 1) {
        ng_unary_operation(PUSH, 0);
    }
}

void
gc_popq_align(int num) {
    if (num % 2 == 1) {
        sg_unary_operation(POP, regs[0]);
    }
}

void
gc_call(char *function) {
    sg_unary_operation(CALL, function);
}

void
gc_pushFPU(int reg) {
    ng_binary_operation(SUB, 16, rSP);
    sg_binary_operation(MOVDQU, FPURegs[reg], gc_reference(rSP));
}

void
gc_popFPU(int reg) {
    sg_binary_operation(MOVDQU, gc_reference(rSP), FPURegs[reg]);
    ng_binary_operation(ADD, 16, rSP);
}

void
gc_pushq(int reg) {
    if (reg >= DIVIDE_MASK) {
        gc_pushFPU(reg >> DIVIDE);
    } else {
        sg_unary_operation(PUSH, regs[reg]);
    }
}

void
gc_popq(int reg) {
    if (reg >= DIVIDE_MASK) {
        gc_popFPU(reg >> DIVIDE);
    } else {
        sg_unary_operation(POP, regs[reg]);
    }
    
}


/*

FLD source
中source可以为32 ，64或者80位整数值．
为了区分这两种长度，GNU汇编器使用FLDS加载单精度浮点数,FLDL加载双精度浮点数，
类似FST用于获取FPU寄存器堆栈中顶部的值, 并且把这个值放到内存位置中, 
对于单精度使用FSTS, 对于双精度使用FSTL

*/


void
gc_movzx(int reg) {
    sg_binary_operation(MOVZX, regs_byte[reg], regs[reg]);
}

void
gc_setcc(char *op, int reg) {
    sg_unary_operation(op, regs_byte[reg]);
}

void
gc_setop(int cond, int reg) {
    switch(cond) {
        case 0:
            gc_setcc(SETE, reg);
            break;
        case 1:
            gc_setcc(SETNE, reg);
            break;
        case 2:
            gc_setcc(SETL, reg);
            break;
        case 3:
            gc_setcc(SETGE, reg);
            break;
        case 4:
            gc_setcc(SETLE, reg);
            break;
        case 5:
            gc_setcc(SETG, reg);
            break;
        default:
            mark("error cond", NULL);
    }
    gc_movzx(reg);
}

void
gc_setopss(int cond, int reg) {
    switch(cond) {
        case 0:
            gc_setcc(SETE, reg);
            break;
        case 1:
            gc_setcc(SETNE, reg);
            gc_setcc(SETP, reg + 1);
            gc_binary_reg2reg(XORB, reg + 1, reg, BoolType->size, BoolType->form);
            break;
        case 2:
            gc_setcc(SETB, reg);
            break;
        case 3:
            gc_setcc(SETAE, reg);
            break;
        case 4:
            gc_setcc(SETBE, reg);
            break;
        case 5:
            gc_setcc(SETA, reg);
            break;
        default:
            mark("error cond", NULL);
    }
    gc_movzx(reg);
}

// https://en.wikibooks.org/wiki/X86_Assembly/Control_Flow
// https://stackoverflow.com/questions/28182827/useless-jp-jnp-assembly-instruction-on-x86-64
// https://stackoverflow.com/questions/51940715/difference-between-jp-and-jpe-or-jnp-and-jpo-in-assembly-language


void
gc_jmp(int lab) {
    char adr[16];
    sprintf(adr, "%c%d", LPREFIX, lab);
    sg_unary_operation(JMP, adr);
}

int
gc_jcmp(int cond, int lab) {
    if (lab == 0) {
        lab = label();
    }
    char *op;
    switch(cond) {
        case 0:
            op = JE;
            break;
        case 1:
            op = JNE;
            break;
        case 2:
            op = JL;
            break;
        case 3:
            op = JGE;
            break;
        case 4:
            op = JLE;
            break;
        case 5:
            op = JG;
            break;
        default:
            mark("error cond", NULL);
    }

    char adr[16];
    sprintf(adr, "%c%d", LPREFIX, lab);
    sg_unary_operation(op, adr);
    return lab;
}

void
save_rCX(int reg2, int reg1) {
    if (reg1 >= rCXI || reg2 >= rCXI) {
        sg_unary_operation(PUSH, rCX);
    }
}

void
restore_rCX(int reg2, int reg1) {
    if (reg1 >= rCXI || reg2 >= rCXI) {
        sg_unary_operation(POP, rCX);
    }
}

void
save_product_registers(int reg2, int reg1) {
    if (reg1 >= rAXI || reg2 >= rAXI) {
        sg_unary_operation(PUSH, rAX);
        sg_unary_operation(PUSH, rDX);
    } else if (reg1 >= rDXI || reg2 >= rDXI) {
        sg_unary_operation(PUSH, rDX);
    }
}

void
restore_product_registers(int reg2, int reg1) {
    if (reg1 >= rAXI || reg2 >= rAXI) {
        sg_unary_operation(POP, rDX);
        sg_unary_operation(POP, rAX);
    } else if (reg1 >= rDXI || reg2 >= rDXI) {
        sg_unary_operation(POP, rDX);
    }
}

void
gc_mul(int reg2, int reg1) {
    save_product_registers(reg2, reg1);
    
    sg_binary_operation(MOV, regs[reg1], rAX);
    sg_unary_operation(IMUL, regs[reg2]);
    sg_binary_operation(MOV, rAX, regs[reg1]);

    restore_product_registers(reg2, reg1);
}


/*
 * cltd;   
 * AT&T汇编里的cltd指令相当于cdq指令，
 * 作用是把eax的32位整数扩展为64位，
 * 高32位用eax的符号位填充保存到edx
 */
void
gc_div(int reg2, int reg1) {
    save_product_registers(reg2, reg1);

    sg_binary_operation(MOV, regs[reg1], rAX);
    sg_operator(CQO);
    sg_unary_operation(IDIV, regs[reg2]);
    sg_binary_operation(MOV, rAX, regs[reg1]);

    restore_product_registers(reg2, reg1);
}

void 
gc_mod(int reg2, int reg1) {
    save_product_registers(reg2, reg1);

    sg_binary_operation(MOV, regs[reg1], rAX);
    sg_operator(CQO);
    sg_unary_operation(IDIV, regs[reg2]);
    sg_binary_operation(MOV, rDX, regs[reg1]);

    restore_product_registers(reg2, reg1);
}

void 
gc_public(char *label) { 
    sg_unary_operation(GLOBL_LABEL, label);
}

void
gc_entry() {
    sg_unary_operation(PUSH, rBP);
    sg_binary_operation(MOV, rSP, rBP);
}

void
gc_exit(void) {
    sg_unary_operation(POP, rBP);
    sg_operator(RET);
}
