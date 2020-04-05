#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "data.h"
#include "declare.h"
#include "define.h"

//http://abcdxyzk.github.io/blog/2012/11/23/assembly-args/
// https://inf.ethz.ch/personal/wirth/ProjectOberon/Sources/Texts.Mod.txt
// https://www.felixcloutier.com/x86/
// https://cs.lmu.edu/~ray/notes/gasexamples/
// http://www.c-jump.com/CIS77/ASM/DataTypes/T77_0200_exchanging_integers.htm
// https://docs.oracle.com/cd/E19455-01/806-3773/instructionset-49/index.html

// rdi rsi rdx rcx r8 r9 
// rax r10 r11 rbx r12 r13 r14 r15

int
is_whitespace(int ch) {
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        return TRUE;
    }
    return FALSE;
}

void
next() {    
    if ('\n' == Ch) {
         Line++;
         Column = 0;
     } else {
         Column++;
     }
     Ch = fgetc(Infile);
}

int
scan_identifier() {
    int index = 0;
    int k = 0;
    do {
        if(index < TEXTLEN) {
            Text[index++] = Ch;
        }
        next();
      
    } while(isalpha(Ch) || isdigit(Ch) || Ch == '_');
    
    Text[index] = 0;
    while(k < NumberOFKeywords && strcmp(Text, Keytable[k].id) != 0) {
        ++k;
    }
    
    if (k < NumberOFKeywords) {
        return Keytable[k].symbol;
    } else {
        return IDENT;
    }
}

float
ten(int e) {
    float x = 1.0, t = 10.0;
    while(e > 0) {
        if (e % 2 == 1) {
            x = t * x;
        }
        t = t * t;
        e = e / 2;
    }
    return x;
}


/*
 * https://docs.microsoft.com/en-us/cpp/c-language/type-float?view=vs-2019
 * https://www.iitg.ac.in/asahu/cs101-2017/Lec07.pdf
 * https://www.h-schmidt.net/FloatConverter/IEEE754.html
 */
void
scan_number() {
    Value = 0;
    RValue = 0.0;
    int d[16], n = 0, i = 0, k = 0, h = 0;
    float x = 0.0;
    int e = 0, s = 0, ns = 0;
    bool negE, marked = FALSE;

    do {
        Text[ns] = Ch;
        ++ns;
        if (n < 16) {
            d[n] = Ch - '0';
            ++n;
        }
        next();
    } while(isdigit(Ch) || (Ch >= 'a' && Ch <= 'f'));

    if (n >= 16) {
        marked = TRUE;
        Text[ns] = '\0';
        mark("%s number too large", Text);
    }
    Text[ns] = Ch;
    ++ns;

    if (Ch == 'h' || Ch == 'x') {
        do {
            h = d[i];
            if (h >= 10) {
                h = h - 39;
            }
            k = k * 0x10 + h;
            ++i;
        } while(i < n);

        if (Ch == 'x') {
            Symbol = CHAR;
            if (k < 0x100) {
                Value = k;
            } else {
                mark("%s illegal value", Text);
                Value = 0;
            }
        } else {
            Symbol = INTEGER;
            Value = k;
        }

        next();
    } else if (Ch == '.') {
        next();
        do {
            x = x * 10.0 + d[i];
            ++i;
        } while(i < n);

        while(isdigit(Ch)) {
            Text[ns] = Ch;
            ++ns;
            x = x * 10.0 + (Ch - '0');
            --e;
            next();
        }

        Text[ns] = Ch;
        ++ns;
        if (Ch == 'e') {
            next();
            if (Ch == '-') {
                negE = TRUE;
                next();
            } else {
                negE = FALSE;
                if (Ch == '+') {
                    next();
                }
            }
           
            if (isdigit(Ch)) {
                do {
                    Text[ns] = Ch;
                    ++ns;
                    s = s * 10 + (Ch - '0');
                    next();
                } while(isdigit(Ch));
                if (negE) {
                    e = e - s;
                } else {
                    e = e + s;
                }
            } else {
                Text[ns] = Ch;
                ++ns;
                Text[ns] = '\0';
                mark("%s digit?", Text);
            }
        }
        
        if (e < 0) {
            if (e >= -MAXEXP) {
                x = x / ten(-e);
            } else {
                x = 0;
            }
        } else if (e > 0) {
            if (e <= MAXEXP) {
                x = ten(e) * x;
            } else {
                x = 0.0;
                Text[ns] = '\0';
                mark("%s too large", Text);
            }
        }
        Symbol = REAL;
        RValue = x;
    } else {
        long int maxint = MAXINT;
        do {
            if (d[i] < 10) {
                if (Symbol == MINUS && k <= (maxint + 1 - d[i]) / 10) {
                    k = k * 10 + d[i];
                } else if (k <= (MAXINT - d[i]) / 10) {
                    k = k * 10 + d[i];
                } else {
                    if (!marked) {
                        mark("%s too large", Text); 
                    }
                    k = 0;
                }
            } else {
                mark("%s bad integer", Text);
            }
            ++i;
        } while( i < n);
        Symbol = INTEGER;
        Value = k;
    }
}

void
scan_comment() {
    next();
    while(1) {
        while(1) {
            while(Ch =='(') {
                next();
                if (Ch == '*') {
                    scan_comment();
                }
            }
            
            if (Ch == '*') {
                next();
                break;
            }
            if (Ch == EOF) {
                break;
            }
            next();
        }
        if (Ch == ')') {
            next();
            break;
        }
        if (Ch == EOF) {
            mark("comment not terminated", NULL);
            break;
        }
    }
}

void
scan_string(char *buf) {
    int i;
    
    for (i=0; i < TEXTLEN - 1; i++) {
        next();
        if (Ch == '"') {
            buf[i] = '\0';
            ++i;
            Value = i;
            return;
        }
        buf[i] = Ch;
    }
    fatal("string literal too long");
    Value = i;
}    

void
get(void) {
    while(Ch <= ' ' && Ch != EOF) {
        next();
    }
    
    if (Ch == EOF) {
        Symbol = EOF;
    }
    
    switch(Ch) {
        case '&':
            next();
            Symbol = AND;
            break;
        case '|':
            next();
            Symbol = SEPARATOR;
            break;
        case '*':
            next();
            Symbol = TIMES;
            break;
        case '/':
            next();
            Symbol = RDIV;
            break;
        case '+':
            next();
            Symbol = PLUS;
            break;
        case '-':
            next(); 
            Symbol = MINUS;
            break;
        case '=':
            next();
            Symbol = EQL;
            break;
        case '#':
            next();
            Symbol = NEQ;
            break;
        case '<':
            next();
            if (Ch == '=') {
                next();
                Symbol = LEQ;
            } else {
                Symbol = LSS;
            }
            break;
        case '>':
            next();
            if (Ch == '=') {
                next();
                Symbol = GEQ;
            } else {
                Symbol = GTR;
            }
            break;
        case ';':
            next();
            Symbol = SEMICOLON;
            break;
        case ',':
            next();
            Symbol = COMMA;
            break;
        case ':':
            next();
            if (Ch == '=') {
                next();
                Symbol = BECOMES;
            } else {
                Symbol = COLON;
            }
            break;
        case '.':
            next();
            Symbol = PERIOD;
            break;
        case '(':
            next();
            if (Ch == '*') {
                scan_comment();
                get();
            } else {
                Symbol = LPAREN;
            }
            break;
        case ')':
            next();
            Symbol = RPAREN;
            break;
        case '[':
            next();
            Symbol = LBRAK;
            break;
        case ']':
            next();
            Symbol = RBRAK;
            break;
        case '"':
            scan_string(Text);
            next();
            Symbol = STRING;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            scan_number();
            break;
        case '~':
            next();
            Symbol = NOT;
            break;
        default:
            if (isalpha(Ch)) {
                Symbol = scan_identifier();
            } else {
                next();
                Symbol = NIL;
            }
    }
}