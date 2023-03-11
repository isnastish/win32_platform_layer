
#define function static
#define global static
#define local_persist static

#include "win32.h"

////////////////////////////////
//NOTE(oleksii): Platform independent code
#include <stdint.h>

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

global Win32 global_win32;

function void win32_init_gdi(Win32 *win32){
    win32->gdi_dll = LoadLibraryA("gdi32.dll");
    if(win32->gdi_dll){
        win32->get_stock_object = (GetStockObjectPtr)GetProcAddress(win32->gdi_dll, "GetStockObject");
        if(!win32->get_stock_object){
            win32->get_stock_object = get_stock_object_stub;
        }
    }
}

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param){ 
    LRESULT result = 0;
    switch(message){
        default:{
            result = DefWindowProc(window, message, w_param, l_param);
        }
    } 
    return 0; 
}

function void win32_register_window_class(Win32 *win32){
    
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_code){
    win32_init_gdi(&global_win32);
    
    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_main_window_procedure;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)global_win32.get_stock_object(DKGRAY_BRUSH);
    window_class.lpszClassName = "WindowClass";
    
    ATOM atom = RegisterClassA(&window_class);
    
    return(0);
}
