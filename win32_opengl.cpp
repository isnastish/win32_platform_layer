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

typedef BOOL (WINAPI *WglChoosePixelFormatARBPtr)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI *WglCreateContextAttribsARBPtr)(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI *WglSwapIntervalETRPtr)(int interval);

global WglChoosePixelFormatARBPtr wglChoosePixelFormatARB;
global WglCreateContextAttribsARBPtr wglCreateContextAttribsARB;
global WglSwapIntervalETRPtr wglSwapIntervalEXT;

global I32 
global_choose_pixel_format_attrib_list[]={
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0,
};

global I32 
global_create_context_attrib_list[]={
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0,
};
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
    if(dummy_opengl_rendering_context){
        wglMakeCurrent(device_context, dummy_opengl_rendering_context);
        win32_load_wgl_procedures();
        wglSwapIntervalEXT(1);
        OpenglInfo opengl_info = opengl_get_info();
#if INTERNAL_BUILD
        {
            U8 debug_buf[1 << 14];
            U8 *fmt =
            (U8 *)"OpenGL Info\n"
                "vendor: %s\n"
                "renderer: %s\n"
                "version: %s\n"
                "glsl version: %s\n"
                "extensions: %s\n";
            sprintf_s_u8(debug_buf, sizeof(debug_buf), fmt, 
                         opengl_info.vendor,
                         opengl_info.renderer,
                         opengl_info.version,
                         opengl_info.shading_language_version,
                         opengl_info.extensions);
            OutputDebugStringA((LPCSTR)debug_buf);
        }
#endif
        if(opengl_info.gl_arb_framebuffer_srgb){
            glEnable(GL_FRAMEBUFFER_SRGB);
        }
        if(opengl_info.gl_ext_texture_srgb_decode){
            glEnable(GL_SRGB8_ALPHA8);
        }
        
        I32 pixel_format;
        U32 formats_count;
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
