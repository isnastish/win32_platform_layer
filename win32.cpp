#include "windows_slim.h"

// gl
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

// wgl
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

struct OpenglExtension{
    B32 supported;
    Str8 name;
};

struct OpenglInfo{
    char *vendor;
    char *renderer;
    char *version;
    char *shading_language_version;
    char *extensions;
    
    OpenglExtension gl_arb_framebuffer_srgb;
    OpenglExtension gl_ext_texture_srgb_decode;
};

typedef BOOL (WINAPI *WglChoosePixelFormatARBPtr)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI *WglCreateContextAttribsARBPtr)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI *WglSwapIntervalETRPtr)(int interval);

global WglChoosePixelFormatARBPtr wglChoosePixelFormatARB;
global WglCreateContextAttribsARBPtr wglCreateContextAttribsARB;
global WglSwapIntervalETRPtr wglSwapIntervalEXT;
global I32 global_choose_pixel_format_attrib_list[]={
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0,
};
global I32 global_create_context_attrib_list[]={
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0,
};
global HGLRC global_opengl_rendering_context;
global B32 global_running;
global Win32AppCode global_app_code;

function OpenglInfo opengl_get_info(){
    OpenglInfo result = {};
    result.vendor = (char *)glGetString(GL_VENDOR);
    result.renderer = (char *)glGetString(GL_RENDERER);
    result.version = (char *)glGetString(GL_VERSION);
    result.shading_language_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    result.extensions = (char *)glGetString(GL_EXTENSIONS);
    result.gl_arb_framebuffer_srgb.name = str8("GL_ARB_framebuffer_sRGB");
    result.gl_ext_texture_srgb_decode.name = str8("GL_EXT_texture_sRGB_decode");
    char *start, *end;
    start = end = result.extensions;
    for(I32 index = 0;
        result.extensions[index];
        ++index, ++end){
        if(is_space(result.extensions[index])){
            U32 ext_len = (end - start);
            char ext[256];
            assert(ext_len <= size_of(ext));
            str_copy(ext, start, ext_len);
            if(str_equal(ext_len, ext, 
                         result.gl_arb_framebuffer_srgb.name.size, 
                         result.gl_arb_framebuffer_srgb.name.data)){
                result.gl_arb_framebuffer_srgb.supported = true;
            }
            else if(str_equal(ext_len, ext,
                              result.gl_ext_texture_srgb_decode.name.size, 
                              result.gl_ext_texture_srgb_decode.name.data)){
                result.gl_ext_texture_srgb_decode.supported = true;
            }
            start = (end + 1);
        }
    }
    return(result);
}

function void opengl_init(){
    OpenglInfo opengl_info = opengl_get_info();
#if INTERNAL_BUILD
    {
        char debug_buf[1 << 14];
        char *fmt =
            "OpenGL Info\n"
            "vendor: %s\n"
            "renderer: %s\n"
            "version: %s\n"
            "glsl version: %s\n"
            "extensions: %s\n";
        my_sprintf(debug_buf,
                   size_of(debug_buf),
                   fmt,
                   opengl_info.vendor,
                   opengl_info.renderer,
                   opengl_info.version,
                   opengl_info.shading_language_version,
                   opengl_info.extensions);
        OutputDebugStringA((LPCSTR)debug_buf);
    }
#endif
    if(opengl_info.gl_arb_framebuffer_srgb.supported){
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    if(opengl_info.gl_ext_texture_srgb_decode.supported){
        glEnable(GL_SRGB8_ALPHA8);
    }
}

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

function void win32_load_wgl_procedures(){
    wglChoosePixelFormatARB = (WglChoosePixelFormatARBPtr)win32_load_opengl_procedure("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (WglCreateContextAttribsARBPtr)win32_load_opengl_procedure("wglCreateContextAttribsARB");
    wglSwapIntervalEXT = (WglSwapIntervalETRPtr)win32_load_opengl_procedure("wglSwapIntervalEXT");
#if INTERNAL_BUILD
    assert(wglChoosePixelFormatARB &&
           wglCreateContextAttribsARB &&
           wglSwapIntervalEXT);
#endif
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
    SetPixelFormat(device_context, pixel_format_index, &suggested_pfd);
    HGLRC dummy_opengl_rendering_context = wglCreateContext(device_context);
    
    I32 pixel_format;
    U32 formats_count;
    if(dummy_opengl_rendering_context){
        wglMakeCurrent(device_context, dummy_opengl_rendering_context);
        win32_load_wgl_procedures();
        wglSwapIntervalEXT(1);
        opengl_init();
        wglChoosePixelFormatARB(device_context, global_choose_pixel_format_attrib_list, 0, 1, &pixel_format, &formats_count);
        global_opengl_rendering_context = wglCreateContextAttribsARB(device_context, dummy_opengl_rendering_context,
                                                                     global_create_context_attrib_list);
        win32_delete_opengl_context(device_context, dummy_opengl_rendering_context);
        wglMakeCurrent(device_context, global_opengl_rendering_context);
        result = true;
    }
    else{
        //TODO(oleksii): error handling (failed to create dummy opengl rendering context).
    }
    return(result);
}

function void win32_get_file_write_time(char *file_name){
    /* 
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
     */
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

function DEBUG_PLATFORM_LOAD_ENTIRE_FILE(win32_load_entire_file){
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

function DEBUG_PLATFORM_WRITE_ENTIRE_FILE(win32_write_entire_file){
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

function DEBUG_PLATFORM_FREE_FILE(win32_free_file){
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

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam){ 
    LRESULT result = 0;
    switch(message){
        case WM_CLOSE:
        case WM_DESTROY:{
            global_running = false;
        }break;
        case WM_SIZE:{
            V2 window_client_size = win32_get_window_client_size(window);
        }break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_KEYDOWN:{
            B32 was_down = ((lparam & (1 << 30)) != 0);
            B32 is_down = ((lparam & (1 << 31)) == 0);
            U32 vk_code = wparam;
            /*             
                        if(was_down != is_down){
                            char buf[256];
                            if(vk_code == VK_SPACE){
                                my_sprintf(buf, size_of(buf), "SPACE was_down: %i\nis_down: %u\nSpace down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'A'){
                                my_sprintf(buf, size_of(buf), "A was_down: %i\nis_down: %u\n'A' down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'W'){
                                my_sprintf(buf, size_of(buf), "W was_down: %i\nis_down: %u\n'W' down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'S'){
                                my_sprintf(buf, size_of(buf), "S was_down: %i\nis_down: %u\n'S' down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'D'){
                                my_sprintf(buf, size_of(buf), "D was_down: %i\nis_down: %u\n'D' down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'Q'){
                                my_sprintf(buf, size_of(buf), "Q was_down: %i\nis_down: %u\n'Q' down\n\n", was_down, is_down);
                            }
                            else if(vk_code == 'E'){
                                my_sprintf(buf, size_of(buf), "E was_down: %i\nis_down: %u\n'E' down\n\n", was_down, is_down);
                            }
                            debug_out(buf);
                        }
                         */
        }break;
        case WM_LBUTTONDOWN:{
            win32_switch_fullscreen(window);
        }break;
        
        default:{
            result = DefWindowProc(window, message, wparam, lparam);
        }
    }
    return(result); 
}

function void win32_process_gamepad_input(Input *old_input, Input *new_input){
    auto process_stick_input = [](int32_t stick_x, int32_t stick_y, int32_t dead_zone) -> V2{
        V2 result = {};
        if(stick_x > dead_zone || stick_x < dead_zone){
            result.x = (stick_x < 0 ? ((float)stick_x / 32768.0f) : ((float)stick_x / 32767.0f));
        }
        if(stick_y > dead_zone || stick_y < -dead_zone){
            result.y = (stick_y < 0 ? ((float)stick_y / 32768.0f) : ((float)stick_y / 32767.0f));
        }
        return(result);
    };
    
    int32_t max_controller_count = XUSER_MAX_COUNT;
    if(max_controller_count > size_of(new_input->controllers)){
        max_controller_count = size_of(new_input->controllers);
    }
    for(int32_t controller_index = 0; controller_index < max_controller_count; controller_index += 1){
        XINPUT_STATE controller_state = {};
        if(XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS){
            XINPUT_GAMEPAD *gamepad = &controller_state.Gamepad;
            V2 trigger = v2(((gamepad->bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? 0 : gamepad->bLeftTrigger), 
                            ((gamepad->bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? 0 : gamepad->bRightTrigger));
            
            V2 left_stick = process_stick_input(gamepad->sThumbLX, gamepad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
            V2 right_stick = process_stick_input(gamepad->sThumbRX, gamepad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
            
            B32 dpad_up = !!(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
            B32 dpad_down = !!(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            B32 dpad_left = !!(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            B32 dpad_right = !!(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
            
            B32 a_button = !!(gamepad->wButtons & XINPUT_GAMEPAD_A);
            B32 b_button = !!(gamepad->wButtons & XINPUT_GAMEPAD_B);
            B32 x_button = !!(gamepad->wButtons & XINPUT_GAMEPAD_X);
            B32 y_button = !!(gamepad->wButtons & XINPUT_GAMEPAD_Y);
            
            B32 left_shoulder = !!(gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            B32 right_shoulder = !!(gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            B32 start = !!(gamepad->wButtons & XINPUT_GAMEPAD_START);
            B32 back = !!(gamepad->wButtons & XINPUT_GAMEPAD_BACK);
        }
        else{
        }
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_code){
    // NOTE(alx): The only reason we don't collapse this code into a function is because this is the only place 
    // where we compute the path to the dll.
    char exe_path[MAX_PATH];
    GetModuleFileNameA(0, exe_path, sizeof(exe_path));
    assert(GetLastError() != ERROR_INSUFFICIENT_BUFFER);
    char *parent_exe_path = exe_path;
    for(char *at = exe_path; *at; at += 1){
        if(*at == '\\'){
            parent_exe_path = (at + 1);
        }
    }
    char *source_dll_name = "app.dll";
    char source_dll_full_path[MAX_PATH];
    cat_strings((parent_exe_path - exe_path), exe_path,
                array_count(source_dll_name) - 1, source_dll_name,
                array_count(source_dll_full_path), source_dll_full_path);
    assert(PathFileExistsA(source_dll_full_path) == TRUE);
    win32_load_app_code(&global_app_code, source_dll_full_path);
    
    CoInitialize(0);
    PlatformApi platform = {};
    platform.load_entire_file = win32_load_entire_file;
    platform.write_entire_file = win32_write_entire_file;
    platform.free_file = win32_free_file;
    platform.allocate_memory = win32_allocate_memory;
    platform.free_memory = win32_free_memory;
    
    win32_init_xinput();
    
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
                assert(memory.frame_storage && memory.permanent_storage);
                
                Input inputs[2] = {};
                Input *new_input = &inputs[0];
                Input *old_input = &inputs[1];
                
                global_running = true;
                while(global_running){
                    MSG msg;
                    while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)){
                        TranslateMessage(&msg);
                        DispatchMessageA(&msg);
                    }
                    
                    win32_process_gamepad_input(old_input, new_input);
                    
                    global_app_code.update_and_render(new_input, &memory);
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
    CoUninitialize();
    
    return(0);
}
