#include "compiler.h"
#include "mmemory.h"

#include <stdarg.h>
#include <stdlib.h>

#define KILOBYTE 1024
#define DEBUG


//prints a formatted string in the buffer. If it's full, the buffer gets flushed to output
//params : output : the output file to print flushed chars
//         reqc   : the number of chars requested
//         format : the same you would pass to printf
void fbufwf(FILE* output,int reqc, char* format, ...) {
    static char buffer[KILOBYTE]={0};
    static int index=0;
    if (index+reqc>=1022) { //1001 caratteri bufferizzati, reqc della string che viene e un null
        fputs(buffer,output);
        index=0;
        for (int i=0;i<1023;i++) { //zero all the buffer to be safe
            buffer[i]=0;
        }
    }
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer,reqc,format,args);
    va_end(args);
}

//flushes the buffer
#define fbufflush(output) fbufwf(output,2000,"")

void addPointer(const int value, FILE* output) {
    switch (value) { //Z
        case-1: {
            fbufwf(output,8,"dec rbx\n");
            return;
        }
        case 0:return;
        case 1: {
            fbufwf(output,8, "inc rbx\n");
            return;
        }
        default:break;
    } //Z - {1,0,-1}
    if (value>1) {
        fbufwf(output,14,"add rbx,%-5X\n",value);
        return;
    } //Z-[-1,INF)
    fbufwf(output,14,"sub rbx,%-5X\n",-value);
}
void movePointer(const int cpos,FILE* output) {
    fbufwf(output,13,"mov rbx,%-5X",cpos);
}

void changeValue(const int value,FILE* output) {
    switch (value) {
        case-1: {
            fbufwf(output,14,"dec [arp+rbx]\n");
            return;
        }
        case 0:return;
        case 1: {
            fbufwf(output,14, "inc [arp+rbx]\n");
            return;
        }
        default:break;
    }
    if (value>1) {
        fbufwf(output,19,"add [arp+rbx],%-5X",value);
        return;
    }
    fbufwf(output,19,"sub [arp+rbx],%-5X",-value);
}

int exitOpcode(FILE* output) {
    fbufwf(output,9,"call EXIT");
    fbufflush(output);
    return 0;
}


// returns the time che char c appears in the char[] buf
// When the function returns, buf points to the first char different from c
// param : c   : the char to check
//         buf : the char[] representing the string
int timesAppearConsec(const char c, const char* buf) {
    int res=0;
    while (*buf==c) {
        res++;
        buf++;
    }
    return res;
}

void setCycle(int lno,FILE* output) {
    fbufwf(output,8,"l%-5x:\n", //max 5-digit number,
        lno
    );
}

void closeCycle(const int lno,FILE* output) {
    fbufwf(
        output,
        28,
        "cmp [arp+rbx],0\n" //17
        "jnz l%-5x\n", //11
        lno
        );
}

RETURNS_OR_EXITS void transpileToAsm(const char* input, FILE *output, const int gridSize) {
    int cpos=0, linecount=0;
    int locl=0; //Last Opened Cycle Line
    int lnesting=0; //How many cycles are nested
    int lno=0; // The label
    while (input!=NULL && *input!=0) {

#define incrementAndExit() input++;break
        switch (*input) {
            case'\n': {
                linecount++;
                incrementAndExit();
            }
            case'[': {
                lno++;
                locl=linecount;
                lnesting++;
                setCycle(lno,output);
                incrementAndExit();
            }
            case']': {
                if (lnesting<=0) {
                    FERROR("A closed loop was not opened at line %i",linecount)
                }
                closeCycle(lno-1,output);
                lnesting--;
                incrementAndExit();
            }
            case'+': {
                const int vta = timesAppearConsec('+',input);
                changeValue(vta,output);
                //No need to incrementAndExit because input is already pointing to the next char
                break;
            }
            case'-':{
                const int vts=-timesAppearConsec('-',input); //- because it returns back
                changeValue(vts,output);
                //No need to incrementAndExit because input is already pointing to the next char
                break;
            }
            case'>':{
                const int dpos = timesAppearConsec('>',input);
                const int temp = cpos+dpos; // to avoid repeated sums
                if (temp > gridSize || temp < 0){
                    movePointer((cpos=temp%gridSize),output);
                } else {
                    addPointer(cpos+=dpos,output);
                }
                //No need to incrementAndExit because input is already pointing to the next char
                break;
                }
            case'<': {
                int dpos=timesAppearConsec('<',input);
                const int temp=cpos-dpos;
                if (temp>gridSize||temp<0){
                    movePointer(cpos=temp%gridSize,output);
                } else {
                    addPointer(cpos-=dpos,output);
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
                FERROR("A char escaped the preprocessing! %c",*input);
                #else
                break;
                #endif
            }
        }
    }
    if (lnesting>0) {
        FERROR("Missing closing cycle at line %i",locl);
    }
    exitOpcode(output);
    fbufflush(output);
}

NOT_NULL char* filterOutGarbage(FILE* input, const options* opts) {
    int c=0;
    int bufs=KILOBYTE;
    char* cp = malloc(bufs*sizeof(char));
    if (!cp) {
        FERROR("Failed to allocate memory for char* preprocess. errno:%i",errno)
    }
    char* cp0=cp; // save the start of the block of memory
    bool isComment=false;
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
                if (cp-cp0>=bufs) {
                    char* ap = realloc(cp0,(bufs+=256*sizeof(char)));
                    if (!ap) {
                        FERROR("Failed to allocate memory for char* preprocess. errno:%i",errno);
                    }
                    const ptrdiff_t offset = cp - cp0;
                    cp0 = ap;
                    cp=cp0+offset;
                }
                *cp=(char)c;
                ++cp;
                break;

            }
            default: {
                if (isComment && opts -> throw) {
                    FERROR("Unexpected char found: \'%c\'. To disable this error, set throw=false in file options.bfo",c)
                }
            }
        }
    }
    if (cp-cp0+ 1 >= bufs) {
        const ptrdiff_t offset = cp - cp0;
        char* ap2 = realloc(cp0, bufs += 1);
        if (!ap2) {
            FERROR("Failed to allocate memory for char* preprocess. errno:%i",errno)
        }
        cp0 = ap2;
        *(cp0+offset)=0;
    } else {
        *cp = 0;
    }
    return cp0;
}
//Check for errors
void createHeader(FILE* output, const options* opts) {
    FILE* header;
    fopen_s(
        &header,
        "bfheader.nasm.asm", //FIXME
        "r");
    int c;
    char buf[1024]={0}; //0..1023
    int index=0;
    while ((c=getc(header))!=EOF) {
        if (index>=1022) {
            fputs(buf,output);
            index=0;
        }
        buf[++index]=(char)c;
    }
    fputs(buf,output);
    fprintf(output,"section.resb\narp resb %i\nsection .text",opts->gridSize);

    fclose(header);
}

NOT_NULL FILE* getFile(char* name) {
    FILE* fp;
    fopen_s(&fp, name, "r");
    if (!fp) FERROR("Error opening file %s",name)
    return fp;
}

RETURNS_OR_EXITS void compileToAsm(FILE* input, const options* opts) {
    FILE* output;
    const errno_t err2 = fopen_s(&output,"bfproject.asm","w");
    if (err2!=0) {
        FERROR("Error creating output file. Error code: %i",err2);
    }
    const char* cp = filterOutGarbage(input,opts);
    //cp is guaranteed to be not null
    createHeader(output,opts);
    transpileToAsm(cp,output,opts->gridSize);
    //after here transpiling went good
    fclose(output);
    puts("Compiled file provided");
}