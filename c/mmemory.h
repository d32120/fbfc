//
// Created by Francesco on 14/04/2026.
//

#ifndef BFC_MEMORY_H
#define BFC_MEMORY_H

#include <stdlib.h>
#define malloc(a) mmalloc(a)
#define realloc(a,b) mrealloc(a,b)
#define free(a) DO_NOT_FREE
#define MEMORY_SIZE 25 //Don't knowwww

#define MESSAGE(arg) {fprintf(stdout,arg);}
#define FERROR(arg,varargs) {fprintf(stderr,arg,varargs); freeptrs(); exit(-1) ;}
#define ERROR(arg) {fprintf(stderr,arg);freeptrs(); exit(-1);}
#define WARNING(arg) fprintf(stdout,"WARNING: %s",arg);

#define NOT_NULL
#define RETURNS_OR_EXITS

static void* ptrs[MEMORY_SIZE]={NULL};

void freeptrs(void);

static void* mrealloc(void*,size_t);

static void* mmalloc(size_t);



#endif //BFC_MEMORY_H
