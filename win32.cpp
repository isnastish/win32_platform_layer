//TODO(oleksii): 
//[x] Create window.
//    [x] Compute the size of the client area using AdjustWindowRect
//    [x] Make a separate function for showing/updating the window. 
//[x] Move window creation and window class creation to its own functions (outside main)
//[ ] Implement error handling for window class registration and window creation, revisit the api.

#define function static
#define global static
#define local_persist static

////////////////////////////////
//NOTE(oleksii): Platform independent code
#include <stdint.h>
#include <assert.h>

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
        float x, y;
    };
    float e[2];
};

inline function V2 v2(float x, float y){
    V2 result = {x, y};
    return(result);
}

#include "win32.h"

#define debug_break() __debugbreak();

global Win32 global_win32;

function V2 win32_get_window_metrics(Win32 *win32){
    V2 result = {};
    RECT client_rect;
    GetClientRect(win32->window, &client_rect);
    result.x = client_rect.right - client_rect.left;
    result.y = client_rect.bottom - client_rect.top;
    win32->window_width = result.x;
    win32->window_height = result.y;
    return(result);
}

function void win32_init_gdi(Win32 *win32){
    win32->gdi_dll = LoadLibraryA("gdi32.dll");
    if(win32->gdi_dll){
        win32->get_stock_object = (GetStockObjectPtr)GetProcAddress(win32->gdi_dll, "GetStockObject");
        if(!win32->get_stock_object){
            win32->get_stock_object = get_stock_object_stub;
        }
    }
}

function void win32_register_window_class(Win32 *win32){
    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_main_window_procedure;
    window_class.hInstance = GetModuleHandle(0);
    window_class.hbrBackground = (HBRUSH)win32->get_stock_object(DKGRAY_BRUSH);
    window_class.lpszClassName = "WindowClass";
    assert(RegisterClassA(&window_class));
}

function void win32_create_window(Win32 *win32, I32 width, I32 height){
    win32->window_width = width;
    win32->window_height = height;
    RECT window_rect = {0, 0, win32->window_width, win32->window_height};
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, 0);
    win32->window = CreateWindowExA(0, "WindowClass", "PlatformLayer",
                                    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                    window_rect.right - window_rect.left, 
                                    window_rect.bottom - window_rect.top,
                                    0, 0, GetModuleHandleA(0), 0);
    assert(win32->window);
}

function void win32_show_window(Win32 *win32){
    ShowWindow(win32->window, SW_SHOW);
    UpdateWindow(win32->window);
}

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param){ 
    LRESULT result = 0;
    switch(message){
        case WM_CLOSE:
        case WM_DESTROY:{
            global_win32.running = false;
        }break;
        
        case WM_SIZE:{
            win32_get_window_metrics(&global_win32);
        }break;
        
        default:{
            result = DefWindowProc(window, message, w_param, l_param);
        }
    } 
    return(result); 
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_code){
    win32_init_gdi(&global_win32);
    win32_register_window_class(&global_win32);
    win32_create_window(&global_win32, 1080, 720);
    win32_show_window(&global_win32);
    
    global_win32.running = true;
    while(global_win32.running){
        MSG msg;
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
    
    return(0);
}
