
#ifndef BFC_OPTION_PARSER_H
#define BFC_OPTION_PARSER_H
#include <stdbool.h>
#include <stdint.h>


typedef struct {
    char* goRight,
    *goLeft,
    *increment,
    *decrement,
    *loopStart,
    *loopEnd,
    *printCur,
    *askInput;
} tbs; //Trivial Brainfuck Substitution

typedef struct{
    bool throw, logs;
    int32_t gridSize;
    tbs* sub;
} options;

int createOptions(char*,options*);

#endif //BFC_OPTION_PARSER_H