#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

struct OpenglInfo{
    U8 *vendor;
    U8 *renderer;
    U8 *version;
    U8 *shading_language_version;
    U8 *extensions;
};

function OpenglInfo gl_get_info(){
    OpenglInfo info = {};
    info.vendor = (U8 *)glGetString(GL_VENDOR);
    info.renderer = (U8 *)glGetString(GL_RENDERER);
    info.version = (U8 *)glGetString(GL_VERSION);
    info.shading_language_version = (U8 *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    info.extensions = (U8 *)glGetString(GL_EXTENSIONS);
    return(info);
}