#include "option_parser.h"
#include "include/inih/ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"

static int handlerForTbs(
    void* user,
    const char* _,
    const char* name,
    const char* value)
{
    tbs* tbs=user;
#define cmp(arg) strcmp(arg, name)==0
    if (cmp("greater")) {
        tbs->greaterThan = _strdup(value);
        return 1;
    }
    if (cmp("less")) {
        tbs->lessThan = _strdup(value);
        return 1;
    }
    if (cmp("loopend")) {
        tbs->loopEnd = _strdup(value);
        return 1;
    }
     if (cmp("loopstart")) {
        tbs->loopStart = _strdup(value);
         return 1;
    } if (cmp("dot")) {
        tbs->printCur = _strdup(value);
        return 1;
    }
    if (cmp("comma")) {
        tbs->askInput = _strdup(value);
        return 1;
    }
    if (cmp("openBracket")) {
        tbs->loopStart = _strdup(value);
        return 1;
    }
    if (cmp("closeBracket")) {
        tbs->loopEnd = _strdup(value);
        return 1;
    }
    return 0;
}

static int handler(
    void* user,
    const char* _,
    const char* name,
    const char* value
    )
{
    options* options = user;

    #define cmp(arg) strcmp(arg, name)==0
    if (cmp("gridSize")) {

        char* nv = value;
        while (*nv!=' '){++nv;}
        options->gridSize = strtol(value,&nv,10);

    } else if (cmp("throw") ) {
        options->throw = strcmp("false",value);

    } else if (cmp("logs")) {
        options->logs = strcmp(value,"false");
    } else if (cmp("subs")) {
        ini_parse(value,handlerForTbs,options->sub);
    }
    return 1;
#undef cmp
}

int init(char* name,options* opts) {
    const int res=ini_parse(name, handler, opts);
    switch (res) {
#if INI_USE_STACK == 0
        case -2: {
            ERROR("Failed memory allocation")
        }
#endif
        case -1: {
            FERROR("Can't open file %s",name)
        }
        case 0:{return 0;}
        default: {
            FERROR("A parsing error occurred in line %i",res);
        }
    }
}

int createProject(void) {
const char buffer[]="throw:true\n"
    "asm:false\n"
    "logs:true\n"
    "gridSize:256\n"
    "path:#path to the assembler\n"
    "includes:#.bfk files to include\0";
    FILE* sketch;
    fopen_s(&sketch,"options.bfo","w");
    if (!sketch) ERROR("Failed to create sketch")
    fputs(buffer,sketch);
    return 0;
}

