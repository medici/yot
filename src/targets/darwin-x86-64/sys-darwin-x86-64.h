/*
 *	NMH's Simple C Compiler, 2013,2014
 *	Darwin/x86-64 environment
 */

#define OS		"Darwin"

#define ASCMD       "/usr/bin/as -arch x86_64 -mmacosx-version-min=10.10 -g -o %s %s"
#define LDCMD       "/usr/bin/ld -lc -arch x86_64 -macosx_version_min 10.10 -o %s %s lib/libs.o lib/libyot.a"