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
    union{
        char *data;
        U8 *data_u8;
    };
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

inline B32 strings_equal(char *a, char *b){
    B32 result = (a == b);
    while((*a == *b) && (*a && *b)){
        ++a;
        ++b;
    }
    result = ((*a == *b) && (!*a));
    return(result);
}

inline B32 strings_equal(I64 a_len, char *a, I64 b_len, char *b){
    B32 result = (a_len == b_len);
    if(result){
        I32 count = 0;
        while((*a++ == *b++) && (count < a_len)){
            ++count;
        }
        result = (count == a_len);
    }
    return(result);
}

inline B32 strings_equal(I64 a_len, U8 *a, I64 b_len, U8 *b){
    return(strings_equal(a_len, (char *)a, b_len, (char *)b));
}

inline B32 strings_equal(String8 a, String8 b){
    return(strings_equal(a.size, (char *)a.data, b.size, (char *)b.data));
}

inline B32 strings_equal(U8 *a, U8 *b){
    return(strings_equal((char *)a, (char *)b));
}

inline char *string_copy(char *dst, char *src, I64 count){
    I32 index = 0;
    while(index < count){
        dst[index] = *src;
    }
    return(dst);
}

inline U8 *string_copy(U8 *dst, U8 *src, I64 count){
    return((U8 *)string_copy((char *)dst, (char *)src, count));
}

#define BASIC_TYPES_H
#endif //BASIC_TYPES_H
