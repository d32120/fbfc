#include "mmemory.h"

#include <stdio.h>

#define DEBUG
#undef malloc
#undef realloc
#undef free

void freeptrs() {
    int i=0;
    while (i<MEMORY_SIZE-1) {
        if (ptrs[i]!=NULL) {
            free(ptrs[i]);
        }
        i++;
    }
}

void* mmalloc(const size_t size) {
    void* nptr=malloc(size);
    if (nptr==NULL) {
        return NULL;
    }
    int new=0;
    while (ptrs[new]!=NULL) { new++; }
    if (new-1>MEMORY_SIZE) { exit(1);}
    ptrs[new]=nptr;
    return nptr;
}

void* mrealloc(void* ptr,const size_t size) {
    void* nptr=realloc(ptr,size);
    if (nptr==NULL) {
        return NULL;
    }
    int new=0;
    while (ptrs[new]!=ptr) { new++; }
#ifdef DEBUG
    if (new-1>MEMORY_SIZE) { ERROR("In function mrelloc the pointer hasn't been found. Check the usage of malloc instead of mmalloc");}
#endif
    ptrs[new]=nptr;
    return nptr;
}

#define malloc(a) mmalloc(a)
#define realloc(a,b) mrealloc(a,b)
#define free(a) DO_NOT_FREE