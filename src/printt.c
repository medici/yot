#include<stdio.h>
#include<stdlib.h>
#include "define.h"
#include "data.h"
#include "scan.h"
#include "printt.h"

void
print_sym(int sym) {
    switch(sym) {
        case NIL:
            printf("NIL: nil\n");
            break;
        case TIMES:
            printf("TIMES: *\n");
            break;
        case DIV:
            printf("DIV: /\n");
            break; 
        case MOD:
            printf("MOD: mod\n");
            break;
        case AND:
            printf("AND: &\n");
            break;
        case PLUS:
            printf("PLUS: +\n");
            break;
        case MINUS:
            printf("MINUS: -\n");
            break; 
        case OR:
            printf("OR: or\n");
        case NEQ:
            printf("NEQ: #\n");
            break;
        case EQL:
            printf("EQL: =\n");
            break;
        case LSS:
            printf("LSS: <\n");
            break;
        case GEQ:
            printf("GEQ: >=\n");
            break;
        case LEQ:
            printf("LEQ: <=\n");
            break;
        case GTR:
            printf("GTR: >\n");
            break;
        case NOT: 
            printf("NOT: ~\n");
            break;
        case BECOMES:
            printf("BECOMES: :=\n");
            break;
        case PERIOD:
            printf("PERIOD: .\n");
            break;
        case COMMA:
            printf("COMMA: ,\n");
            break;
        case COLON:
            printf("COLON: :\n");
            break;
        case LPAREN:
            printf("LPAREN: (\n");
            break;
        case LBRAK:
            printf("LBRAK:  [\n");
            break;
        case RPAREN:
            printf("RPAREN:  )\n");
            break;
        case RBRAK:
            printf("RPAREN:  ]\n");
            break;
        case SEMICOLON:
            printf("SEMICOLON:  ;\n");
            break;
        case SEPARATOR:
            printf("SEPARATOR: |\n");
            break;
        case IDENT:
            printf("IDENT:  %s\n", Text);
            break;
        case STRING:
            printf("STRING %s\n",  Text);
            break;
        case OF:
            printf("of\n");
            break;
        case THEN:
            printf("then\n");
            break;
        case DO:
            printf("do\n");
            break;
        case END:
            printf("end\n");
            break;
        case ELSE:
            printf("else\n");
            break;
        case ELSIF:
            printf("elseif\n");
            break;
        case IF:
            printf("if\n");
            break;
        case WHILE:
            printf("while\n");
            break;
        case ARRAY:
            printf("array\n");
            break;
        case RECORD:
            printf("record\n");
            break;
        case CONST:
            printf("const\n");
            break;
        case TYPE:
            printf("type\n");
            break;
        case VAR:
            printf("var\n");
            break;
        case FUNC:
            printf("func\n");
            break;
        case REPEAT:
            printf("repeat\n");
            break;
        case UNTIL:
            printf("until\n");
            break;
        case MODULE:
            printf("module\n");
            break;
    }
}