#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>

#include "base.h"
#include "define.h"
#include "scan.h"

/* tokens */
enum {
  NIL,

  TIMES,
  DIV,
  RDIV,
  MOD,
  AND,
  PLUS,
  MINUS,
  OR,

  EQL,
  NEQ,
  LSS,
  GEQ,
  LEQ,
  GTR,
  NOT,
  BECOMES,

  PERIOD,
  COMMA,
  COLON,
  LPAREN,
  LBRAK,
  RPAREN,
  RBRAK,

  CHAR,
  INT,
  INTEGER,
  REAL,
  STRING,
  FLS,  // FALSE
  TRU,  // TRUE

  IDENT,
  IF,
  WHILE,
  REPEAT,
  CASE,
  FOR,

  OF,
  THEN,
  DO,
  TO,
  BY,
  SEMICOLON,
  END,
  SEPARATOR,
  ELSE,
  ELSIF,
  UNTIL,
  RETURN,
  ARRAY,
  RECORD,
  POINTER,
  CONST,
  TYPE,
  VAR,
  FUNC,
  MODULE,
  IMPORT
};

int Line;
int Column;
int Ch;                  // current char
char Text[TEXTLEN + 1];  // current ident
long int Value;
double RValue;
char *File;
int Symbol;

int NumberOFKeywords;
Keyword Keytable[KEYWORD];

#endif