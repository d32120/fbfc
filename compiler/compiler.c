#include "compiler.h"
#include "mmemory.h"

#include <stdarg.h>
#include <stdlib.h>

#define KILOBYTE 1024
#define DEBUG

#define FBUFWF_FLUSH 1
#define FBUFWF_WRITE 2
#define FBUFWF_FREE 3

//prints a formatted string in the buffer. when it's called with state=FBUFWF_FLUSH, the buffer gets flushed to output
//params : output : the output file to print flushed chars
//         reqc   : the number of chars requested
//         format : the same you would pass to printf
// Shares a global state: DO NOT USE IN MULTITHREAD
// USE ONLY IN THIS FILE
// Once flushed the buffer get voided
void _fbufwf(int state,FILE* output, char* format, ...) {
#ifdef STARTFBWF
#error "DON'T USE _fbufwf DIRECTLY"
#endif
    static dga* buffer=NULL;
    if (!buffer) {
        buffer=createDGA(KILOBYTE);
    }
    switch (state) {
        case FBUFWF_FLUSH: {
            finalizeDGA(buffer);
            fputs(buffer->data,output);
            break;
        }
        case FBUFWF_WRITE: {
            va_list args;
            va_start(args, format);
            putFormatDGA(buffer,format,args);
            va_end(args);
            break;
        }
        case FBUFWF_FREE: {
            freeDGA(buffer);
            buffer=NULL;
            break;
        }
        default:break;
    }
}

#define fbufflush(output) do{ _fbufwf(FBUFWF_FLUSH,output,"");_fbufwf(FBUFWF_FREE,NULL,"");}while(0)
#define fbufwf(format,...) _fbufwf(FBUFWF_WRITE,NULL,format,##__VA_ARGS__)
#define STARTFBWF

void addPointer(const int value) {
    switch (value) { //Z
        case-1: {
            fbufwf("dec rbx\n");
            return;
        }
        case 0:return;
        case 1: {
            fbufwf("inc rbx\n");
            return;
        }
        default:break;
    } //Z - {1,0,-1}
    if (value>1) {
        fbufwf("add rbx,%-5X\n",value);
        return;
    } //Z-[-1,INF)
    fbufwf("sub rbx,%-5X\n",-value);
}

void movePointer(const int cpos) {
    fbufwf("mov rbx,%-5X",cpos);
}

void changeValue(const int value) {
    switch (value) {
        case-1: {
            fbufwf("dec [arp+rbx]\n");
            return;
        }
        case 0:return;
        case 1: {
            fbufwf("inc [arp+rbx]\n");
            return;
        }
        default:break;
    }
    if (value>1) {
        fbufwf("add [arp+rbx],%-5X",value);
        return;
    }
    fbufwf("sub [arp+rbx],%-5X",-value);
}

// returns the time che char c appears in the dga* buf (buf->data)
// When the function returns, buf points to the first char different from c
// param : c   : the char to check
//         buf : the char[] representing the string
int timesAppearConsec(const char c,dga* buf) {
    int res=0;
    while (getDGA(buf,0)==c) {
        res++;
        buf++;
        consumeFirstDGA(buf);
    }
    return res;
}

void setCycle(int lno) {
    fbufwf(
        "l%-5x:\n",
        lno
    );
}

void closeCycle(const int lno) {
    fbufwf(
        "cmp [arp+rbx],0\n" //17
        "jnz l%-5x\n", //11
        lno
        );
}

int transpileToAsm(_Inout_ dga* input,_In_ FILE* output,_In_ const int gridSize) {
    int cpos=0, linecount=0;
    int locl=0; //Last Opened Cycle Line
    int lnesting=0; //How many cycles are nested
    int lno=0; // The label
    char current;
    while ((current=consumeFirstDGA(input))>0) {
        switch (current) {
            case'\n': {
                linecount++;
                input++;
                break;
            }
            case'[': {
                lno++;
                locl=linecount;
                lnesting++;
                setCycle(lno);
                input++;
                break;
            }
            case']': {
                if (lnesting<=0) {
                    fprintf(stderr, "A closed loop was not opened at line %i", linecount);
                    return -1;
                }
                closeCycle(lno-1);
                lnesting--;
                input++;
                break;
            }
            case'+': {
                const int vta = timesAppearConsec('+',input);
                changeValue(vta);
                //No need to incrementAndExit because input is already pointing to the next char
                break;
            }
            case'-': {
                const int vts =- timesAppearConsec('-',input); //- because it returns back
                changeValue(vts);
                //No need to incrementAndExit because input is already pointing to the next char
                break;
            }
            case'>': {
                const int dpos = timesAppearConsec('>',input);
                const int temp = cpos+dpos; // to avoid repeated sums
                if (temp > gridSize || temp < 0){
                    movePointer(cpos=temp%gridSize);
                } else {
                    cpos=temp;
                    addPointer(dpos);
                }
                //No need to incrementAndExit because input is already pointing to the next char
                break;
                }
            case'<': {
                int dpos = timesAppearConsec('<',input);
                const int temp=cpos-dpos;
                if (temp>gridSize||temp<0){
                    movePointer(cpos=temp%gridSize);
                } else {
                    cpos=temp;
                    addPointer(-dpos);
                }
                //No need to incrementAndExit because input is already pointing to the next char
                break;
            }
            case'.': {
                fbufwf("call  "); //FIXME
            }
            case',': {
                fbufwf("call "); //FIXME TOO
            }
            default: {
                #ifdef DEBUG
                {
                    fprintf(stderr, "A char escaped the preprocessing! %c", current);
                    freeptrs();
                    freeDGA(input);
                    exit(-1);
                };
                #else
                break;
                #endif
            }
        }
    }
    if (lnesting>0) {
            fprintf(stderr, "Missing closing cycle at line %i", locl);
            return -1;
    }
    fbufwf("call EXIT");
    fbufflush(output);
    return 0;
}

NOT_NULL dga* filterOutGarbage(_In_ FILE* input, _In_ const options* opts) {
    dga* array = createDGA(256);
    bool isComment=false;
    char c;
    while ((c=getc(input))!=EOF){
        switch (c){
            case COMMENT_CHAR:isComment=true;break;
            case'\n':isComment=false;break;
            case'+':
            case'-':
            case'<':
            case'>':
            case'[':
            case']':
            case'.':
            case',': {
                if (pushDGA(array,c)<=0) {
                    freeptrs();
                    freeDGA(array);
                    fprintf(stderr,"Terminating the execution...");
                    exit(-1);

                }
                break;
            }
            default: {
                if (isComment && opts -> throw) {
                    {
                        fprintf(
                            stderr,
                            "Unexpected char found: \'%c\'. To disable this error, set throw=false in options file",
                            c);
                        freeptrs();
                        exit(-1);
                    }
                }
            }
        }
    }
    finalizeDGA(array);
    return array;
}

NOT_NULL FILE* getFile(char* name) {
    FILE* fp;
    fopen_s(&fp, name, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file %s", name);
        freeptrs();
        exit(-1);
    }
    return fp;
}

RETURNS_OR_EXITS void compileToAsm(FILE* input, const options* opts) {
    FILE* output;
    const errno_t err2 = fopen_s(&output,"bfproject.asm","w");
    if (err2!=0) {
        {
            fprintf(stderr, "Error creating output file. Error code: %i", err2);
            freeptrs();
            exit(-1);
        };
    }
    const dga* cp = filterOutGarbage(input,opts);
    //cp is guaranteed to be not null
   // createHeader(output,opts);
    transpileToAsm(cp,output,opts->gridSize);
    //after here transpiling went good
    fclose(output);
    puts("Compiled file provided");
}