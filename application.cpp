#include "application.h"
#define APPAPI extern "C" __declspec(dllexport)

//NOTE(oleksii): Including windows.h just for testing purposes. Mainly because of debug_break();
#if defined(_WIN32)
#include "win32.h"
#endif

APPAPI APP_UPDATE_AND_RENDER(app_update_and_render){
    I32 x_ = 0;
}