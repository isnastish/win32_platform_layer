#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

struct OpenglExtensions{
    B32 gl_arb_framebuffer_srgb;
    B32 gl_ext_texture_srgb_decode;
    B32 gl_arb_vertex_attrib_binding;
    B32 gl_arb_vertex_buffer_object;
    B32 gl_arb_vertex_program;
    B32 gl_arb_vertex_shader;
};

struct OpenglInfo{
    U8 *vendor;
    U8 *renderer;
    U8 *version;
    U8 *shading_language_version;
    U8 *extensions;
};

function OpenglExtensions opengl_parse_extensions(U8 *extensions_string){
    OpenglExtensions result = {};
    U8 *start, *end;
    start = end = extensions_string;
    for(I32 index = 0;
        *(extensions_string + index) != '\0';
        ++index, ++end){
        if(is_space(*(extensions_string + index))){
            U32 ext_length = (end - start);
            U8 *ext = (U8 *)malloc(ext_length + 1);
            memcpy(ext, start, ext_length);
            ext[ext_length] = 0;
            start = (end + 1);
#if INTERNAL_BUILD
            OutputDebugStringA((LPCSTR)ext);
            OutputDebugStringA((LPCSTR)"\n");
#endif
            if(strncmp((char *)ext, "GL_ARB_framebuffer_sRGB", strlen("GL_ARB_framebuffer_sRGB")) == 0){
                result.gl_arb_framebuffer_srgb = true;
            }
            if(strncmp((char *)ext, "GL_EXT_texture_sRGB_decode", strlen("GL_EXT_texture_sRGB_decode")) == 0){
                result.gl_ext_texture_srgb_decode = true;
            }
#if 0
            B32 gl_arb_vertex_attrib_binding;
            B32 gl_arb_vertex_buffer_object;
            B32 gl_arb_vertex_program;
            B32 gl_arb_vertex_shader;
#endif
        }
    }
    return(result);
}

function OpenglInfo opengl_get_info(){
    OpenglInfo info = {};
    info.vendor = (U8 *)glGetString(GL_VENDOR);
    info.renderer = (U8 *)glGetString(GL_RENDERER);
    info.version = (U8 *)glGetString(GL_VERSION);
    info.shading_language_version = (U8 *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    info.extensions = (U8 *)glGetString(GL_EXTENSIONS);
    return(info);
}