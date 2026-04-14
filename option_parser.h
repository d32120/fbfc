
#ifndef BFC_OPTION_PARSER_H
#define BFC_OPTION_PARSER_H
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    char* greaterThan,
    *lessThan,
    *pointerLeft,
    *pointerRight,
    *loopStart,
    *loopEnd,
    *printCur,
    *askInput;
} tbs; //Trivial Brainfuck Substitution

typedef struct{
    bool throw, logs;
    int gridSize;
    tbs* sub;
} options;

int init(char*,options*);

int createProject(void);

#define MESSAGE(arg) {fprintf(stdout,arg);}
#define FERROR(arg,varargs) {fprintf(stderr,arg,varargs); exit(-1) ;}
#define ERROR(arg) {fprintf(stderr,arg); exit(-1);}
#define RERROR(arg,varargs,returnv) {fprintf(stderr,arg,varargs); return returnv;}
#define WARNING(arg) fprintf(stdout,"WARNING: %s",arg);

#endif //BFC_OPTION_PARSER_H