#ifndef BASIC_TYPES_H

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <string>

#define assert(expr) if(!(expr)){ *((int *)0) = 0xff; }else{}
#define size_of(array) (sizeof(array) / sizeof((array)[0]))
#define array_count(array) (sizeof(array) / sizeof((array)[0]))

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
        float x, y;
    };
    float e[2];
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

inline char *str_concat(char *dest,
                        const char *source){
    char *at = dest;
    while(*at){
        ++at;
    }
    while((*at++ = *source++) != '\0'); //make sure that we do null-terminate the final string
    return(dest);
}

inline char *str_concat(char *dest,
                        const char *source, /*not necessary null-terminated string*/
                        MemIndex count){
    char *at = dest;
    while(*at){
        ++at;
    }
    while(--count){ //have to be carefull here, because it's an unsigned value
        *at++ = *source++;
    }
    return(dest);
}


inline char *str_copy(char *dest, const char *source){
    while((*dest++ = *source++) != 0){
        ;
    }
    return(dest);
}

inline char *str_copy(char *dest, const char *source, MemIndex count){
    MemIndex index;
    for(index = 0;
        index < count;
        index += 1){
        dest[index] = source[index];
    }
    assert(index == count);
    return(dest);
}

//TODO(alexey): Should we return I64 or MemIndex type???
inline I64 str_length(char *str){
    I64 result = 0;
    while(*str){
        ++str;
        ++result;
    }
    return(result);
}

inline B32 str_equal(char *a, char *b){
    B32 result = (a == b);
    while((*a == *b) && (*a && *b)){
        ++a;
        ++b;
    }
    result = ((*a == *b) && (!*a));
    return(result);
}

inline B32 str_equal(MemIndex len_a, char *a, MemIndex len_b, char *b){
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

inline B32 str_equal(Str8 a, Str8 b){
    return(str_equal(a.size, a.data, b.size, b.data));
}

inline char *int_to_str(I64 value, char *buf, I32 radix_/*not used*/){
    I32 index = 0;
    char value_inv[256];
    while(value){
        value_inv[index++] = (char)('0' + (value % 10));
        value /= 10;
    }
    while(index){
        *buf++ = value_inv[--index];
    }
    return(buf);
}

inline char *uint_to_str(U64 value, char *buf, I32 radix_/*not used*/){
    //TODO(alexey): Not implemented yet!
}

inline char *my_sprintf(char *buf, MemIndex buf_size, char *fmt, ...){
    va_list args_list;
    va_start(args_list, fmt);
    I64 fmt_len = str_length(fmt);
    assert(buf_size > fmt_len);
    I32 pos = 0;
    for(char *at = fmt;
        *at;
        at += 1){
        if(*at != '%'){
            buf[pos++] = *at;
            continue;
        }
        at += 1; //skip '%'
        switch(*at){
            case 's':{
                char *str = va_arg(args_list, char *);
                I64 str_len = str_length(str);
                assert((fmt_len + str_len - 2) < buf_size);
                for(; *str; str += 1){
                    buf[pos++] =  *str;
                }
            }break;
            case 'i':{
                I64 value = va_arg(args_list, I32);
                I32 index = 0;
                char value_inv[256] = {};
                if(value < 0){
                    value *= -1; //get rid of sign
                    buf[pos++] = '-';
                }
                while(value){
                    value_inv[index++] = (char)('0' + (value % 10));
                    value /= 10;
                }
                while(index){
                    buf[pos++] = value_inv[--index];
                }
            }break;
            case 'u':{
                //TODO(alexey): Collapse into a function!!!
                //And simplify this thing as well.
                U64 value = va_arg(args_list, U32);
                if(value){
                    I32 index = 0;
                    char value_inv[256] = {};
                    while(value){
                        value_inv[index++] = (char)('0' + (value % 10));
                        value /= 10;
                    }
                    while(index){
                        buf[pos++] = value_inv[--index];
                    }
                }
                else{
                    buf[pos++] = '0';
                }
            }break;
            //TODO(alexey): In the future support (V|v)(2|3|4)(i|f) -> V2f, v2f, V3i ...
            case 'v':
            case 'V':{
                at += 1;
                if(*at == '2'){
                    V2 value = va_arg(args_list, V2);
                    std::string x = std::to_string(value.x);
                    std::string y = std::to_string(value.y);
                    char *cx = (char *)x.c_str();
                    char *cy = (char *)y.c_str();
                    while(*cx){
                        buf[pos++] = *cx++;
                    }
                    buf[pos++] = ',';
                    buf[pos++] = ' ';
                    while(*cy){
                        buf[pos++] = *cy++;
                    }
                }
                else{
                    //TODO(alexey): Format is not supported.
                    assert(0);
                }
            }break;
            
            default:{
                //TODO(alexey): Error handling, invalid format specifier!
                assert(0);
            }break;
        }
    }
    buf[pos] = 0;
    va_end(args_list);
    return(buf);
}


// TODO(alx): Optimize concatenation function. And maybe return the number of copied characters into the destination buffer.
function void cat_strings(MemIndex source_a_count, char *source_a,
                          MemIndex source_b_count, char *source_b,
                          MemIndex dest_count, char *dest){
    assert((source_a_count + source_b_count + 1) <= dest_count); // reserve 1 for null-terminated character.
    for(I32 index = 0;
        index < source_a_count;
        index += 1){
        *dest++ = *source_a++;
    }
    for(I32 index = 0;
        index < source_b_count;
        index += 1){
        *dest++ = *source_b++;
    }
    *dest++ = 0;
}

#define BASIC_TYPES_H
#endif //BASIC_TYPES_H
