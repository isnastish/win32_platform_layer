#include "application.h"
#define APPAPI extern "C" __declspec(dllexport)

#if defined(_WIN32)
#include "win32.h"
#endif

APPAPI APP_UPDATE_AND_RENDER(app_update_and_render){
    platform_ptr = platform;
    FileLoadResult result = platform_ptr->load_entire_file("e:/work/win32_platform_layer/code/win32.cpp");
}