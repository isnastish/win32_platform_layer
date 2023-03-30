#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

struct OpenglInfo{
    char *vendor;
    char *renderer;
    char *version;
    char *shading_language_version;
    char *extensions;
};

function OpenglInfo gl_get_info(){
    OpenglInfo info = {};
    info.vendor = (char *)glGetString(GL_VENDOR);
    info.renderer = (char *)glGetString(GL_RENDERER);
    info.version = (char *)glGetString(GL_VERSION);
    info.shading_language_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    info.extensions = (char *)glGetString(GL_EXTENSIONS);
    return(info);
}