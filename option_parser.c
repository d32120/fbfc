#include "option_parser.h"
#include "include/inih/ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"

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
    } else if (cmp("asmoutput")) {
        options->asmoutput= strcmp("false",value);
    } else if (cmp("path")) {
        options->assemblerPath = _strdup(value);
    } else if (cmp("includes")) {
        char* context;
        char* string=_strdup(value);
        const char* token = strtok_s(string, ",", &context);
        int i=0;
        while (token != NULL) {
            options->includes[i] = _strdup(token);
            i++;
            if (i==15) ERROR("Too many imports")
            token = strtok_s(NULL, "&", &context);
        }
    }
    return 1;
#undef cmp
}

int init(char* name,options* opts) {
    const int res=ini_parse(name, handler, opts);
    switch (res) {
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

