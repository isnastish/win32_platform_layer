#ifndef PLATFORM_H
#include "basic_types.h"

#define PLATFORM_ALLOCATE_MEMORY(name) void *name(I64 size, void *base_address)
typedef void *(*AllocateMemoryPtr)(I64 size, void *base_address);

#define PLATFORM_FREE_MEMORY(name) void name(void *memory)
typedef void (*FreeMemoryPtr)(void *memory);

//maybe prefix with DEBUG as well?
struct FileLoadResult{
    I64 size;
    I64 compressed;
    void *data;
};

#define DEBUG_PLATFORM_LOAD_ENTIRE_FILE(name) FileLoadResult name(char *file_name)
typedef FileLoadResult (*LoadEntireFilePtr)(char *file_name);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) B32 name(char *file_name, I64 size, void *memory)
typedef B32 (*WriteEntireFilePtr)(char *file_name, I64 size, void *memory);

#define DEBUG_PLATFORM_FREE_FILE(name) void name(void *memory)
typedef void (*FreeFilePtr)(void *memory);

struct PlatformApi{
    //file io
    LoadEntireFilePtr load_entire_file;
    WriteEntireFilePtr write_entire_file;
    FreeFilePtr free_file;
    
    //memory
    AllocateMemoryPtr allocate_memory;
    FreeMemoryPtr free_memory;
};

struct Memory{
    B32 initialized;
    
    I64 frame_storage_size;
    void *frame_storage;
    
    I64 permanent_storage_size;
    void *permanent_storage;
    
    PlatformApi *platform;
};

struct Input{
};

global PlatformApi *platform;

#define APP_UPDATE_AND_RENDER(name) void name(Input *input, Memory *memory)
typedef void (*AppUpdateAndRenderPtr)(Input *input, Memory *memory);

#define PLATFORM_H
#endif //PLATFORM_H
