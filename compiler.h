#ifndef BFC_COMPILER_H
#define BFC_COMPILER_H
#include <stdio.h>
#include <stdbool.h>

#include "option_parser.h"

#define COMMENT_CHAR '%'
FILE* createSketch(void);
FILE* createAsm(void);

int compile(FILE*,char*);
int transpileToAsm(char*,FILE*,int);
int compileToAsm(FILE*,options*);
char* preprocess(FILE*,options*);
int nasmCompile(char*);

#endif //BFC_COMPILER_H