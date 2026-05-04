#ifndef BFC_MEMORY_H
#define BFC_MEMORY_H

#include <stdint.h>
#include "../include/dgalib/dgalib.h"

DGAHEADER(char,Ch,cdga)

DGAHEADER(int,I,idga)

#define MESSAGE(arg) {fprintf(stdout,arg);}
#define FERROR(arg,varargs) {fprintf(stderr,arg,varargs); freeptrs(); exit(-1) ;}
#define ERROR(arg) {fprintf(stderr,arg);freeptrs(); exit(-1);}
#define WARNING(arg) fprintf(stdout,"WARNING: %s",arg);

#define NOT_NULL
#define RETURNS_OR_EXITS




#endif //BFC_MEMORY_H
