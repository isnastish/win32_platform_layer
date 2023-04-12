#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

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
            string_copy(ext, start, ext_len);
            if(strings_equal(ext_len, ext, 
                             result.gl_arb_framebuffer_srgb.name.size, 
                             result.gl_arb_framebuffer_srgb.name.data)){
                result.gl_arb_framebuffer_srgb.supported = true;
            }
            else if(strings_equal(ext_len, ext,
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