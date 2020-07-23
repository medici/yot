#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "declare.h"
#include "define.h"
#include "generator.h"
#include "parser.h"
#include "printt.h"
#include "scan.h"
#include "sys.h"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

static void cmderror(char *s, char *a) {
  fprintf(stderr, "scc: ");
  fprintf(stderr, s, a);
  fputc('\n', stderr);
  exit(EXIT_FAILURE);
}

char *newfilename(char *file, int sfx) {
  char *ofile;

  if ((ofile = strdup(file)) == NULL) mark("too many file names", NULL);
  if (sfx != 0) {
    ofile[strlen(ofile) - 3] = sfx;
    ofile[strlen(ofile) - 2] = 0;
  } else {
    ofile[strlen(ofile) - 4] = 0;
  }

  return ofile;
}

void collect(char *file) { Files[Nf++] = file; }

static void concat(char *buf, char *s) {
  strcat(buf, " ");
  strcat(buf, s);
}

static void compile(char *file) {
  FILE *in, *out;
  char *ofile;

  if (file) {
    if ((in = fopen(file, "r")) == NULL) {
      mark("no such file: %s", file);
    }
    ofile = newfilename(file, 's');
    if ((out = fopen(ofile, "w+")) == NULL) {
      mark("cannot create file: %s", ofile);
    }
  }
  File = file;
  Infile = in;
  Outfile = out;

  gprelude();

  next();
  module();

  gpostlude();
  if (file) {
    fclose(in);
    if (out) fclose(out);
  }
}

static void assemble(char *file, int delete) {
  char cmd[TEXTLEN + 1];
  char *ofile;

  ofile = newfilename(file, 'o');
  file = newfilename(file, 's');

  collect(ofile);
  sprintf(cmd, ASCMD, ofile, file);

  system(cmd);
  if (delete) {
    remove(file);
  }
}

static void linker(char *file) {
  char cmd[TEXTLEN + 2];
  char cmd2[TEXTLEN + 2];
  cmd[0] = '\0';
  int i;
  for (i = 0; i < Nf; i++) {
    concat(cmd, Files[i]);
  }

  sprintf(cmd2, LDCMD, file, cmd);
  system(cmd2);
}

static void usage(void) {
  printf(
      "Usage: yot [-c] [-S] [-o]"
      " file \n");
}

static void longusage(void) {
  printf("\n");
  usage();
  printf(
      "\n"
      "-c       compile only, do not link\n"
      "-o file  write linker output to FILE\n"
      "-S       compile to assembly language\n"
      "\n");
}

int main(int argc, char *argv[]) {
  // realpath(argv[0],resolved_path);

  Nf = 0;
  char *filename;
  if (argc == 1 || *argv[1] != '-') {
    longusage();
  } else {
    filename = argv[2];
    char dir[32];
    copy_ident(dir, filename);
    copy_ident(CurrentDirectory, dirname(dir));
    strcat(CurrentDirectory, "/");
    if (!filename) {
      longusage();
      return 0;
    }
    init();
    compile(filename);
    switch (argv[1][1]) {
      case 'o':
        assemble(filename, 1);
        linker(newfilename(filename, 0));
        break;
      case 'S':
        break;
      case 'c':
        assemble(filename, 1);
        break;
      default:
        longusage();
        break;
    }
  }

  // float a;
  // double c;
  // int b,k,l=0,m[20],n,i,count=0,p;
  // printf("Enter any number:");
  // scanf("%lf",&c);
  // b=c;
  // a=c-b;
  // printf("%d,%f",b,a);
  // printf("\n");
  // while(b!=0){
  //     n=b%2;
  //     b/=2;
  //     m[i]=n;
  //     count+=1;
  //     i++;
  // }
  // for(k=count-1;k>=0;k--){
  //     printf("%d",m[k]);
  // }

  // printf(".");
  // for(p=1;p<=8;p++){
  //     a=a*2;
  //     l=a;
  //     printf("%d",l);
  //     if(l==1)
  //         a=a-l;
  // }
  // printf("\n");
  // next();
  // do {
  //     get();
  //     print_sym(Symbol);
  // } while(Ch != EOF);
}