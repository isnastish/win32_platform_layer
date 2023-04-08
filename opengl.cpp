#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

struct OpenglInfo{
    U8 *vendor;
    U8 *renderer;
    U8 *version;
    U8 *shading_language_version;
    U8 *extensions;
    
    B32 gl_arb_framebuffer_srgb;
    B32 gl_ext_texture_srgb_decode;
    B32 gl_arb_vertex_attrib_binding;
    B32 gl_arb_vertex_buffer_object;
    B32 gl_arb_vertex_program;
    B32 gl_arb_vertex_shader;
};

//OpenGL extension names to query for:
global String8 opengl_arb_framebuffer_srgb_extension = Str8("GL_ARB_framebuffer_sRGB");
global String8 opengl_ext_texture_srgb_decode_extension = Str8("GL_EXT_texture_sRGB_decode");
global String8 opengl_arb_vertex_attrib_binding_extension = Str8("GL_ARB_vertex_attrib_binding");
global String8 opengl_arb_vertex_buffer_object_extension = Str8("GL_ARB_vertex_buffer_object");
global String8 opengl_arb_vertex_program_extension = Str8("GL_ARB_vertex_program");
global String8 opengl_arb_vertex_shader_extension = Str8("GL_ARB_vertex_shader");

function OpenglInfo opengl_get_info(){
    OpenglInfo result = {};
    //TODO(alexey): Think about how to avoid casting?
    result.vendor = (U8 *)glGetString(GL_VENDOR);
    result.renderer = (U8 *)glGetString(GL_RENDERER);
    result.version = (U8 *)glGetString(GL_VERSION);
    result.shading_language_version = (U8 *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    result.extensions = (U8 *)glGetString(GL_EXTENSIONS);
    U8 *start, *end;
    start = end = result.extensions;
    for(I32 index = 0;
        result.extensions[index];
        ++index, ++end){
        if(is_space(result.extensions[index])){
            U32 ext_len = end - start;
            U8 ext_buf[256];
            assert(ext_len <= sizeof(ext_buf));
#if 0
            strncpy_u8(ext_buf, start, ext_len);
#else
            //TODO(alexey): Not tested yet!
            string_copy(ext_buf, start, ext_len);
#endif
            if(strings_equal(ext_len, ext_buf, opengl_arb_framebuffer_srgb_extension.size, opengl_arb_framebuffer_srgb_extension.data_u8)){
                result.gl_arb_framebuffer_srgb = true;
            }
            else if(strings_equal(ext_len, ext_buf, opengl_ext_texture_srgb_decode_extension.size, opengl_ext_texture_srgb_decode_extension.data_u8)){
                result.gl_ext_texture_srgb_decode = true;
            }
            else if(strings_equal(ext_len, ext_buf, opengl_arb_vertex_attrib_binding_extension.size, opengl_arb_vertex_attrib_binding_extension.data_u8)){
                result.gl_arb_vertex_attrib_binding = true;
            }
            else if(strings_equal(ext_len, ext_buf, opengl_arb_vertex_buffer_object_extension.size, opengl_arb_vertex_buffer_object_extension.data_u8)){
                result.gl_arb_vertex_buffer_object = true;
            }
            else if(strings_equal(ext_len, ext_buf, opengl_arb_vertex_program_extension.size, opengl_arb_vertex_program_extension.data_u8)){
                result.gl_arb_vertex_program = true;
            }
            else if(strings_equal(ext_len, ext_buf, opengl_arb_vertex_shader_extension.size, opengl_arb_vertex_shader_extension.data_u8)){
                result.gl_arb_vertex_shader = true;
            }
            start = (end + 1);
        }
    }
    return(result);
}