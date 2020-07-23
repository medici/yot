

#define BITS 64
#define OS "Darwin"

#define ASCMD "/usr/bin/as -arch x86_64 -mmacosx-version-min=10.10 -g -o %s %s"
#define LDCMD                                                        \
  "/usr/bin/ld -lc -arch x86_64 -macosx_version_min 10.10 -o %s %s " \
  "lib/libs.o lib/libyot.a"

#define WORDSIZE 8
#define INTSIZE WORDSIZE

// https://stackoverflow.com/questions/1505582/determining-32-vs-64-bit-in-c
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif