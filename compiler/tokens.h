#ifndef FUZZY_KTS_TOKENS_H
#define FUZZY_KTS_TOKENS_H
#include <stdint.h>
#include "../include/dgalib/dgalib.h"

typedef struct {
    uint8_t opcode;
    uint16_t params;
} token;

#define INDEX_OUT_OF_BOUNDS_OPCODE (uint8_t)(-2)
#define EOF_TOKEN_OPCODE (uint8_t)(-3)
#define NULL_TOKEN_OPCODE (0)

int isOutOfBoundsToken(const token* token);
int isLastTerm(const token* token);

DGAHEADER(token,,TokenList);

token createToken(uint8_t opcode, uint16_t params,token defaultToken);
token createToken(uint8_t opcode, uint16_t params,token defaultToken);


#endif //FUZZY_KTS_TOKENS_H
