#ifndef PLATFORM_H
#include "basic_types.h"

#define PLATFORM_ALLOCATE_MEMORY(name) void *name(I64 size, void *base_address)
typedef void *(*AllocateMemoryPtr)(I64 size, void *base_address);

#define PLATFORM_FREE_MEMORY(name) void name(void *memory)
typedef void (*FreeMemoryPtr)(void *memory);

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
    LoadEntireFilePtr load_entire_file;
    WriteEntireFilePtr write_entire_file;
    FreeFilePtr free_file;
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

// TODO(alx): Study Casey's approach of handling input. (coputing half_transiions rather than sending events)
struct ButtonState{
    I32 half_transition_count;
    B32 ended_down;
};

// A controller can be either a gamepad or keyboard controller
struct Controller{
    ButtonState a_button;
    B32 is_connected;
};

#define MAX_CONTROLLER_COUNT 4u // + 1 for keyboard controller
struct Input{
    Controller controllers[MAX_CONTROLLER_COUNT];
};

global PlatformApi *platform;

#define APP_UPDATE_AND_RENDER(name) void name(Input *_input, Memory *_memory)
typedef void (*AppUpdateAndRenderPtr)(Input *input, Memory *memory);

#define PLATFORM_H
#endif //PLATFORM_H
