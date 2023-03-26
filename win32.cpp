//TODO:
//
//[x] Refactor the codebase, make everything explicit!
//[ ] Init OpenGL, load opengl procedures
//[ ] Finish todos for error handling
//[ ] Implement our own sprintf function with %v2, %v3, %v4, %m2, %m3, %m4 formats (and all the standart formats as well)

#include "win32.h"

////////////////////////////////
//NOTE(oleksii): Global variables
global B32 global_running;
global Win32AppCode global_app_code;
global HGLRC global_opengl_rendering_context;

function void *win32_load_opengl_procedure(char *proc_name){
    void *proc = (void *)wglGetProcAddress(proc_name);
    if(proc == 0 ||
       (proc == (void*)0x1) || (proc == (void*)0x2) || (proc == (void*)0x3) ||
       (proc == (void*)-1)){
        HMODULE opengl_library = LoadLibraryA("opengl32.dll");
        proc = (void *)GetProcAddress(opengl_library, proc_name);
    }
    return(proc);
}

function void win32_delete_opengl_context(HDC device_context, HGLRC rendering_context){
    wglMakeCurrent(device_context, 0);
    wglDeleteContext(rendering_context);
}

function B32 win32_init_opengl(HDC device_context){
    B32 result = false;
    PIXELFORMATDESCRIPTOR desired_pfd = {};
    desired_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    desired_pfd.nVersion = 1;
    desired_pfd.dwFlags = (PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER);
    desired_pfd.iPixelType = PFD_TYPE_RGBA;
    desired_pfd.cColorBits = 32;
    desired_pfd.cDepthBits = 24;
    desired_pfd.cStencilBits = 8;
    desired_pfd.iLayerType = PFD_MAIN_PLANE;
    
    I32 pixel_format_index = ChoosePixelFormat(device_context, &desired_pfd);
    PIXELFORMATDESCRIPTOR suggested_pfd = {};
    DescribePixelFormat(device_context, pixel_format_index,
                        sizeof(PIXELFORMATDESCRIPTOR), &suggested_pfd);
    if(SetPixelFormat(device_context, pixel_format_index, &suggested_pfd) == TRUE){
        HGLRC dummy_opengl_rendering_context = wglCreateContext(device_context);
        wglMakeCurrent(device_context, dummy_opengl_rendering_context);
        wglChoosePixelFormatARB = (WglChoosePixelFormatARBPtr)win32_load_opengl_procedure("wglChoosePixelFormatARB");
        const I32 attrib_list[]={
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0,
        };
        I32 pixel_format;
        U32 formats_count;
        if(wglChoosePixelFormatARB(device_context, attrib_list, 0, 1, &pixel_format, &formats_count) != FALSE){
            wglCreateContextAttribsARB = (WglCreateContextAttribsARBPtr)win32_load_opengl_procedure("wglCreateContextAttribsARB");
            const I32 create_context_attrib_list[]={
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                0,
            };
            global_opengl_rendering_context = wglCreateContextAttribsARB(device_context, dummy_opengl_rendering_context, 
                                                                         create_context_attrib_list);
            win32_delete_opengl_context(device_context, dummy_opengl_rendering_context);
            wglMakeCurrent(device_context, global_opengl_rendering_context);
            result = true;
        }
        else{
            //TODO(oleksii): error handling (failed to choose pixel format ARB)
        }
    }
    else{
        //TODO(oleksii): error handling (failed to set pixel format for device context)
    }
    return(result);
}

function void win32_load_app_code(Win32AppCode *app_code, char *dll_name){
    app_code->dll = LoadLibraryA(dll_name);
    if(app_code->dll){
        app_code->update_and_render = 
            (AppUpdateAndRenderPtr)GetProcAddress(app_code->dll, "app_update_and_render");
        if(!app_code->update_and_render){
            app_code->update_and_render = app_update_and_render_stub;
            app_code->loaded = true;
        }
    }
}

function void win32_unload_app_code(Win32AppCode *app_code){
    if(app_code->dll){
        FreeLibrary(app_code->dll);
        app_code->loaded = false;
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

function FileLoadResult win32_load_entire_file(char *file_name){
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

function B32 win32_write_entire_file(char *file_name, I64 size, void *memory){
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

function void win32_free_file(void *memory){
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
    Platform platform = {};
    platform.load_entire_file = win32_load_entire_file;
    platform.write_entire_file = win32_write_entire_file;
    platform.free_file = win32_free_file;
    
    win32_init_xinput();
    win32_load_app_code(&global_app_code, "e:/work/build/application.dll");
    
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
                    glClearColor(157.0f/255.0f, 130.0f/255.0f, 97.0f/255.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    global_app_code.update_and_render(&platform);
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
