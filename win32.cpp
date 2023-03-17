//[x] Load xinput.dll, XInputGetState, XInputSetState.
//[x] Load application dll
//[ ] Implement our own sprintf_s(char *buf, size_t buf_size, char *format, ...) function in order to be able to print debug information.
//[ ] Add a String8 type in basic_types.h file.
//[ ] Implement error handling for window class registration and window creation, revisit the api.

#include "win32.h"

global Win32 global_win32;

function FileLoadResult win32_load_entire_file_into_memory(char *file_name){
    FileLoadResult result = {};
    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, 0, 0,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, /*FILE_ATTRIBUTE_NORMAL*/ 0);
    if(file_handle != INVALID_HANDLE_VALUE){
        GetFileSizeEx(file_handle, (PLARGE_INTEGER)&result.size);
        ULARGE_INTEGER file_compressed_size;
        file_compressed_size.LowPart = GetCompressedFileSizeA(file_name, &file_compressed_size.HighPart);
        result.compressed_size = file_compressed_size.QuadPart;
        
        void *memory = VirtualAlloc(0, result.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if(memory){
            DWORD bytes_read;
            assert(result.size <= U32Max);
            if(ReadFile(file_handle, memory, result.size, &bytes_read, 0) &&
               (bytes_read == result.size)){
                result.data = memory;
            }
            else{
                //TODO(oleksii): 
                VirtualFree(memory, 0, MEM_RELEASE);
                debug_break();
            }
        }
        else{
            //TODO(oleksii): 
            debug_break();
        }
        CloseHandle(file_handle);
    }
    else{
        //TODO(oleksii): 
        debug_break();
    }
    return(result);
}

function B32 win32_write_memory_to_file(char *file_name, I64 size, void *memory){
    B32 result = false;
    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, 0,
                                     CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); //0
    if(file_handle){
        DWORD bytes_written;
        if(WriteFile(file_handle, memory, size, &bytes_written, 0) &&
           (bytes_written == size)){
            result = true;
        }
        else{
            //TODO(oleksii): 
            debug_break();
        }
        CloseHandle(file_handle);
    }
    else{
        //TODO(oleksii): 
        debug_break();
    }
    return(true);
}

function void win32_free_file_memory(void *memory){
    if(memory){
        VirtualFree(memory, 0, MEM_RELEASE);
    }
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

function void win32_load_app_code(Win32 *win32, char *dll_path){
    HMODULE app_library = LoadLibraryA(dll_path);
    if(app_library){
        win32->app_dll = app_library;
        win32->app_update_and_render = (AppUpdateAndRenderPtr)GetProcAddress(win32->app_dll,
                                                                             "app_update_and_render");
        if(!win32->app_update_and_render){
            win32->app_update_and_render = app_update_and_render_stub;
        }
    }
    else{
        //TODO(oleksii): Error handling, failed to load the application code.
        debug_break();
    }
}

function void win32_unload_app_code(Win32 *win32){
    if(win32->app_dll){
        FreeLibrary(win32->app_dll);
        win32->app_dll = 0;
    }
}

function void win32_init_xinput(Win32 *win32){
    HMODULE xinput_library = LoadLibraryA("xinput1_4.dll");
    xinput_library = xinput_library ? xinput_library : LoadLibraryA("xinput1_3.dll");
    xinput_library = xinput_library ? xinput_library : LoadLibraryA("xinput9_1_0.dll");
    if(xinput_library){
        win32->xinput_get_state = (XinputGetStatePtr)GetProcAddress(xinput_library, "XInputGetState");
        win32->xinput_set_state = (XinputSetStatePtr)GetProcAddress(xinput_library, "XInputSetState");
        if(!win32->xinput_get_state && 
           !win32->xinput_set_state){
            win32->xinput_get_state = xinput_get_state_stub;
            win32->xinput_set_state = xinput_set_state_stub;
        }
    }
    else{
        win32->xinput_get_state = xinput_get_state_stub;
        win32->xinput_set_state = xinput_set_state_stub;
    }
}

function void win32_init_gdi(Win32 *win32){
    HMODULE gdi_library = LoadLibraryA("gdi32.dll");
    if(gdi_library){
        win32->get_stock_object = (GetStockObjectPtr)GetProcAddress(gdi_library, "GetStockObject");
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
    win32_init_xinput(&global_win32);
    win32_register_window_class(&global_win32);
    win32_create_window(&global_win32, 1080, 720);
    win32_show_window(&global_win32);
    win32_load_app_code(&global_win32, "e:/work/build/application.dll");
    
    Memory memory = {};
    Input input = {};
    
    global_win32.running = true;
    while(global_win32.running){
        MSG msg;
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        for(I32 controller_index = 0; 
            controller_index < XUSER_MAX_COUNT; 
            controller_index += 1){
            XINPUT_STATE controller_state = {};
            memset(&controller_state, 0, sizeof(XINPUT_STATE));
            if(global_win32.xinput_get_state(controller_index, &controller_state) == ERROR_SUCCESS){
                XINPUT_GAMEPAD *gamepad = &controller_state.Gamepad;
                U16 dpad_up = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                U16 dpad_down = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                U16 dpad_left = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                U16 dpad_right = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                U16 start = (gamepad->wButtons & XINPUT_GAMEPAD_START);
                U16 back = (gamepad->wButtons & XINPUT_GAMEPAD_BACK);
                U16 left_thumb = (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                U16 right_thumb = (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                U16 left_shoulder = (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                U16 right_shoulder = (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                U16 a_button = (gamepad->wButtons & XINPUT_GAMEPAD_A);
                U16 b_button = (gamepad->wButtons & XINPUT_GAMEPAD_B);
                U16 x_button = (gamepad->wButtons & XINPUT_GAMEPAD_X);
                U16 y_button = (gamepad->wButtons & XINPUT_GAMEPAD_Y);
                
                V2 trigger = v2(((gamepad->bLeftTrigger < 30) ? 0 : gamepad->bLeftTrigger), 
                                ((gamepad->bRightTrigger < 30) ? 0 : gamepad->bRightTrigger));
                V2 l_stick = v2();
                V2 r_stick = v2();
                
                if((gamepad->sThumbLX > XINPUT_LEFT_STICK_DEADZONE) ||
                   (gamepad->sThumbLX < -XINPUT_LEFT_STICK_DEADZONE)){
                    l_stick.x = ((gamepad->sThumbLX < 0) ? 
                                 (gamepad->sThumbLX / 32768.0f) : (gamepad->sThumbLX / 32767.0f));
                }
                if((gamepad->sThumbLY > XINPUT_LEFT_STICK_DEADZONE) ||
                   (gamepad->sThumbLY < -XINPUT_LEFT_STICK_DEADZONE)){
                    l_stick.y = ((gamepad->sThumbLY < 0) ? 
                                 (gamepad->sThumbLY / 32768.0f) : (gamepad->sThumbLY / 32767.0f));
                }
                if((gamepad->sThumbRX > XINPUT_RIGHT_STICK_DEADZONE) ||
                   (gamepad->sThumbRX < -XINPUT_RIGHT_STICK_DEADZONE)){
                    r_stick.x = ((gamepad->sThumbRX < 0) ? 
                                 (gamepad->sThumbRX / 32768.0f) : (gamepad->sThumbRX / 32767.0f));
                }
                if((gamepad->sThumbRY > XINPUT_RIGHT_STICK_DEADZONE) ||
                   (gamepad->sThumbRY < -XINPUT_RIGHT_STICK_DEADZONE)){
                    r_stick.y = ((gamepad->sThumbRY < 0) ? 
                                 (gamepad->sThumbRY / 32768.0f) : (gamepad->sThumbRY / 32767.0f));
                }
                
                //TODO(oleksii): Might be useful in the future to print debug information.
                //So, not removing it for now!
                {
                    char DEBUG_buf[1 << 10] = {};
                    char *fmt = "Lstick(%f, %f)\nRstick(%f, %f)\nTrigger(%f, %f)\n\n";
                    sprintf_s(DEBUG_buf, sizeof(DEBUG_buf), fmt,
                              l_stick.x, l_stick.y, r_stick.x, r_stick.y, trigger.x, trigger.y);
                    OutputDebugStringA(DEBUG_buf);
                }
            }
            else{
            }
        }
        global_win32.app_update_and_render(&memory, &input);
    }
    
    return(0);
}
