#pragma once

#include <stdint.h>
#include "mmemory.h"

DGAHEADER(uint8_t,,tape)

int interpret(const cdga*,const cdga*, tape*);

int interpretCheckForChange(char*);

#define precalc(input,output) interpret(input,NULL,output)
