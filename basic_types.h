#ifndef BASIC_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define function static
#define global static
#define local_persist static

#define U32Max 0xffffffffu

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
        float x, y;
    };
    float e[2];
};

inline function V2 v2(float x=0.0f, float y=0.0f){
    V2 result = {x, y};
    return(result);
}

#define BASIC_TYPES_H
#endif //BASIC_TYPES_H
