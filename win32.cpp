//TODO:
//
//[x] Finish todos for error handling
//[x] Refactor the codebase, make everything explicit!
//[x] Create functions for memory allocation/deallocation so we can call them in the application
//    (in case we need some additional memory)
//[x] Init OpenGL, load opengl procedures
//    [x] Move platform-specific opengl code (wgl) to its own file.
//    [ ] Experiment with opengl extensions to understand them better.
//    [ ] Learn about Vsync and get it to work, I'm assuming using wglSwapIntervalsEXT function?
//    [ ] Move platform-independent opengl code in win32.h somewhere else, because it shouldn't really be there.
//        OpenglInfo struct and gl_get_info() procedure.
//[ ] Finish loading app_code as a dll. Implement live code editing. use FILETIME? GetFileTime, SetFileTime.
//[ ] Implement our own sprintf function with %v2, %v3, %v4, %m2, %m3, %m4 formats (and all the standart formats as well)
//[ ] Compute the full path to .dll. Maybe retrieve somehow current working directory.
//[ ] Pack gamepad input into some struct and pass to app_update_and_render()
//[ ] Mouse/keyboard input (maybe raw input in as well, just to be familiar with it)
//[ ] Understand how to create projects in 4coder. The goal is to specify the directory to the source files
//    so they can be loaded automatically.
//[x] Implement my own version of strncpy. Currently it's used in oepngl.cpp while parsing extensions
//[x] my_sprintf(), support only %s format for now.
//[ ] Think about better name for my_sprintf function.
//[ ] string_concat(char *dest, const char *source)
//    [ ] string_concat(char *dest, const char *source, MemIndex source_count)

#include "win32.h"

#include "opengl.cpp"
#include "win32_opengl.cpp"

////////////////////////////////
//NOTE(oleksii): Global variables
global B32 global_running;
global Win32AppCode global_app_code;


function void win32_get_file_write_time(char *file_name){
    BOOL SetFileTime(HANDLE hFile,
                     const FILETIME *lpCreationTime,
                     const FILETIME *lpLastAccessTime,
                     const FILETIME *lpLastWriteTime);
    
    LONG CompareFileTime(const FILETIME *lpFileTime1,
                         const FILETIME *lpFileTime2);
    
    BOOL GetFileTime(HANDLE hFile,
                     LPFILETIME lpCreationTime,
                     LPFILETIME lpLastAccessTime,
                     LPFILETIME lpLastWriteTime);
}

PLATFORM_ALLOCATE_MEMORY(win32_allocate_memory){
    I64 alloc_size = size;
    void *result = VirtualAlloc(base_address, alloc_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    assert(result);
    return(result);
}

PLATFORM_FREE_MEMORY(win32_free_memory){
    if(memory){
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

function void win32_load_app_code(Win32AppCode *app_code, char *dll_name){
    app_code->dll = LoadLibraryA(dll_name);
    if(app_code->dll){
        app_code->update_and_render = (AppUpdateAndRenderPtr)GetProcAddress(app_code->dll, "app_update_and_render");
        if(app_code->update_and_render){
            app_code->valid = true;
        }
    }
    if(!app_code->valid){
        app_code->update_and_render = 0;
    }
}

function void win32_unload_app_code(Win32AppCode *app_code){
    if(app_code->dll){
        FreeLibrary(app_code->dll);
        app_code->valid = false;
    }
}

function void win32_init_xinput(){
    HMODULE xinput_library = LoadLibraryA("xinput1_4.dll");
    xinput_library = xinput_library ? xinput_library : LoadLibraryA("xinput1_3.dll");
    xinput_library = xinput_library ? xinput_library : LoadLibraryA("xinput9_1_0.dll");
    if(xinput_library){
        XInputGetState = (XInputGetStatePtr)GetProcAddress(xinput_library, "XInputGetState");
        XInputSetState = (XInputSetStatePtr)GetProcAddress(xinput_library, "XInputSetState");
        if(!XInputGetState &&
           !XInputSetState){
            XInputGetState = xinput_get_state_stub;
            XInputSetState = xinput_set_state_stub;
        }
    }
}

function 
DEBUG_PLATFORM_LOAD_ENTIRE_FILE(win32_load_entire_file){
    FileLoadResult result = {};
    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
    if(file_handle != INVALID_HANDLE_VALUE){
        GetFileSizeEx(file_handle, (PLARGE_INTEGER)&result.size);
        ULARGE_INTEGER file_compressed_size;
        file_compressed_size.LowPart = GetCompressedFileSizeA(file_name, &file_compressed_size.HighPart);
        result.compressed = file_compressed_size.QuadPart;
        void *memory = VirtualAlloc(0, result.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if(memory){
            DWORD bytes_read;
            assert(result.size <= U32Max);
            if(ReadFile(file_handle, memory, result.size, &bytes_read, 0) &&
               (bytes_read == result.size)){
                result.data = memory;
            }
            else{
                //TODO(oleksii): error handline (failed to read a file)
                VirtualFree(memory, 0, MEM_RELEASE);
            }
        }
        else{
            //TODO(oleksii): error handling (failed to allocate memory)
        }
        CloseHandle(file_handle);
    }
    else{
        //TODO(oleksii): error handling (failed to open a file)
    }
    return(result);
}

function 
DEBUG_PLATFORM_WRITE_ENTIRE_FILE(win32_write_entire_file){
    B32 result = false;
    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(file_handle){
        DWORD bytes_written;
        if(WriteFile(file_handle, memory, size, &bytes_written, 0) &&
           (bytes_written == size)){
            result = true;
        }
        else{
            //TODO(oleksii): error handling (failed to write to a file)
        }
        CloseHandle(file_handle);
    }
    else{
        //TODO(oleksii): error handling (failed to open a file)
    }
    return(result);
}

function 
DEBUG_PLATFORM_FREE_FILE(win32_free_file){
    if(memory){
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

function void win32_switch_fullscreen(HWND window){
    //Raymond Chan's article: https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
    local_persist WINDOWPLACEMENT window_placement = {};
    window_placement.length = sizeof(window_placement);
    
    DWORD style = GetWindowLongA(window, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW){
        MONITORINFO monitor_info = {};
        monitor_info.cbSize = sizeof(monitor_info);
        if(GetWindowPlacement(window, &window_placement) && 
           GetMonitorInfoA(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info)){
            DWORD new_style = (style & ~WS_OVERLAPPEDWINDOW);
            SetWindowLongA(window, GWL_STYLE, new_style);
            SetWindowPos(window, HWND_TOP,
                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, 
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else{
        SetWindowLongA(window, GWL_STYLE, (style | WS_OVERLAPPEDWINDOW));
        SetWindowPlacement(window, &window_placement);
        SetWindowPos(window, 0, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

function V2 win32_get_window_client_size(HWND window){
    V2 result;
    RECT rect;
    GetClientRect(window, &rect);
    result.x = (rect.right - rect.left);
    result.y = (rect.bottom - rect.top);
    return(result);
}

function V2 win32_get_window_size(HWND window){
    V2 result;
    RECT rect;
    GetWindowRect(window, &rect);
    result.x = (rect.right - rect.left);
    result.y = (rect.bottom - rect.top);
    return(result);
}

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param){ 
    LRESULT result = 0;
    switch(message){
        case WM_CLOSE:
        case WM_DESTROY:{
            global_running = false;
        }break;
        case WM_SIZE:{
            V2 window_client_size = win32_get_window_client_size(window);
        }break;
        
        case WM_SYSKEYDOWN:{
            //bit 31, the value is always 0 for WM_SYSKEYDOWN
            U32 transition_state = !!(w_param << 31);
            char debug_buf[64];
            my_sprintf(debug_buf, size_of(debug_buf), "transition_state: %u\n", transition_state);
            OutputDebugStringA((LPCSTR)debug_buf);
        }break;
        case WM_SYSKEYUP:{
            //bit 31, the value is always 1 for WM_SYSKEYUP
        }break;
        case WM_KEYUP:{
        }break;
        case WM_KEYDOWN:{
        }break;
        
        //toggle fullscreen mode using left mouse button.
        case WM_LBUTTONDOWN:{
            win32_switch_fullscreen(window);
        }break;
        
        default:{
            result = DefWindowProc(window, message, w_param, l_param);
        }
    } 
    return(result); 
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_code){
    PlatformApi platform = {};
    platform.load_entire_file = win32_load_entire_file;
    platform.write_entire_file = win32_write_entire_file;
    platform.free_file = win32_free_file;
    platform.allocate_memory = win32_allocate_memory;
    platform.free_memory = win32_free_memory;
    
    win32_init_xinput();
    
    //TODO(alexey): We have to get the full path to the dll somehow
    char working_dir[1024] = {};
    U32 size = GetCurrentDirectory(sizeof(working_dir), working_dir);
    working_dir[size] = 0;
    Str8 app_dll = str8("\\app.dll");
    strncat(working_dir, app_dll.data, app_dll.size);
    working_dir[size + app_dll.size] = 0;
    OutputDebugStringA((LPCSTR)working_dir);
    if(PathFileExistsA((LPCSTR)working_dir) == TRUE){
        win32_load_app_code(&global_app_code, working_dir);
    }
    else{
        //TODO(alexey): Error handling, file doesn't exist.
    }
    
    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_main_window_procedure;
    window_class.hInstance = GetModuleHandle(0);
    window_class.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    window_class.lpszClassName = "WindowClass";
    if(RegisterClassA(&window_class)){
        RECT window_rect = {0, 0, 1080, 720};
        AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, 0);
        HWND window = CreateWindowExA(0, "WindowClass", "PlatformLayer",
                                      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                      window_rect.right - window_rect.left, 
                                      window_rect.bottom - window_rect.top,
                                      0, 0, GetModuleHandleA(0), 0);
        if(window){
            ShowWindow(window, SW_SHOW);
            UpdateWindow(window);
            HDC device_context = GetDC(window);
            if(win32_init_opengl(device_context)){
                Memory memory = {};
                memory.frame_storage_size = Gigabytes(2);
                memory.permanent_storage_size = Gigabytes(2);
                void *base_address = 0;
#if INTERNAL_BUILD
                base_address = (void *)Terabytes(8);
#endif
                memory.frame_storage = VirtualAlloc(base_address, 
                                                    (memory.frame_storage_size + memory.permanent_storage_size),
                                                    MEM_RESERVE|MEM_COMMIT,
                                                    PAGE_READWRITE);
                memory.permanent_storage = (void *)((char *)memory.frame_storage + memory.frame_storage_size);
                memory.platform = &platform;
                //NOTE(oleksii): Just assert for now to avoid deep nesting with if statements.
                assert(memory.frame_storage && memory.permanent_storage);
                
                //TODO(oleksii): Experiment with different inputs, event-driven (but without memory allocation)
                //just introduce and array of events probably [4096] events can be stored per frame.
                //Try to understand Casey's approach on how to write input systems. Choos which one is better.
                //But do it only after opengl clean up, so we can easily test the input.
                Input input = {};
                
                global_running = true;
                while(global_running){
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
                        if(XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS){
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
                            
                            if(a_button){
                                XINPUT_VIBRATION vibration;
                                vibration.wLeftMotorSpeed = 32768;
                                vibration.wRightMotorSpeed = 15600;
                                XInputSetState(controller_index, &vibration);
                            }
                        }
                        else{
                        }
                    }
                    global_app_code.update_and_render(&input, &memory);
                    SwapBuffers(device_context);
                }
                win32_delete_opengl_context(device_context, global_opengl_rendering_context);
            }
            else{
                //TODO(oleksii): error handling (failed to initialize OpenGL)
            }
            ReleaseDC(window, device_context);
        }
        else{
            //TODO(oleksii): error handling (failed to create a window)
        }
    }
    else{
        //TODO(oleksii): error handling (failed to register window class).
    }
    return(0);
}
