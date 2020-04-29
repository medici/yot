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
gcdata() {
    sg_operator(DATA_LABEL);
}

void
gctext() {
    sg_operator(TEXT_LABEL);
}

void gcprelude(void)	{ }
void gcpostlude(void)	{ }

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
gcstack(int n)	{
    n = gc_stack_align(n);
    ng("%s\t$%d, %s", "addq", n, "%rsp"); 
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
    ng("%s\t%d", ".byte", v, NULL); 
}

void
gc_def_long(long int v) {
    ng("%s\t%d", ".long", v, NULL); 
}

void
gcldlab(int id)	{
    lg("%s\t%c%d(%%rip), %%rax", "leaq", id); 
}

void
gc_def_quad(int v) { 
    ng("%s\t%d", ".quad", v, NULL);
}

void
gc_align(int num) {
    if (num % 2 == 1) {
        ng("%s\t$%d", "pushq", 0, NULL); 
    }
}

void
gc_popq_align(int num) {
    if (num % 2 == 1) {
        sg("popq\t%s", regs[0], NULL);
    }
}

void
gc_call(char *s) {
    sg("call\t%s", s, NULL);
}

void
gc_pushFPU(int reg) {
    sg("subq\t$16, %%rsp", NULL, NULL);
    sg("movdqu\t%s, (%%rsp)", FPURegs[reg], NULL);
}

void
gc_popFPU(int reg) {
    sg("movdqu\t(%%rsp), %s", FPURegs[reg], NULL);
    sg("addq\t$16, %%rsp", NULL, NULL);
}

void
gc_pushq(int reg) {
    if (reg >= DIVIDE_MASK) {
        gc_pushFPU(reg >> DIVIDE);
    } else {
        sg("pushq\t%s", regs[reg], NULL);
    }
}

void
gc_popq(int reg) {
    if (reg >= DIVIDE_MASK) {
        gc_popFPU(reg >> DIVIDE);
    } else {
        sg("popq\t%s", regs[reg], NULL);
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
    sg("movzx\t%s, %s", regs_byte[reg], regs[reg]);
}

void
gc_sete(int reg) {
    sg("sete\t%s", regs_byte[reg], NULL);
}

void
gc_setnp(int reg) {
    sg("setnp\t%s", regs_byte[reg], NULL);
}

void
gc_setne(int reg) {
    sg("setne\t%s", regs_byte[reg], NULL);
}

void
gc_setp(int reg) {
    sg("setp\t%s", regs_byte[reg], NULL);
}

void
gc_setl(int reg) {
    sg("setl\t%s", regs_byte[reg], NULL);
}

void
gc_setb(int reg) {
    sg("setb\t%s", regs_byte[reg], NULL);
}

void
gc_setge(int reg) {
    sg("setge\t%s", regs_byte[reg], NULL);
}

void
gc_setae(int reg) {
    sg("setae\t%s", regs_byte[reg], NULL);
}

void
gc_setle(int reg) {
    sg("setle\t%s", regs_byte[reg], NULL);
}

void
gc_setbe(int reg) {
    sg("setbe\t%s", regs_byte[reg], NULL);
}

void
gc_setg(int reg) {
    sg("setg\t%s", regs_byte[reg], NULL);
}

void
gc_seta(int reg) {
    sg("seta\t%s", regs_byte[reg], NULL);
}

void
gc_setop(int cond, int reg) {
    switch(cond) {
        case 0:
            gc_sete(reg);
            break;
        case 1:
            gc_setne(reg);
            break;
        case 2:
            gc_setl(reg);
            break;
        case 3:
            gc_setge(reg);
            break;
        case 4:
            gc_setle(reg);
            break;
        case 5:
            gc_setg(reg);
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
            gc_sete(reg);
            break;
        case 1:
            gc_setne(reg);
            gc_setp(reg + 1);
            gc_xorb(reg + 1, reg);
            break;
        case 2:
            gc_setb(reg);
            break;
        case 3:
            gc_setae(reg);
            break;
        case 4:
            gc_setbe(reg);
            break;
        case 5:
            gc_seta(reg);
            break;
        default:
            mark("error cond", NULL);
    }
    gc_movzx(reg);
}

void
gc_cmp(int reg2, int reg1) {
    sg("cmpq\t%s, %s", regs[reg2], regs[reg1]);
}

void
gc_cmp2(int reg) {
    sg("cmpq\t$0, %s", regs[reg], NULL);
}

void
gc_cmp3(int reg) {
    sg("cmpq\t$0, (%s)", regs[reg], NULL);
}


void
gc_ucomiss(int reg2, int reg1) {
    sg("ucomiss\t%s, %s", FPURegs[reg2], FPURegs[reg1]);
}

// https://en.wikibooks.org/wiki/X86_Assembly/Control_Flow
// https://stackoverflow.com/questions/28182827/useless-jp-jnp-assembly-instruction-on-x86-64
// https://stackoverflow.com/questions/51940715/difference-between-jp-and-jpe-or-jnp-and-jpo-in-assembly-language

void
gc_je() {
    graw("\tje");
}

void
gc_jnp() {
    graw("\tjnp");
}

void
gc_jne() {
    graw("\tjne");
}

void
gc_jp() {
    graw("\tjp");
}

void
gc_jl() {
    graw("\tjl");
}

void
gc_jb() {
    graw("\tjb");
}

void
gc_jge() {
    graw("\tjge");
}

void
gc_jae() {
    graw("\tjbe");
}

void
gc_jle() {
    graw("\tjle");
}

void
gc_jbe() {
    graw("\tjbe");
}

void
gc_jg() {
    graw("\tjg");
}

void
gc_ja() {
    graw("\tjb");
}

void
gc_jmp(int lab) {
    lg("%s\t%c%d", "jmp", lab);
}

int
gc_jcmp(int cond, int lab) {
    if (lab == 0) {
        lab = label();
    }
    switch(cond) {
        case 0:
            gc_je();
            break;
        case 1:
            gc_jne();
            break;
        case 2:
            gc_jl();
            break;
        case 3:
            gc_jge();
            break;
        case 4:
            gc_jle();
            break;
        case 5:
            gc_jg();
            break;
        default:
            mark("error cond", NULL);
    }
    lg("%s\t%c%d", "", lab);
    return lab;
}

void
gc_xorb(int reg2, int reg1) {
    sg("xorb\t%s, %s", regs_byte[reg2], regs_byte[reg1]);
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
gcpublic(char *s) { 
    sg("%s\t%s",GLOBL_LABEL, s); 
}

void
gcentry() {
    g("pushq\t%rbp");
   	g("movq\t%rsp,%rbp");
}

void
gcexit(void) { 
    g("popq\t%rbp");
    g("ret"); 
}
