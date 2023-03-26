#ifndef PLATFORM_H
#include "basic_types.h"

struct FileLoadResult{
    I64 size;
    I64 compressed;
    void *data;
};

typedef FileLoadResult (*LoadEntireFilePtr)(char *file_name);
typedef B32 (*WriteEntireFilePtr)(char *file_name, I64 size, void *memory);
typedef void (*FreeFilePtr)(void *memory);

struct Platform{
    LoadEntireFilePtr load_entire_file;
    WriteEntireFilePtr write_entire_file;
    FreeFilePtr free_file;
};

global Platform *platform_ptr;

#define APP_UPDATE_AND_RENDER(name) void name(Platform *platform)
typedef void (*AppUpdateAndRenderPtr)(Platform *platform);
APP_UPDATE_AND_RENDER(app_update_and_render_stub){ return; }

#define PLATFORM_H
#endif //PLATFORM_H
