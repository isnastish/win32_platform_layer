#ifndef BASIC_TYPES_H

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#undef assert
#define assert(expr) if(!(expr)){ *((int *)0) = 0xff; }else{}

#define size_of(array) (sizeof(array) / sizeof((array)[0]))

#define function static
#define global static
#define local_persist static

#define U32Max 0xffffffffu
#define Terabytes(n) ((n)*(1024ull)*(1024ull)*(1024ull)*(1024ull))
#define Gigabytes(n) ((n)*(1024ull)*(1024ull)*(1024ull))
#define Megabytes(n) ((n)*(1024ull)*(1024ull))
#define Kilobytes(n) ((n)*(1024ull))

//TODO(alexey): Once we have our own strncpy/cmp we can remove these macros.
#define strncpy_u8(dest, src, size) strncpy((char *)(dest), (const char *)(src), (size_t)(size))
#define strncmp_u8(s1, s2, count) strncmp((const char *)(s1), (const char *)(s2), (size_t)(count))
#define sprintf_s_u8(buf, size, fmt, ...) sprintf_s((char *)buf, size, (const char *)(fmt), ## __VA_ARGS__)

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

typedef size_t MemIndex;

typedef uintptr_t Umm;

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

struct Str8{
    I64 size;
    char *data;
};

inline Str8 make_str8(I64 size, char *str){
    Str8 result = {size, str};
    return(result);
}

#define str8(str) make_str8((sizeof(str) - 1), (str))
#define str8_comp(str) {(sizeof(str) - 1), (str)}

inline B32 is_space(U8 c){
    return(c == ' ');
}

inline B32 strings_equal(char *a, char *b){
    B32 result = (a == b);
    while((*a == *b) && (*a && *b)){
        ++a;
        ++b;
    }
    result = ((*a == *b) && (!*a));
    return(result);
}

inline B32 strings_equal(MemIndex len_a, char *a, MemIndex len_b, char *b){
    B32 result = (len_a == len_b);
    if(result){
        I32 count = 0;
        while((*a++ == *b++) && (count < len_a)){
            ++count;
        }
        result = (count == len_a);
    }
    return(result);
}

inline B32 strings_equal(Str8 a, Str8 b){
    return(strings_equal(a.size, a.data, b.size, b.data));
}

//TODO(alexey): Think about better naming
inline char *my_sprintf(char *buf, MemIndex buf_size, char *fmt, ...){
    va_list args_list;
    va_start(args_list, fmt);
    for(;;){
    }
    va_end(args_list);
}

#define BASIC_TYPES_H
#endif //BASIC_TYPES_H
