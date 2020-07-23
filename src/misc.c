#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "data.h"
#include "declare.h"
#include "define.h"
#include "generator.h"
#include "parser.h"
#include "scan.h"

void *checked_malloc(int len) {
  void *p = malloc(len);
  if (!p) {
    fprintf(stderr, "\nRan out of memory!\n");
    exit(1);
  }
  return p;
}

void copy_ident(char *name, char *s) { strcpy(name, s); }

/*
 * https://www3.ntu.edu.sg/home/ehchua/programming/java/datarepresentation.html
 * http://www.anonhack.in/2016/08/program-convert-floating-decimal-binary/
 * https://rosettacode.org/wiki/Decimal_floating_point_number_to_binary#Go
 * https://www.geeksforgeeks.org/program-for-conversion-of-32-bits-single-precision-ieee-754-floating-point-representation/
 * https://www.exploringbinary.com/converting-floating-point-numbers-to-binary-strings-in-c/
 */
long int IEEE754float(Float var) {
  unsigned int binary[32];
  unsigned int index = 0;
  unsigned int j = 0;
  int k;
  long int ieee754 = 0;

  binary[index] = var.raw.sign;

  for (k = 7; k >= 0; k--) {
    index++;
    if ((var.raw.exponent >> k) & 1)
      binary[index] = 1;
    else
      binary[index] = 0;
  }
  for (k = 22; k >= 0; k--) {
    index++;
    if ((var.raw.mantissa >> k) & 1)
      binary[index] = 1;
    else
      binary[index] = 0;
  }

  for (k = 31; k >= 0; --k) {
    if (k == 31) {
      ieee754 += binary[k] * (2 >> 1);
    } else if (k == 30) {
      ieee754 += binary[k] * 2;
    } else {
      ieee754 += binary[k] * (2 << (30 - k));
    }
  }

  return ieee754;
}

void enter_keyword(int symbol, char *name) {
  Keytable[NumberOFKeywords].symbol = symbol;
  strcpy(Keytable[NumberOFKeywords].id, name);
  ++NumberOFKeywords;
}

void init() {
  AndLabel = 0;
  OrLabel = 0;
  Line = 1;
  Column = 0;
  NumberOFKeywords = 0;
  Ch = -1;
  Infile = stdin;
  enter_keyword(MODULE, "module");
  enter_keyword(MOD, "mod");
  enter_keyword(FLS, "false");
  enter_keyword(TRU, "true");
  enter_keyword(IMPORT, "import");
  enter_keyword(FUNC, "func");
  enter_keyword(VAR, "var");
  enter_keyword(TYPE, "type");
  enter_keyword(ARRAY, "array");
  enter_keyword(OF, "of");
  enter_keyword(CONST, "const");
  enter_keyword(THEN, "then");
  enter_keyword(END, "end");
  enter_keyword(DIV, "div");
  enter_keyword(IF, "if");
  enter_keyword(ELSIF, "elsif");
  enter_keyword(ELSE, "else");
  enter_keyword(OR, "or");
  enter_keyword(NOT, "not");
  enter_keyword(REPEAT, "repeat");
  enter_keyword(UNTIL, "until");
  enter_keyword(WHILE, "while");
  enter_keyword(POINTER, "pointer");
  enter_keyword(DO, "do");
  enter_keyword(TO, "to");
  enter_keyword(BY, "by");
  enter_keyword(FOR, "for");
  enter_keyword(NIL, "nil");
  enter_keyword(RECORD, "record");
  enter_keyword(RETURN, "return");
  base_init();
  parser_init();
}