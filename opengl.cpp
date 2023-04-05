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

global String8 opengl_extension_names[]={
    Str8("GL_ARB_framebuffer_sRGB"),
};

//NOTE(alexey): Only works when extensions are separated with a single space.
//Probably I have to implement a more robust parser.
function OpenglExtensions opengl_get_extensions(U8 *extensions){
    OpenglExtensions result = {};
    U8 *start, *end;
    start = end = extensions;
    for(I32 index = 0;
        extensions[index];
        ++index, ++end){
        if(is_space(extensions[index])){
            U32 ext_len = end - start;
            U8 ext_buf[256];
            assert(ext_len <= sizeof(ext_buf));
            strncpy_u8(ext_buf, start, ext_len);
            ext_buf[ext_len] = 0;
            //TODO(alexey): Collapse it into the function
            if(!strncmp_u8(ext_buf, "GL_ARB_framebuffer_sRGB", sizeof("GL_ARB_framebuffer_sRGB"))){
                result.gl_arb_framebuffer_srgb = true;
            }
            if(!strncmp_u8(ext_buf, "GL_EXT_texture_sRGB_decode", sizeof("GL_EXT_texture_sRGB_decode"))){
                result.gl_ext_texture_srgb_decode = true;
            }
            if(!strncmp_u8(ext_buf, "GL_ARB_vertex_attrib_binding", sizeof("GL_ARB_vertex_attrib_binding"))){
                result.gl_arb_vertex_attrib_binding = true;
            }
            if(!strncmp_u8(ext_buf, "GL_ARB_vertex_buffer_object", sizeof("gl_arb_vertex_buffer_object"))){
                result.gl_arb_vertex_buffer_object = true;
            }
            if(!strncmp_u8(ext_buf, "GL_ARB_vertex_program", sizeof("GL_ARB_vertex_program"))){
                result.gl_arb_vertex_program = true;
            }
            if(!strncmp_u8(ext_buf, "GL_ARB_vertex_shader", sizeof("GL_ARB_vertex_shader"))){
                result.gl_arb_vertex_shader = true;
            }
            
            //skip the space
            start = (end + 1);
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