#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>

DGAIMPL(token,,TokenList,createToken(INDEX_OUT_OF_BOUNDS_OPCODE,0),createToken(EOF_TOKEN_OPCODE,0));

inline int isOutOfBoundsToken(const token* token){ return token->opcode == INDEX_OUT_OF_BOUNDS_OPCODE; }
inline int isLastTerm(const token* token){ return token->opcode == EOF_TOKEN_OPCODE; }
inline int isNullToken(const token* token){ return token->opcode == NULL_TOKEN_OPCODE; }

token createToken(uint8_t opcode, uint16_t param,token defaultToken) {
    token* tk= malloc(sizeof(token));
    if (!tk) return defaultToken;
    tk->opcode = opcode;
    tk->params = param;
    return *tk;
}

