/*
 *	NMH's Simple C Compiler, 2013,2014
 *	Linux/x86-64 environment
 */

#define OS		"Linux"

#define ASCMD       "/usr/bin/as  -o %s %s"
#define LDCMD       "/usr/bin/ld  -o %s %s lib/libs.o lib/libyot.a"