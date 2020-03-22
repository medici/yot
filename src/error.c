#include <string.h>
#include <stdlib.h>
#include "define.h"
#include "data.h"
#include "declare.h"

void mark(char *s, char *a) {
	fprintf(stderr, "error: %s: %d: %ld: ", File, Line, Column - strlen(Text));
	fprintf(stderr, s, a);
	fprintf(stderr, "\n");
}

void fatal(char *s) {
	mark(s, NULL);
	mark("fatal error, stop", NULL);
	exit(EXIT_FAILURE);
}