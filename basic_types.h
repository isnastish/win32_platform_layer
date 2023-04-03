#ifndef BASIC_TYPES_H

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define function static
#define global static
#define local_persist static

#define U32Max 0xffffffffu
#define Terabytes(n) ((n)*(1024ull)*(1024ull)*(1024ull)*(1024ull))
#define Gigabytes(n) ((n)*(1024ull)*(1024ull)*(1024ull))
#define Megabytes(n) ((n)*(1024ull)*(1024ull))
#define Kilobytes(n) ((n)*(1024ull))

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

typedef I32 B32;

union V2{
    struct{
        F32 x, y;
    };
    F32 e[2];
};

inline function V2 v2(F32 x=0.0f, F32 y=0.0f){
    V2 result = {x, y};
    return(result);
}

struct String8{
    I64 size;
    char *data;
};

inline String8 make_string8(I64 size, char *str){
    String8 result = {size, str};
    return(result);
}

#define Str8(str) make_string8((sizeof(str) - 1), (str))
#define Str8Comp(str) {(sizeof(str) - 1), (str)}

inline B32 is_space(U8 c){
    return(c == ' ');
}

#define BASIC_TYPES_H
#endif //BASIC_TYPES_H
