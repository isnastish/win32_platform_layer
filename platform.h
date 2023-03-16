#ifndef PLATFORM_H
#include "basic_types.h"

struct Memory{
};

struct Input{
};

#define APP_UPDATE_AND_RENDER(name) void name(Memory *memory, Input *input)
typedef void (*AppUpdateAndRenderPtr)(Memory *memory, Input *input);
APP_UPDATE_AND_RENDER(app_update_and_render_stub){ return; }

#define PLATFORM_H
#endif //PLATFORM_H
