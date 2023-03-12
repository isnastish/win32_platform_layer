//TODO(oleksii): 
//[ ] Implement error handling for window class registration and window creation, revisit the api.

#include "win32.h"

global Win32 global_win32;

function FileLoadResult win32_load_entire_file(char *file_path){
    FileLoadResult result = {};
    /* 
        HANDLE file_handle = CreateFileA((LPCSTR)file_path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL, 0);
        if(file_handle != INVALID_HANDLE_VALUE){
            I64 file_size;
            GetFileSizeEx(file_handle, (PLARGE_INTEGER)&file_size);
            
            //maybe it's better to cast it directly here to char * and replace void * with char * in FileLoadResult struct definition.
            void *file_data = VirtualAlloc();
            if(file_data){
                BOOL ReadFile(HANDLE       hFile,
                              LPVOID       lpBuffer,
                              DWORD        nNumberOfBytesToRead,
                              LPDWORD      lpNumberOfBytesRead,
                              LPOVERLAPPED lpOverlapped);
                
                //if read fails!
                VirtualFree();
            }
            CloseHandle(file_handle);
        }
        else{
            //TODO(oleksii): Error handling or diagnostic.
        }
     */
    return(result);
}

function void win32_write_to_file(void *memory){
    //TODO(oleksii):
}


function void win32_switch_fullscreen(Win32 *win32){
    //Raymond Chan's article: https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
    local_persist WINDOWPLACEMENT window_placement = {};
    window_placement.length = sizeof(window_placement);
    
    DWORD style = GetWindowLongA(win32->window, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW){
        MONITORINFO monitor_info = {};
        monitor_info.cbSize = sizeof(monitor_info);
        if(GetWindowPlacement(win32->window, &window_placement) && 
           GetMonitorInfoA(MonitorFromWindow(win32->window, MONITOR_DEFAULTTOPRIMARY), &monitor_info)){
            DWORD new_style = (style & ~WS_OVERLAPPEDWINDOW);
            SetWindowLongA(win32->window, GWL_STYLE, new_style);
            SetWindowPos(win32->window, HWND_TOP,
                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, 
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else{
        SetWindowLongA(win32->window, GWL_STYLE, (style | WS_OVERLAPPEDWINDOW));
        SetWindowPlacement(win32->window, &window_placement);
        SetWindowPos(win32->window, 0, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

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
        case WM_LBUTTONDOWN:{
            win32_switch_fullscreen(&global_win32);
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
    
    //win32_load_entire_file("e:/work/win32_platform_layer/code/win32.cpp");
    
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
