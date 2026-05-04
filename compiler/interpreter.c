#include "interpreter.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "mmemory.h"

INTDGAIMPL(uint8_t,,tape);

//returns -1 on failure, -2 on memory error and 0 if all went good. If returns 0 then result contains the pointer to the precalculated tape
//Usage: args == NULL to precalc the condition, args != NULL to run the program

int interpret(
    _In_ const cdga *input,
    _In_opt_ const cdga * args,
    _Out_opt_ tape *tape
    ){
    cdga *stack = createChcdga(32);
    if (!stack) return DGA_BAD_MEM_ALLOC;
    int argsAddress=0;
    int ip = 0;
    int curpos = 0;

    while (ip < input->size) {
        switch (getChcdga(input, ip)) {
            case '+': {
                const uint8_t temp = gettape(tape,curpos);
                if (temp == -1) {           //try increment and check for error
                    freeChcdga(stack);
                    return -2;
                }
                settape(tape,curpos,temp+1);
                ip++;
                break;
            }
            case '-': {
                const uint8_t temp = gettape(tape,curpos);
                if (temp == -1) {           //try decrement and check for error
                    freeChcdga(stack);
                    return -2;
                }
                settape(tape,curpos,temp-1);
                ip++;
                break;
            }
            case '>': curpos++;
                ip++;
                break;
            case '<': curpos--;
                ip++;
                break;
            case '.': putchar(gettape(tape,curpos));
                ip++;
                break;
            case ',': {
                if (args != NULL) {
                    char t;
                    if ((t = getChcdga(args,argsAddress++))) {
                        settape(tape,curpos,t);
                        ip++;
                        break;
                    }
                    puts("The script required more input than the provided");
                    return -4;
                }
                return 0;
            }
            case '[': {
                if (gettape(tape,curpos) == 0) {
                    char temp=0;                                                          // if current cell is 0,
                    do {
                        temp = getChcdga(input, ++ip);
                    } while (temp!=']' && temp!=DGA_INDEX_OUT_OF_BOUNDS);                  // goto the matching ]
                } else {                                                                  // else
                    if (pushChcdga(stack, ip++) == -1) {                                  // push the current instruction pointer and continue
                        freeChcdga(stack);                                                // if something is wrong,
                        return -2;                                                        // abort
                    }
                }
                break;
            }
            case ']': {
                if (gettape(tape,curpos) != 0) {                   // if the current cell is not 0.
                    ip = popChcdga(stack) + 1;                    // jump back to the beginning of the loop
                } else {                                          // else
                    if (popChcdga(stack) == -1) {                 // try to remove the loop from the stack. if error,
                        freeChcdga(stack);                        // abort
                        return -1;
                    }
                    ip++;
                };
                break;
            }
            default: ip++;
                break;
        }
    }
    return 0;
}
/*

int interpretCheckForChange(char* input) { //FINISH ME
    tape mtape;
    dga* arg1=createDGA(strlen(input));
    if (!arg1) {
        return DGA_BAD_MEM_ALLOC;
    }
    interpret(arg1,NULL,&mtape);
    if (mtape.arrow[mtape.size-1] == 0) {}
}*/