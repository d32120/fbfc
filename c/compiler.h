#ifndef BFC_COMPILER_H
#define BFC_COMPILER_H
#include <stdio.h>
#include "mmemory.h"

#include "option_parser.h"

#define COMMENT_CHAR '%'
FILE* createSketch(void);
FILE* createAsm(void);


int compile(FILE*,char*);

RETURNS_OR_EXITS void transpileToAsm(const char*,FILE*,int);
RETURNS_OR_EXITS void compileToAsm(FILE*, const options*);
NOT_NULL char* preprocess(FILE*, const options*);
int nasmCompile(char*);

#endif //BFC_COMPILER_H