
#ifndef BFC_OPTION_PARSER_H
#define BFC_OPTION_PARSER_H
#include <stdbool.h>


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

#endif //BFC_OPTION_PARSER_H