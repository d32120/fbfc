#include <stdlib.h>

#include "compiler.h"
#include "c/mmemory.h"
#include "c/option_parser.h"
#include "include/cli/cli.h"

int main(int argc, char **argv) {
    bool outAsm = false;
    char* infile = NULL;
    char* optsn=NULL;
    int throw=-1;
    clioptions("my program", argc, argv) {
        cliopt("-h\tThis help") {
            cliusage(CLIEXIT);
        }
        cliopt("-S, --asm\tBe verbose") {
            outAsm=true;
        }

        cliopt("i\tThe file to compile") {     // required positional
            infile = cliarg;
        }

        cliopt("op (options.bfo)\tThe options' file name (default:options.bfo)") { // optional positional
            optsn = cliarg;
        }

        cliopt("-t, --throw\tThrow an error if found an unexpected char (overrides options file)") {
            throw=1;
        }
        cliopt("-nt, --nothrow\tDon't throw an error if found an unexpected char (overrides options file)") {
            throw = 0;
        }

        cliopt() {    // mandatory catch-all, last
            if (cliarg[0] == '!') {cliexit();}      // stop scanning
        }
    }
    options opts;
    init(optsn,&opts);
    switch (throw) {
        case 1: opts.throw=true;break;
        case 0 :opts.throw=false;break;
        default:break;
    }
    FILE* inf;
    fopen_s(&inf,infile,"r");
    if (inf==NULL) {
        ERROR("Could not open input file")
    }
    compileToAsm(inf,&opts);
   if (outAsm) {
       return 0;
   }
    nasmCompile("bfproject.asm");
    puts("Compiled project into an object file. Now pass it into the linker.");
    return 0;
}
