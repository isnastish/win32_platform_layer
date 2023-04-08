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

//OpenGL extension names to query for:
global String8 opengl_arb_framebuffer_srgb_extension = Str8("GL_ARB_framebuffer_sRGB");
global String8 opengl_ext_texture_srgb_decode_extension = Str8("GL_EXT_texture_sRGB_decode");
global String8 opengl_arb_vertex_attrib_binding_extension = Str8("GL_ARB_vertex_attrib_binding");
global String8 opengl_arb_vertex_buffer_object_extension = Str8("GL_ARB_vertex_buffer_object");
global String8 opengl_arb_vertex_program_extension = Str8("GL_ARB_vertex_program");
global String8 opengl_arb_vertex_shader_extension = Str8("GL_ARB_vertex_shader");

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
#if INTERNAL_BUILD
            {
                U8 debug_buf[512];
                sprintf_s_u8(debug_buf, sizeof(debug_buf), "Ext: %s\n", ext_buf);
                OutputDebugStringA((LPCSTR)debug_buf);
            }
#endif
            if(!strncmp_u8(ext_buf, 
                           opengl_arb_framebuffer_srgb_extension.data,
                           opengl_arb_framebuffer_srgb_extension.size)){
                result.gl_arb_framebuffer_srgb = true;
            }
            else if(!strncmp_u8(ext_buf, 
                                opengl_ext_texture_srgb_decode_extension.data, 
                                opengl_ext_texture_srgb_decode_extension.size)){
                result.gl_ext_texture_srgb_decode = true;
            }
            else if(!strncmp_u8(ext_buf, 
                                opengl_arb_vertex_attrib_binding_extension.data, 
                                opengl_arb_vertex_attrib_binding_extension.size)){
                result.gl_arb_vertex_attrib_binding = true;
            }
            else if(!strncmp_u8(ext_buf, 
                                opengl_arb_vertex_buffer_object_extension.data, 
                                opengl_arb_vertex_buffer_object_extension.size)){
                result.gl_arb_vertex_buffer_object = true;
            }
            else if(!strncmp_u8(ext_buf, 
                                opengl_arb_vertex_program_extension.data, 
                                opengl_arb_vertex_program_extension.size)){
                result.gl_arb_vertex_program = true;
            }
            else if(!strncmp_u8(ext_buf, 
                                opengl_arb_vertex_shader_extension.data, 
                                opengl_arb_vertex_shader_extension.size)){
                result.gl_arb_vertex_shader = true;
            }
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