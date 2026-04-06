#include "compiler.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "option_parser.h"

#define KILOBYTE 1024
#define DEBUG

#define ARG_POS false
#define ARG_VAL true

#define GETINPUT
#define PRINT

void fbufwf(FILE* output,int reqc, char* format, ...) {
    static char buffer[KILOBYTE]={0};
    static int bufindex=0;
    if (bufindex+reqc>=sizeof(buffer)-2) { //1001 caratteri bufferizzati, reqc della string che viene e un null
        fputs(buffer,output);
        bufindex=0;
        for (int i=0;i<1023;i++) { //zero all the buffer
            buffer[bufindex]=0;
        }
    }
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer,reqc,format,args); //FIXME
    va_end(args);
}


void addPointer(int value,FILE* output) {
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
void movePointer(int cpos,FILE* output) {
    fbufwf(output,13,"mov rbx,%-5X",cpos);
}

void changeValue(int value,FILE* output) {
    switch (value) {
        case-1: {
            fbufwf(output,14,"dec [arp+rbx]\n");
            return;
        }
        case 0:return;
        case 1: {
            fbufwf(output,7, "inc [arp+rbx]\n");
            return;
        }
        default:break;
    }
    if (value>1) {
        fbufwf(output,13,"add rbx,%-5X",value);
        return;
    }
    fbufwf(output,13,"sub rbx,%-5X",-value);
}

int exitOpcode(FILE* output) {
    fputs("mov rax,60\nxor rdi,rdi\nsyscall\n\0",output); //FIXME
    return 0;
}
int timesAppearConsec(char c,char* buf) {
    int res=0;
    while (*buf==c) {
        res++;
        buf++;
    }
    return res;
}


void setCycle(int lno,FILE* output) {
    fbufwf(
        output,
        8,
        "l%-5x:\n", //max 5-digit number,
        lno
    );
}

void closeCycle(int cpos,int lno,FILE* output) {
    fbufwf(
        output,
        18+3*11,
        "cmp [arp+%-5i],0\n" // 18
        "jz le%-5x\n" //11
        "jmp l%-5x\n" //11
        "\nle%-5x:\n",//11
        cpos,
        lno,
        lno,
        lno
        );
}

int transpileToAsm(char* input, FILE *output, const int gridSize) {
    int cpos=0, linecount=0;
    int locl=0; //Last Opened Cycle Line
    int lnesting=0; //How many cycles are nested
    int lno=0; // The label
    while (input!=NULL&&*input!=0) {
        switch (*input) {
            case'\n': {
                linecount++;
                break;
            }
            case'[': {
                lno++;
                locl=linecount;
                lnesting++;
                setCycle(lno,output);
                break;
            }
            case']': {
                if (lnesting<=0) {
                    FERROR("A closed loop was not opened at line %i",linecount)
                }
                closeCycle(cpos,lno-1,output);
                lnesting--;
            }
            case'+': {
                const int vta=timesAppearConsec('+',input);
                changeValue(vta,output);
                break;
            }
            case'-':{
                const int vts=-timesAppearConsec('-',input); //- because it returns back
                changeValue(vts,output);
                break;
            }
            case'>':{
                int dpos=timesAppearConsec('>',input);
                const int temp=cpos+dpos;
                if (temp>gridSize||temp<0){
                    movePointer((cpos=temp%gridSize),output);
                } else {
                    addPointer(cpos+=dpos,output);
                }
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
                break;
            }
            default: {
                #ifdef DEBUG
                RERROR("A char escaped the preprocessing%c",'!',-1);
                #else
                break;
                #endif
            }
        }
        ++input;
    }
    if (lnesting>0) {
        RERROR("Missing closing cycle at line %i",locl,-1);
    }
    exitOpcode(output);
    return 0;
}

char* preprocess(FILE* input,options* opts) {
    int c=0;
    int bufs=KILOBYTE;
    char* cp = malloc(bufs*sizeof(char));
    if (!cp) {
        RERROR("Failed to allocate memory for char* preprocess. errno:%i",errno,NULL)
    }
    char* cp0=cp;
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
                        free(cp0);
                        RERROR("Failed to allocate memory for char* preprocess. errno:%i",errno,NULL);
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
                    free(cp0);
                    RERROR("Unexpected char found: \'%c\'. To disable this error, set throw=false in file options.bfo",c,NULL)
                }
            }
        }
    }
    if (cp-cp0+ 1 >= bufs) {
        const ptrdiff_t offset = cp - cp0;
        char* ap2 = realloc(cp0, bufs += 1);
        if (!ap2) {
            free(cp0);
            RERROR("Failed to allocate memory for char* preprocess. errno:%i",errno,NULL)
        }
        cp0 = ap2;
        *(cp0+offset)=0;
    } else {
        *cp = 0;
    }
    return cp0;
}

void createHeader(FILE* output, const options* opts) {
    FILE* header;
    fopen_s(
        &header,
        "bfheader.nasm.asm",
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

FILE* getFile(char* name) {
    FILE* fp;
    fopen_s(&fp, name, "r");
    if (!fp) RERROR("Error opening file %s",name,NULL)
    return fp;
}

int compileToAsm(FILE* input,options* opts) {
    FILE* output;
    const errno_t err2 = fopen_s(&output,"bfproject.asm","w");
    if (err2!=0) {
        FERROR("Error creating output file. Error code: %i",err2);
    }
    char* cp = preprocess(input,opts);
    if (cp==NULL) {
        free(opts);
        fclose(output);
        exit(-1);
    }

    createHeader(output,opts);
    if (transpileToAsm(cp,output,opts->gridSize)==-1) {
        free(opts);
        fclose(output);
        free(cp);
        exit(-1);
    }
    fclose(output);
    puts("Compiled file provided");
    free(cp);
    return 0;
}