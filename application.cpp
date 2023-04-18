#include "application.h"

//NOTE(oleksii): Should be replaced with /EXPORT because __declspec(dllexport) is windows-specific thing.
#define APPAPI extern "C" __declspec(dllexport)

#if defined(_WIN32)
#include "win32.h"
#endif

APPAPI APP_UPDATE_AND_RENDER(app_update_and_render){
    platform = memory->platform;
    if(!memory->initialized){
        FileLoadResult result = platform->load_entire_file("e:/work/win32_platform_layer/code/win32.cpp");
        memory->initialized = true;
    }
    
    Input *input_ptr = input;
    if(input_ptr){
        __debugbreak();
    }
    
    glClearColor(0.0f/255.0f, 130.0f/255.0f, 97.0f/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}