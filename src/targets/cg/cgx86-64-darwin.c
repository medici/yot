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
    g(".data"); 
}

void
gctext() {
    g(".text");
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

void 
gc_def_char(int c) {
    ng("%s\t'%c'", ".byte", c, NULL); 
}

void
gc_def_byte(int v) { 
    ng("%s\t%d", ".byte", v, NULL); 
}

void
gc_long_int(long int v) {
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
gc_byte_literal(int reg, int v) {
    gc_xor2(reg, reg);
    ng("%s\t$%d, %s", "movb", v, regs_byte[reg]);
}

void
gc_integer_literal(int reg, int v) {
    ng("%s\t$%d, %s", "movq", v, regs[reg]);
}

void
gc_real_literal(int reg, int v) {
    lg2("movss\t%c%d(%%rip), %s", v, FPURegs[reg]);
}

void
gc_call(char *s) {
    sg("call\t%s", s, NULL);
}

void
gc_call2(char *s) {
    sg("call\t%s", g_symbol(s), NULL);
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

void
gc_cvtsi2ss(int rh, int rhf) {
    sg("cvtsi2ss\t%s, %s", regs[rh], FPURegs[rhf]);
}

void
gc_cvtss2si(int rhf, int rh) {
    sg("cvtss2si\t%s, %s", FPURegs[rhf], regs[rh]);
}

void
lgc_cvtss2si(int lab, int rh) {
    lg2("cvtss2si\t%c%d(%%rip), %s", lab, regs[rh]);
}

/*

FLD source
中source可以为32 ，64或者80位整数值．
为了区分这两种长度，GNU汇编器使用FLDS加载单精度浮点数,FLDL加载双精度浮点数，
类似FST用于获取FPU寄存器堆栈中顶部的值, 并且把这个值放到内存位置中, 
对于单精度使用FSTS, 对于双精度使用FSTL

*/
void
gc_flds(int offset) {
    ng("%s\t%d(%%rbp)", "flds", offset, NULL);
}

void
gc_fldt(int offset) {
    ng("%s\t%d(%%rbp)", "fldt", offset, NULL);
}

void
gc_flds2(char *name) {
    sg("%s\t%s(%%rip)", "flds", g_symbol(name));
}

void 
gc_fldt2(char *name) {
    sg("%s\t%s(%%rip)", "fldt", g_symbol(name));
}

void
gc_flds3(int reg) {
    sg("movss\t%s, (%%rsp)", FPURegs[reg], NULL); 
    sg("%s\t(%%rsp)", "flds", NULL);
}

void 
gc_fldt3(int reg) {
    sg("movss\t%s, (%%rsp)", FPURegs[reg], NULL); 
    sg("%s\t(%%rsp)", "fldt", NULL);
}

void
gc_frndint() {
    sg("frndint", NULL, NULL);
}

void
gc_fsts(int offset) {
    ng("%s\t%d(%%rbp)", "fsts", offset, NULL);
}

void
gc_fstl(int offset) {
    ng("%s\t%d(%%rbp)", "fstl", offset, NULL);
}

void
gc_fsts2(char *name) {
    sg("%s\t%s(%%rip)", "fsts", g_symbol(name));
}

void
gc_fstl2(char *name) {
    sg("%s\t%s(%%rip)", "fstl", g_symbol(name));
}

void
gc_fsts3(int reg) {
    sg("movss\t(%%rsp), %s", FPURegs[reg], NULL); 
    sg("%s\t(%%rsp)", "fsts", NULL);
}

void
gc_fstl3(int reg) {
    sg("movss\t(%%rsp), %s", FPURegs[reg], NULL); 
    sg("%s\t(%%rsp)", "fstl", NULL);
}

void
gc_fabs() {
    sg("fabs", NULL, NULL);
}

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
gc_mov(int reg2, int reg1) {
    sg("movq\t%s, %s", regs[reg2], regs[reg1]);
}

void 
gc_shrl(int bit, int reg) {
    ng("%s\t$%d, %s", "shrl", bit, regs_32[reg]);
}

void 
gc_xor(int reg) {
    sg("xorq\t$1, %s", regs[reg], NULL);
}

void 
gc_xor2(int reg2, int reg1) {
    sg("xorq\t%s, %s", regs[reg2], regs[reg1]);
}

void
gc_xorb(int reg2, int reg1) {
    sg("xorb\t%s, %s", regs_byte[reg2], regs_byte[reg1]);
}

void
gc_add(int reg2, int reg1) {
    sg("addq\t%s, %s", regs[reg2], regs[reg1]);
}

void
gc_add2(int v, int reg1) {
    ng("%s\t$%d, %s", "addq", v, regs[reg1]);
}

void
gc_sub(int reg2, int reg1) {
    sg("subq\t%s, %s", regs[reg2], regs[reg1]);
}

void
gc_mul(int reg2, int reg1) {
    if (reg1 > 5 || reg2 > 5) {
        g("pushq\t%rax");
        g("pushq\t%rdx");
    } else if (reg1 > 2 || reg2 > 2) {
        g("pushq\t%rdx");
    }
    
    sg("movq\t%s, %s", regs[reg1], "%rax");
    sg("imulq\t%s", regs[reg2], NULL);
    sg("movq\t%s, %s", "%rax", regs[reg1]);
    if (reg1 > 5 || reg2 > 5) {
        g("popq\t%rdx");
        g("popq\t%rax");
    } else if (reg1 > 2 || reg2 > 2) {
        g("popq\t%rdx");
    }
}

void
gc_mul2(int v, int reg) {
    ng("%s\t$%d, %s", "imulq", v, regs[reg]);
}

void
gc_div(int reg2, int reg1) {
    if (reg1 > 5 || reg2 > 5) {
        g("pushq\t%rax");
        g("pushq\t%rdx");
    } else if (reg1 > 2 || reg2 > 2) {
        g("pushq\t%rdx");
    }
    
    sg("movq\t%s, %s", regs[reg1], "%rax");
    g("cqo"); //  cltd   ;   AT&T汇编里的cltd指令相当于cdq指令，作用是把eax的32位整数扩展为64位，高32位用eax的符号位填充保存到edx
    sg("idivq\t%s", regs[reg2], NULL);
    sg("movq\t%s, %s", "%rax", regs[reg1]);
    if (reg1 > 5 || reg2 > 5) {
        g("popq\t%rdx");
        g("popq\t%rax");
    } else if (reg1 > 2 || reg2 > 2) {
        g("popq\t%rdx");
    }
}

void 
gc_mod(int reg2, int reg1) {
    if (reg1 > 5 || reg2 > 5) {
        g("pushq\t%rax");
        g("pushq\t%rdx");
    } else if (reg1 > 2 || reg2 > 2) {
        g("pushq\t%rdx");
    }
    sg("movq\t%s, %s", regs[reg1], "%rax");
    g("cqo");
    sg("idivq\t%s", regs[reg2], NULL);
    sg("movq\t%s, %s", "%rdx", regs[reg1]);
    if (reg1 > 5 || reg2 > 5) {
        g("popq\t%rdx");
        g("popq\t%rax");
    } else if (reg1 > 2 || reg2 > 2) {
        g("popq\t%rdx");
    }
}

void
gc_leaq(int offset, int reg) {
    ng("%s\t%d(%%rbp), %s", "leaq", offset, regs[reg]); 
}

void
gc_leaq2(int lab, int reg) {
    lg2("leaq\t%c%d(%%rip), %s", lab, regs[reg]);
}

void
gc_leaq3(char *name, int reg) {
    sg("leaq\t%s(%%rip), %s", name, regs[reg]);
}

void
gc_addss(int reg2, int reg1) {
    int RHF = rhf();
    if (RHF - 2 != reg1) {
        sg2("vaddss\t%s, %s, %s", FPURegs[reg2], FPURegs[reg1], FPURegs[RHF - 2]);
    } else {
        sg("addss\t%s, %s", FPURegs[reg2], FPURegs[reg1]);
    }
}

void
gc_subss(int reg2, int reg1) {
    int RHF = rhf();
    if (RHF - 2 != reg1) {
        sg2("vsubss\t%s, %s, %s", FPURegs[reg2], FPURegs[reg1], FPURegs[RHF - 2]);
    } else {
        sg("subss\t%s, %s", FPURegs[reg2], FPURegs[reg1]);
    }
}

void
gc_mulss(int reg2, int reg1) {
    int RHF = rhf();
    if (RHF -2  != reg1) {
        sg2("vmulss\t%s, %s, %s", FPURegs[reg2], FPURegs[reg1], FPURegs[RHF - 2]);
    } else {
        sg("mulss\t%s, %s", FPURegs[reg2], FPURegs[reg1]);
    }
}

void
gc_divss(int reg2, int reg1) {
    int RHF = rhf();
    if (RHF - 2 != reg1) {
        sg2("vdivss\t%s, %s, %s", FPURegs[reg2], FPURegs[reg1], FPURegs[RHF - 2]);
    } else {
        sg("divss\t%s, %s", FPURegs[reg2], FPURegs[reg1]);
    }
}

void 
gc_and(int reg2, int reg1) {
    sg("andq\t%s, %s", regs[reg2], regs[reg1]);
}

void
gc_xchgq(int reg1, int reg2) {
    sg("xchgq\t%s, %s", regs[reg1], regs[reg2]);
}

void
sar(int count, int reg) {
    ng("%s\t$%d, %s", "sarq", count, regs[reg]);
}

void
gc_shift(int op, int count, int reg) {
    switch(op) {
        case 1:
            sar(count, reg);
        default:
            break;
    }
}
void 
gc_load(int offset, int reg) {
    ng("%s\t%d(%%rbp), %s", "movq", offset, regs[reg]); 
}

void 
gc_load_integer(int v, int reg) {
    ng("%s\t$%d, %s", "movq", v, regs[reg]); 
}

void 
gc_load_uinteger(unsigned int v, int reg) {
    ng("%s\t$%u, %s", "movq", v, regs[reg]); 
}

void
gc_load_imm(int reg2, int reg1) {
    sg("movq\t(%s), %s", regs[reg2], regs[reg1]); 
}

void
gc_load_immsb(int reg2, int reg1) {
    sg("movsb\t(%s), %s", regs[reg2], regs[reg1]); 
}

void
gc_load_imm2(int reg2, int reg1) {
    sg("movss\t(%s), %s", regs[reg2], FPURegs[reg1]); 
}

void
gc_load_offset(int reg1) {
    sg("addq\t%%rbp, %s", regs[reg1], NULL);
}

void
gc_load_offset2(int reg2, int reg1) {
    sg("addq\t%s, %s", regs[reg2], regs[reg1]);
}

void 
gc_loadss(int offset, int reg) {
    ng("%s\t%d(%%rbp), %s", "movss", offset, FPURegs[reg]); 
}

void 
gc_loadsb(int offset, int reg) {
    ng("%s\t%d(%%rbp), %s", "movsb", offset, regs[reg]); 
}

void
gc_loadb(int reg2, int reg1) {
    sg("movb\t(%s), %s", regs[reg2], regs_byte[reg1]); 
}

void
gc_load_globl(char *name, int rh) {
    sg("movq\t%s(%%rip), %s", name, regs[rh]);
}

void 
gc_load_globlss(char *name, int rhf) {
    sg("movss\t%s(%%rip), %s", name, FPURegs[rhf]);
}

void 
gc_load_globlsb(char *name, int rh) {
    sg("movsb\t%s(%%rip), %s", name, regs[rh]);
}

void
gc_store_reg(int reg, int v) {
    ng("movq\t%s, %d(%s)", regs[reg], v, "%rbp"); 
}

void
gc_store_regss(int reg, int v) {
    ng("movss\t%s, %d(%s)", FPURegs[reg], v, "%rbp"); 
}

void
gc_store_integer_literal(int v, int offset) {
    ng2("%s\t$%d, %d(%%rbp)", "movq", v, offset); 
}

void 
gc_store_byte_literal(int v, int offset) {
    ng2("%s\t$%d, %d(%%rbp)", "movb", v, offset); 
}

void
gc_store_regsb(int reg, int v) {

}

void
gc_storeb(int reg2, int reg1) {
    sg("movb\t%s, (%s)", regs_byte[reg2], regs[reg1]); 
}


void
gc_store_reg2globl(int rh, char *name) {
    sg("movq\t%s, %s(%%rip)", regs[rh], g_symbol(name));
}

void
gc_store_reg2globlss(int rh, char *name) {
    sg("movss\t%s, %s(%%rip)", FPURegs[rh], g_symbol(name));
}

void
gc_store_reg2globlb(int reg, char *name) {
    sg("movb\t%s, %s(%%rip)", regs_byte[reg], g_symbol(name));
}

void
gc_store_reg2imm(int reg2, int reg1) {
    // sg("addq\t%%rbp, %s", regs[reg1], NULL);
    sg("movq\t%s, (%s)", regs[reg2], regs[reg1]); 
}

void 
gc_store_regss2imm(int reg2, int reg1) {
    sg("movss\t%s, (%s)", FPURegs[reg2], regs[reg1]);
}

void 
gc_store_bool_literal(int v, char*name) {
    ng("%s\t$%d, %s(%%rip)", "movb", v, g_symbol(name));
}

void 
gcpublic(char *s) { 
    ng(".globl\t%s", s, 0, NULL); 
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
