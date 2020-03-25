#include <ctype.h>
#include "declare.h"
#include "data.h"
#include "define.h"
#include "parser.h"
#include "generator.h"
#include "generate_code.h"
#include "scan.h"

int RH;  // normal, integer
int RHF; // float

void
incR() {
    ++RH;
}

void
incFPUR() {
    ++RHF;
}

void
open() {
    PC = 0;
    RH = 0;
    RHF = 0;
}

void 
gdata(void) {
	if (Textseg) gcdata();
	Textseg = 0;
}

void
gtext() {
    if (!Textseg) gctext();
    Textseg = 1;
}


void
gentry() {
    gcentry();
}

void 
gstack(int n) {
	if (n) {
		gtext();
		gcstack(n);
	}
}

void
gparamters(int nofpar, Object params) {
    int i = 0;
    int j = 0;
    int f = 0;
    int offset = 0;
    for (; i < nofpar; ++i) {
        if (params->type->form == FINTEGER) {
            offset -= params->type->size * ( j + 1);
            gc_store_reg(j, offset);
            ++j;
        } else if (params->type->form == FREAL) {
            offset -= params->type->size * ( j + 1);
            gc_store_regss(f, offset);
            ++f;
        } else if (params->type->form == FARRAY) {
            offset -= WORDSIZE * ( j + 2);
            gc_store_reg(j, offset);
            gc_store_reg(j + 1, offset + WORDSIZE);
            ++j;
            ++j;
        } else if (params->type->form == FBOOLEAN) {
            offset -= params->type->size * ( j + 1);
            gc_store_reg(j, offset);
            ++j;
        }
        params = params->next;
    }
}

void
gexit(void) {
	gtext();
	gcexit();
}

void
ng(char *s, char *inst, int n, char *reg) {
    if (NULL == Outfile) return;
    fputc('\t', Outfile);
    fprintf(Outfile, s, inst, n, reg);
    fputc('\n', Outfile);
}

void
ng2(char *s, char *inst, int v, int offset) {
    if (NULL == Outfile) return;
    fputc('\t', Outfile);
    fprintf(Outfile, s, inst, v, offset);
    fputc('\n', Outfile);
}

void 
lg(char *s, char *inst, int n) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, LPREFIX, n);
	fputc('\n', Outfile);
}

void
lg2(char *s, int n, char *inst) {
    if (NULL == Outfile) return;
    fputc('\t', Outfile);
    fprintf(Outfile, s, LPREFIX, n, inst);
    fputc('\n', Outfile);
}

int
rh() {
    return RH;
}
int
rhf() {
    return RHF;
}

void
check_regs() {
    if (RH !=0 || RHF !=0) {
        mark("reg stack", NULL);
        printf("RH %d, RHF %d\n", RH, RHF);
        RH = 0;
        RHF = 0;
    }
}

char*
g_symbol(char *s) {
	static char	name[TEXTLEN+2];

	name[0] = PREFIX;
	copy_ident(&name[1], s);
	return name;
}

void
graw(char *s) {
    if (NULL == Outfile) return;
    fprintf(Outfile, "%s", s);
}

void
g_name(char *name) {
    graw(g_symbol(name));
    graw(":");
}

void
g_load_globl(char *name, int rh) {
    gc_load_globl(g_symbol(name), rh);
}

void
g_load_globlss(char *name, int rhf) {
    gc_load_globlss(g_symbol(name), rhf);
}

void
g_load_globlsb(char *name, int rh) {
    gc_load_globlsb(g_symbol(name), rh);
}

void
gpublic(char *procid) {
    gcpublic(g_symbol(procid));
}

void
glab(int id) {
    if (NULL == Outfile) return;
    fprintf(Outfile, "%c%d:\n", LPREFIX, id);
}

void
g_def_string(char *s, int len) {
    int i;
    gdata();
    for (i=0; i<len; i++) {
		gc_def_byte(s[i] + 0);
	}
}

void
g_def_byte(int v) {
	gdata();
	gc_def_byte(v);
}

void
g_def_quad(int v) {
    gdata();
    gc_def_quad(v);
}

void
g_def_IEEE754_float(long int v) {
    gc_long_int(v);
}

void 
gcstorlw(int n) { 
    ng("%s\t%%rax,%d(%%rbp)", "movq", n, NULL); 
}

void
galign(int k) {
	gdata();
	while (k++ % INTSIZE)
		gc_def_byte(0);
}

void
gprelude(void) {
	gcprelude();
	Textseg = 0;
	gtext();
}

void
gpostlude(void) {
	gcpostlude();
}

void 
g(char *s) {
    if (NULL == Outfile) return;
    fprintf(Outfile, "\t%s\n", s);
}

void
sg(char *s, char *inst, char *s2) {
    if (NULL == Outfile) return;
    fputc('\t', Outfile);
    fprintf(Outfile, s, inst, s2);
    fputc('\n', Outfile);
}
void
sg2(char *s, char *inst, char *s2, char *s3) {
    if (NULL == Outfile) return;
    fputc('\t', Outfile);
    fprintf(Outfile, s, inst, s2, s3);
    fputc('\n', Outfile);
}


int
label(void) {
	static int id = 1;

	return id++;
}

void
g_cvtsi2ss(Item x) {
    x->type = RealType;
    gc_cvtsi2ss(x->r, RHF);
    x->r = RHF;
    incFPUR();
    --RH;
}

void
g_cvtss2si(Item x) {
    x->type = IntType;
    gc_cvtss2si(x->r, RH);
    x->r = RH;
    incR();
    --RHF;
}

void
lg_cvtss2si(Item x) {
    int lab = label();
    gdata();
    glab(lab);
    g_def_IEEE754_float(x->a);
    gtext();
    x->type = IntType;
    lgc_cvtss2si(lab, RH);
    x->mode = CREG;
    x->r = RH;
    incR();
}

void
gc_store_lab(int lab, int offset) {
    gcldlab(lab);
    gcstorlw(offset);
}

void
g_load(int offset, int RH) {
    gc_load(offset, RH);
}

void
g_loadss(int offset, int RH) {
    gc_loadss(offset, RH);
}

void 
g_setne(Item x) {
    gc_setne(x->r);
    gc_movzx(x->r);
}

void
g_cmp2(int reg) {
    gc_cmp2(reg);
}

void
load_compare_expression(Item x) {
    if (x->mode == CCONST) {
        load(x);
        g_cmp2(x->r);
    } else {
        if (x->a != 0) {
            Fixup(x->a);
        }

        if (x->a != 0 && x->b !=0) {
            FJump(x->b);
        }

        //g_setne(y);
        if (x->b != 0) {
            Fixup(x->b);
        }
    }
    g_setne(x);
}

Item
make_item(Object y) {
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
    } else if (y->class == CCONST && y->type->form == CSTRING) {
        x->b = y->level;
    } else {
        x->r = y->level;
    }

    if (y->level > 0 && y->level != CurrentLevel && y->class != CCONST) {
        mark("%s not accessible ", Text);
    }
    
    return x;
}

Item
make_const_item(int value, Type tp) {
    Item x = checked_malloc(sizeof(*x));
    x->mode = CCONST;
    x->type = tp;
    x->name[0] = '\0';
    x->a = value;
    return x;
}

Item
make_real_item(float value) {
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


Item
make_strlit_item(char *value) {
    int lab = label(), k = 0;
    gdata();
    glab(lab);
    g_def_string(Text, Value);
    k += Value - 1;
    gc_def_byte(0);
    galign(k+1);
    
    Item x = checked_malloc(sizeof(*x));
    x->mode = CCONST;
    x->type = StringType;
    x->a = lab;
    x->b = Value;
    
    return x;
}

void
load(Item x) {
    gtext();
    if (x->mode != CREG) {
        if (x->mode == CCONST) {
            if (x->type == IntType || x->type->form == FCHAR) {
                gc_integer_literal(RH, x->a);
                x->r = RH;
                incR();
            } else if (x->type == RealType) {
                if (x->r > 0) { // local 
                    gc_real_literal(RHF, x->a);
                    x->r = RHF;
                    incFPUR();
                } else if(x->name[0] == '\0') {
                    int lab = label();
                    gdata();
                    glab(lab);
                    g_def_IEEE754_float(x->a);
                    gtext();
                    gc_real_literal(RHF, lab);
                    x->r = RHF;
                    incFPUR();
                } else {
                    g_load_globlss(x->name, RHF);
                    x->r = RHF;
                    incFPUR();
                }
            } else if (x->type->form == FBOOLEAN) {
                gc_byte_literal(RH, x->a);
                x->r = RH;
                incR();
            }
        } 
        // else if(x->mode == CCVAR) {
        //     g_load_globlss(x->name, RHF);
        //     x->r = RHF;
        //     incFPUR();
        // } 
        // else if(x->mode == CLAB) {
        //     if (x->type->form == FREAL) {
        //         gc_real_literal(RHF, x->a);
        //         x->r = RHF;
        //         incFPUR();
        //     }
        // } 
        else if (x->mode == CVAR) {
            if (x->r > 0) {
                if (x->type == IntType) {
                    g_load(x->a, RH);
                    x->r = RH;
                    incR();
                } else if(x->type == CharType) {
                    gc_loadsb(x->a, RH);
                    x->r = RH;
                    incR();
                } else if (x->type == RealType) {
                    g_loadss(x->a, RHF);
                    x->r = RHF;
                    incFPUR();
                } else if (x->type->form == FBOOLEAN) {
                    gc_loadsb(x->a, RH);
                    x->r = RH;
                    incR();
                }
            } else {
                if (x->type == IntType) {
                    g_load_globl(x->name, RH);
                    x->r = RH;
                    incR();
                } else if (x->type == RealType) {
                    g_load_globlss(x->name, RHF);
                    x->r = RHF;
                    incFPUR();
                } else if(x->type == BoolType || x->type->form == FCHAR) {
                    g_load_globlsb(x->name, RH);
                    x->r = RH;
                    incR();
                }
            }
        } else if (x->mode == CREGI) {
             if (x->type->form == FREAL) {
                gc_load_imm2(x->r, RHF);
                --RH;
                incFPUR();
             } else if (x->type->form == FCHAR) {
                gc_load_immsb(x->r, x->r);
             } else {
                gc_load_imm(x->r, x->r);
             }
        } else if (x->mode == CCOND) {
            x->r = RH;
            incR();
            //incR();
        }
        // else if (x->mode == CMVAR) {
        //     if (x->type == IntType) {
        //         g_load_globl(x->name, RH);
        //         x->r = RH;
        //         incR();
        //     } else if (x->type == RealType) {
        //         g_load_globlss(x->name, RHF);
        //         x->r = RHF;
        //         incFPUR();
        //     } else if(x->type == BoolType) {
        //         g_load_globlsb(x->name, RH);
        //         x->r = RH;
        //         incR();
        //     }
        // }
        x->mode = CREG;
    }
    
}

void
load_adr(Item x) {
    if (x->mode == CVAR) {
        if (x->r > 0) {
            x->r = RH;
            gc_leaq(x->a, x->r);
        } else {
            x->r = RH;
            gc_leaq3(g_symbol(x->name), x->r);
        }
        incR();
    }
    x->mode = CREG;
}

void
load_string_adr(Item x) {
    gtext();
    x->r = RH;
    gc_leaq2(x->a, x->r);
    incR();
}

void
load_cond(Item x) {
    if (x->type->form == FBOOLEAN) {
        load(x);
        gc_cmp2(x->r);
        x->c = NEQ - EQL;
        
        x->mode = CCOND;
        x->a = 0;
        x->b = 0;
        --RH;
    } else {
        mark("%s not Boolean?", x->name);
    }
}

void
save_regs(int r) {
    int r0 = 0;
    do {
        gc_pushq(r0);
        ++r0;
    } while(r0 != r);
}

void
save_fpu_regs(int r) {
    int r0 = 0;
    do {
        gc_pushq(r0 << DIVIDE | DIVIDE_MASK);
        ++r0;
    } while(r0 != r);
}

void
restore_regs(int r) {
    int r0 = r;
    while (r0 != 0) {
        --r0;
        gc_popq(r0);
    }
}
void
restore_fpu_regs(int r) {
    int r0 = r;
    while (r0 != 0) {
        --r0;
        gc_popq(r0 << DIVIDE | DIVIDE_MASK);
    }
}

void
call(Item x, int r[2], char *name) {
    gc_call2(name);

    Object dsc = x->type->dsc;
    int nofpar = x->type->nofpar;
    int j;
    for (j =0; j<nofpar; j++) {
        if (dsc->type->form == FINTEGER || dsc->type->form == FBOOLEAN) {
            RH = RH - 1;
        } else if (dsc->type->form == FREAL) {
            RHF = RHF - 1;
        } else if (dsc->type->form == FARRAY) {
            RH = RH - 2;
        }
        dsc = dsc->next;
    }
    restore_regs(r[0]);
}

void
neg(Item x) {
    if (x->type->form == FINTEGER) {
        if (x->mode == CCONST) {
            x->a = -x->a;
        } else {
            load(x);
            gc_load_integer(0, RH);
            gc_sub(x->r, RH);
            gc_xchgq(RH, x->r);
        }
    }
}

int*
pre_call(Item x) {
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

int 
negated(int cond) {
    if (cond % 2 == 0) {
        ++cond;
    } else {
        --cond;
    }

    return cond;
}

void
setCC(Item x, int op) {
    // x->mode = CCOND;
    // x->a = 0;
    // x->b = 0;
    // x->c = op;
    gc_cmp2(x->r);
    x->c = NEQ - EQL;
    
    x->mode = CCOND;
    x->a = 0;
    x->b = 0;
}

void 
int_relation(int op, Item x, Item y) {
    if (x->mode == CCOND || y->mode == CCOND) {
        mark("not implemented", NULL);
    }
    load(x);
    load(y);
    gc_cmp(y->r, x->r);
    gc_setop(op - EQL, x->r);
    --RH;
    --RH;
    setCC(x, op - EQL);
}

void
real_relation(int op, Item x, Item y) {
    if (x->mode == CCOND || y->mode == CCOND) {
        mark("not implemented", NULL);
    }
    load(x);
    load(y);
    gc_ucomiss(y->r, x->r);
    gc_setopss(op - EQL, x->r);
    --RHF;
    --RHF;
    setCC(x, op - EQL);
}

void
Fixup(int lab) {
    glab(lab);
}

void
FJump(int lab) {
    gc_jmp(lab);
}

void
CFJump(Item x) {
    if (x->mode != CCOND) {
        load_cond(x);
    }
    x->a = gc_jcmp(negated(x->c), x->a);
}

void
CBJump(Item x, int lab) {
    if (x->mode != CCOND) {
        load_cond(x);
    }
    x->a = gc_jcmp(negated(x->c), lab);
}

void
For0(Item x, Item y) {
    load(y);
}

void
For1(Item x, Item y, Item z, Item w, int lab) {
    load(z);
    gc_cmp(z->r, y->r);
    --RH;

    if (w->a > 0) {
        lab = gc_jcmp(GTR - EQL, lab);
    } else if(w->a < 0) {
        lab = gc_jcmp(LSS - EQL, lab);
    } else {
        mark("zero increment", NULL);
    }

    store(x, y);
}

void
For2(Item x, Item y, Item w) {
    load(x);
    --RH;
    gc_add2(w->a, x->r);
}

void
real_op(int op, Item x, Item y) {
    load(x);
    load(y);
    if (op == RDIV) {
        gc_divss(y->r, x->r);
        --RHF;
    } else if(op == TIMES) {
        gc_mulss(y->r, x->r);
        --RHF;
    } else if(op == PLUS) {
        gc_addss(y->r, x->r);
        --RHF;
    } else if(op == MINUS) {
        gc_subss(y->r, x->r);
        --RHF;
    }
    x->r = RHF - 1;
}

void
add_op(int op, Item x, Item y) {
    if (op == PLUS) {
        load(x);
        load(y);
        gc_add(y->r, x->r);
        --RH;
    } else {
        load(x);
        load(y);
        gc_sub(y->r, x->r);
        --RH;
    }
}

void
mul_op(Item x, Item y) {
    load(x);
    load(y);
    gc_mul(y->r, x->r);
    --RH;
}

void
div_op(int op, Item x, Item y) {
    if (op == DIV) {
        load(x);
        load(y);
        gc_div(y->r, x->r);
        --RH;
    } else {
        load(x);
        load(y);
        gc_mod(y->r, x->r);
        --RH;
    }
}

void
store(Item x, Item y) {
    switch(x->mode) {
        case CVAR:
            if (x->r > 0) { //local
                if (y->mode == CCONST && x->type->form == FINTEGER) {
                    gc_store_integer_literal(y->a, x->a);
                } else if (x->type->form == FBOOLEAN && y->mode == CCONST) {
                    gc_store_byte_literal(y->a, x->a);
                } else if (x->type->form == FCHAR && y->mode == CCONST) {
                    gc_store_byte_literal(y->a, x->a);
                } else {
                    load(y);
                    gtext();
                    if (y->mode == CREG) {
                        switch(x->type->form) {
                            case FBOOLEAN:
                                gc_store_reg(y->r, x->a);
                                --RH;
                                break;
                            case FINTEGER:
                                gc_store_reg(y->r, x->a);
                                --RH;
                                break;
                            case FSTRING:
                                gc_store_lab(y->a, x->a);
                                --RH;
                                break;
                            case FREAL:
                                gc_store_regss(y->r, x->a);
                                --RHF;
                                break;
                        }
                    }
                }
            } else {
                if (x->type->form == FBOOLEAN && y->mode == CCONST) {
                    gc_store_bool_literal(y->a, x->name);
                } else {
                    load(y);
                    gtext();
                    if (y->type == IntType) {
                        gc_store_reg2globl(y->r, x->name);
                        --RH;
                    } else if (y->type == RealType) {
                        gc_store_reg2globlss(y->r, x->name);
                        --RHF;
                    } else if (x->type->form == FBOOLEAN || x->type->form == FCHAR) {
                        gc_store_reg2globlb(y->r, x->name);
                        --RH;
                    }
                }
            }
            break;
        // case CMVAR:
            
        //     load(y);
        //     gtext();
        //     if (y->type == IntType) {
        //         gc_store_reg2globl(y->r, x->name);
        //         --RH;
        //     } else if (y->type == RealType) {
        //         gc_store_reg2globlss(y->r, x->name);
        //         --RHF;
        //     } else if (x->type->form == FBOOLEAN) {
        //         gc_store_reg2globlsb(y->r, x->name);
        //         --RH;
        //     }
        //     break;
        case CREGI:
            load(y);
            if (x->type->form == FREAL) {
                gc_store_regss2imm(y->r, x->r);
                --RHF;
            } else {
                gc_store_reg2imm(y->r, x->r);
                --RH;
            }
            --RH;
            break;
        default:
            break;
    } 
}

void
arr_index(Item x, Item y) {
    int lim = x->type->len;
    int size = x->type->base->size;

    if (y->mode == CCONST && lim > 0) {
        if (y->a < 0 || y->a >= lim) {
            char    buf[32];
            sprintf(buf, "%ld", y->a);
            mark("%s bad index", buf); 
        }

        if (x->mode == CVAR) {
            x->a = x->a + y->a * size;
        }
    } else {
        load(y);
        gc_mul2(size, y->r);
        if (x->mode == CVAR) {
            gc_add2(x->a, y->r);
            gc_load_offset(y->r);
            x->r = y->r;
            x->mode = CREGI;
        } else if (x->mode == CPAR) {
            g_load(x->a, RH);
            x->r = RH;
            gc_load_offset2(x->r, y->r);
            x->r = y->r;
            x->mode = CREGI;
        } else if (x->mode == CREGI) {
            printf("CREGI\n");
        }
    }
}

void
field(Item x, Object y) {
    if (x->mode == CVAR) {
        if (x->r > 0) {
            x->a = x->a + y->value;
        } else {
            load_adr(x);
            gc_add2(y->value, x->r);
            x->mode = CREGI;
        }
    }
}

void
copy_string(Item x, Item y) {
    gtext();
    int len = x->type->len;
    load_adr(x);

    if (len >= 0) {
        if (len < y->b) {
            mark("%s string too long", x->name);
        }
    }

    load_string_adr(y);
    gc_xor2(RH, RH);
    int lab = label();
    Fixup(lab);
    gc_loadb(y->r, RH);
    gc_storeb(RH, x->r);
    gc_add2(1, y->r);
    gc_add2(1, x->r);
    gc_cmp2(RH);
    gc_jcmp(NEQ - EQL, lab);
    RH = RH - 2;
}

void
str_to_char(Item x) {
    x->type = CharType;
    x->a = Text[0];
}

void
open_array_param(Item x) {
    load_adr(x);
    if (x->type->len >= 0) {
        gc_load_integer(x->type->len, RH);
        incR();
    } else {

    }
}

void
string_parameter(Item x) {
    load_string_adr(x);
    gc_load_integer(x->b, RH);
    incR();
}

int 
merged(int L0, int L1) {
    if (L0 != 0) {
        Fixup(L0);
        gc_jmp(L1);
    } 
    return L1;
}

void
prologue(Object proc, int parblksize, int locblksize) {
    gtext();
    if(CurrentLevel == 1) {
        gpublic(proc->name);
    }
    g_name(proc->name);
    gentry();
    gstack(-locblksize);
    gparamters(proc->type->nofpar, proc->type->dsc);
}

void
epilogue(int locblksize) {
    gstack(locblksize);
    gexit();
}

void
not(Item x) {
    long int t;
    if (x->mode != CCOND) {
        load_cond(x);
    }
    gc_xor(x->r);
    gc_cmp2(x->r);
}

void
or1(Item x) {
    if (x->mode != CCOND) {
        load_cond(x);
    }
    x->b = gc_jcmp(x->c, x->b);
    if (x->a != 0) {
        Fixup(x->a);
    }
    x->a = 0;
}

void
or2(Item x, Item y) {
    if (y->mode != CCOND) {
        load_cond(y);
    }
    x->a = y->a;
    x->b = merged(y->b, x->b);
    x->c = y->c;
}

void
and1(Item x) {
    if (x->mode != CCOND) {
        load_cond(x);
    }

    x->a = gc_jcmp(negated(x->c), x->a);
    if (x->b != 0) {
        Fixup(x->b);
    }
    x->b = 0;
}

void
and2(Item x, Item y) {
    if (y->mode != CCOND) {
        load_cond(y);
    }
    x->a = merged(y->a, x->a);
    x->b = y->b;
    x->c = y->c;
}

// https://helloacm.com/optimized-abs-function-in-assembly/
// https://stackoverflow.com/questions/2639173/x86-assembly-abs-implementation
void
Abs(Item x) {
    if (x->mode == CCONST && x->type->form == FINTEGER) {
        x->a = -x->a;
    } else {
        if (x->type->form == FREAL) {
            if (x->mode == CVAR) {
                if (x->r > 0) { 
                    gc_flds(x->a);
                    gc_fabs();
                    gc_fsts(x->a);
                    x->type = IntType;
                } else {
                    gc_flds2(x->name);
                    gc_fabs();
                    gc_fsts2(x->name);
                }
            } else if (x->mode == CCONST) {
                gcstack(-8);
                load(x);
                gc_flds3(x->r);
                gc_fabs();
                gc_fsts3(x->r);
                gcstack(8);
            } else if (x->mode == CREG) {
                gcstack(-8);
                gc_flds3(x->r);
                gc_fabs();
                gc_fsts3(x->r);
                gcstack(8);
            }
            x->type = RealType;
        } else {
            load(x);
            int reg = x->r;
            if (reg > 5) {
                g("pushq\t%rax");
                g("pushq\t%rdx");
            } else if (reg > 2) {
                g("pushq\t%rdx");
            }
            gc_mov(reg, 6);
            g("cqo");
            gc_xor2(3, 6);
            gc_sub(3, 6);
            gc_mov(6, reg);
            if (reg > 5) {
                g("popq\t%rdx");
                g("popq\t%rax");
            } else if (reg > 2) {
                g("popq\t%rdx");
            }
        }
    }
}

void
Floor(Item x) {
    if (x->mode == CVAR) {
        if (x->r > 0) { 
            gc_flds(x->a);
            gc_frndint();
            gc_fsts(x->a);
            load(x);
            gc_cvtss2si(x->r, RH);
            --RHF;
            x->r = RH;
            incR();
            x->type = IntType;
        } else {
            gc_flds2(x->name);
            gc_frndint();
            gc_fsts2(x->name);
            load(x);
            gc_cvtss2si(x->r, RH);
            --RHF;
            x->r = RH;
            incR();
            x->type = IntType;
        }
    } else if (x->mode == CCONST) {
        gcstack(-8);
        load(x);
        gc_flds3(x->r);
        gc_frndint();
        gc_fsts3(x->r);
        gc_cvtss2si(x->r, RH);
        x->r = RH;
        incR();
        gcstack(8);
        x->type = IntType;
        --RHF;
    } else if (x->mode == CREG) {
        gcstack(-8);
        gc_flds3(x->r);
        gc_frndint();
        gc_fsts3(x->r);
        gc_cvtss2si(x->r, RH);
        x->r = RH;
        incR();
        gcstack(8);
        --RHF;
    }
}

void 
Ord(Item x) {
    int tf = x->type->form;
    if (tf == FREAL) {
        x->type = IntType;
    }
    load(x);

    if (tf == FREAL) {
        int unsigned v= 0xFFFFFFFF;
        gc_load_uinteger(v, RH);
        gc_and(RH, x->r);
    }
}

void
Shift(int fct, Item x, Item y) {
    int op;
    load(x);

    op = fct;

    if (y->mode == CCONST) {
        gc_shift(op, y->a % 32, x->r);
    } else {
        load(x);
    }
}

void
Write(int r[2]) {
    gc_align(r[0] + r[1]);
    gc_call("Write");
    gc_popq_align(r[0] + r[1]);
    restore_fpu_regs(r[1]);
    restore_regs(r[0]);
    RHF = 0;
    RH = 0;
}

void
call_write_int(int r[2]) {
    gc_align(r[0] + r[1]);
    gc_call("WriteInt");
    gc_popq_align(r[0] + r[1]);
    restore_fpu_regs(r[1]);
    restore_regs(r[0]);
    RHF = 0;
    RH = 0;
}

void
call_write_real(int r[2]) {
    gc_align(r[0] + r[1] * 2);
    gc_call("WriteReal");
    gc_popq_align(r[0] + r[1] * 2);
    restore_fpu_regs(r[1]);
    restore_regs(r[0]);
    RHF = 0;
    RH = 0;
}

void
call_write_bool(int r[2]) {
    gc_align(r[0] + r[1]);
    gc_call("WriteBool");
    gc_popq_align(r[0] + r[1]);
    restore_fpu_regs(r[1]);
    restore_regs(r[0]);
    RHF = 0;
    RH = 0;
}

void
call_write_ln() {
    gc_call("WriteLn");
}
