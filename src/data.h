#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include "define.h"
#include "base.h"

// scan


// generator

FILE *Infile, *Outfile;
int	Textseg;

#define MAXFILES    32
char    *Files[MAXFILES];
char    CurrentDirectory[TEXTLEN];
int Nf;
int AndLabel;
int OrLabel;


#endif