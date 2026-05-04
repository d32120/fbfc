#ifndef FUZZY_KTS_DGALIB_H
#define FUZZY_KTS_DGALIB_H


#define DGA_INC 256
#define DGA_BAD_MEM_ALLOC (-1)
#define DGA_INDEX_OUT_OF_BOUNDS (-2)
#define DGA_BAD_FORMAT (-3)
#include <stdint.h>

//A DGA can be a queue, a stack or an array.
//It consists of:
//  current -> to keep track of the stack
//  size -> to keep track of the size

#define DGAHEADER(type,shtype,structname) \
typedef struct {uint32_t size,current;type* data;} structname;\
int push ## shtype ## structname (structname * array,type value);\
int set ## shtype ##structname (structname * array,const type value,uint32_t index);\
type get ## shtype ## structname (const structname * array,uint32_t index);\
type pop ## shtype ## structname (structname * array);\
/*adds a 0 to the end (eq to push(..,0))*/\
int finalize ##shtype ## structname (structname * array);\
structname * create ## shtype ## structname (uint32_t size);\
int addAll ## shtype ## structname (structname * array, const type * buffer,uint32_t size);\
void free ## shtype ## structname (structname * array);


#define DGAIMPL(type, shtype, structname,errorIndexBounds,finalElement) \
    int grow ## structname ## IfNeeded(structname * array, const uint32_t needed) {\
    if (array->current + needed >= array->size) {\
        type* cp = realloc(array->data, (array->size+DGA_INC)*sizeof(type));\
        if (!cp) { fprintf(stderr, "Failed to allocate memory. errno:%i, %s,%i", (*_errno()),__FILE__,__LINE__); return DGA_BAD_MEM_ALLOC;}\
        array->data=cp; } return DGA_INDEX_OUT_OF_BOUNDS;}\
int push ## shtype ## structname (structname * array, const type value) {\
if (!grow ##structname ## IfNeeded(array,1)) { return DGA_BAD_MEM_ALLOC; }\
array->data[array->current++]=value;return 0;}\
int set ## shtype ## structname (structname * array, const type value, const uint32_t index) {\
    if (!grow ##structname ## IfNeeded(array,1)) { return DGA_BAD_MEM_ALLOC;}\
    if (index>=array->size) { return DGA_INDEX_OUT_OF_BOUNDS; }\
    array->data[index]=value;return 0;}\
type get ## shtype ## structname (const structname * array, const uint32_t index) {\
    if (index>=array->size) { return errorIndexBounds;}\
    return array->data[index];}\
int finalize ## shtype ##structname (structname * array) { return push ##shtype ## structname (array,finalElement);}\
type pop ##shtype ## structname( structname * array) {\
    if (array->current<=0) { return errorIndexBounds; }\
    return array->data[array->current--];}\
structname * create ## shtype ## structname(const uint32_t size) {\
    structname* array=malloc(sizeof(structname));\
    if (!array) { return NULL;} array->data = malloc(size*sizeof(type)); if (!array->data) { free(array); return NULL; }\
    array->size=size; array->current=0; return array;}\
int addAll ##shtype ## structname ( structname * array, const type* buffer, const uint32_t size) {\
    uint32_t index=0; while (index<size) { if (!push ##shtype ## structname (array,buffer[index++])) { return DGA_BAD_MEM_ALLOC;}} return 0;}\
void free ## shtype ## structname ( structname * array) { free(array->data); free(array);}

#define INTDGAIMPL(type,shtype,structname) DGAIMPL(type,shtype,structname,DGA_BAD_MEM_ALLOC,DGA_INDEX_OUT_OF_BOUNDS,0)

/*
int putFormatDGA(dga* array, const char* format, const va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    const int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    if (needed < 0) return DGA_BAD_FORMAT;
    char* buffer = malloc(needed);
    if (!buffer) return DGA_BAD_MEM_ALLOC;
    vsnprintf(buffer, needed, format, args);
    addAllDGA(array, buffer, needed);
    free(buffer);
    return 0;
}
*/

#endif //FUZZY_KTS_DGALIB_H
