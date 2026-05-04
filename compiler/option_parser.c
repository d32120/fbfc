#include "option_parser.h"
#include "../include/inih/ini.h"
#include "mmemory.h"

#include <stdlib.h>
#include <string.h>

static int handlerForTbs(
    void *user,
    const char *_,
    const char *name,
    const char *value) {
    tbs *tbs = user;
#define check() if(value==NULL) return 0

    if (strcmp("greater", name) == 0) {
        check();
        tbs->goRight = _strdup(value);
        return 1;
    }
    if (strcmp("less", name) == 0) {
        check();
        tbs->goLeft = _strdup(value);
        return 1;
    }
    if (strcmp("loopend", name) == 0) {
        check();
        tbs->loopEnd = _strdup(value);
        return 1;
    }
    if (strcmp("loopstart", name) == 0) {
        check();
        tbs->loopStart = _strdup(value);
        return 1;
    }
    if (strcmp("dot", name) == 0) {
        check();
        tbs->printCur = _strdup(value);
        return 1;
    }
    if (strcmp("comma", name) == 0) {
        check();
        tbs->askInput = _strdup(value);
        return 1;
    }
    if (strcmp("inc", name) == 0) {
        check();
        tbs-> increment = _strdup(value);
        return 1;
    }
    if (strcmp("dec", name) == 0) {
        check();
        tbs-> decrement = _strdup(value);
        return 1;
    }

    return 0;
}

static int handler(
    void *user,
    const char *_,
    const char *name,
    const char *value
) {
    options *options = user;

#define cmp(arg) strcmp(arg, name)==0
    if (strcmp("gridSize", name) == 0) {
        char *nv = (char *) value;
        while (*nv != ' ') { ++nv; }
        options->gridSize = (int32_t) strtol(value, &nv, 10);
        return 1;
    }
    if (strcmp("throw", name) == 0) {
        options->throw = strcmp("false", value);
        return 1;
    }
    if (strcmp("logs", name) == 0) {
        options->logs = strcmp(value, "false");
        return 1;
    }
    if (strcmp("subs", name) == 0) {
        if (value==NULL) {
            options->sub=NULL;
            return 1;
        }
        ini_parse(value, handlerForTbs, options->sub);
        return 1;
    }
    return 0;
#undef cmp
}

int createOptions(_In_ char *name,_Out_ options *opts) {
    const int res = ini_parse(name, handler, opts);
    switch (res) {
#if INI_USE_STACK == 0
        case -2: {
            ERROR("Failed memory allocation")
        }
#endif
        case -1: {
            {
                fprintf(stderr, "Can't open file %s", name);
                freeptrs();
                exit(-1);
            }
        }
        case 0: { return 0; }
        default: {
            {
                fprintf(stderr, "A parsing error occurred in line %i", res);
                freeptrs();
                exit(-1);
            };
        }
    }
}
