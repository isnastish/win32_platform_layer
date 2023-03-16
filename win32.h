#ifndef WIN32_H

#include "platform.h"
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS //     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // - VK_*
#define NOSYSMETRICS //      - SM_*
#define NOMENUS //           - MF_*
#define NOICONS //           - IDI_*
#define NOKEYSTATES //       - MK_*
#define NOSYSCOMMANDS //     - SC_*
#define NORASTEROPS //       - Binary and Tertiary raster ops
#define OEMRESOURCE //       - OEM Resource values
#define NOATOM //            - Atom Manager routines
#define NOCLIPBOARD //       - Clipboard routines
#define NOCOLOR //           - Screen colors
#define NOCTLMGR //          - Control and Dialog routines
#define NODRAWTEXT //        - DrawText() and DT_*
#define NOKERNEL //          - All KERNEL defines and routines
#define NONLS //             - All NLS defines and routines
#define NOMB //              - MB_* and MessageBox()
#define NOMEMMGR //          - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE //        - typedef METAFILEPICT
#define NOMINMAX //          - Macros min(a,b) and max(a,b)
#define NOSCROLL //          - SB_* and scrolling routines
#define NOSERVICE //         - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND //           - Sound driver routines
#define NOTEXTMETRIC //      - typedef TEXTMETRIC and associated routines
#define NOWH //              - SetWindowsHook and WH_*
#define NOCOMM //            - COMM driver routines
#define NOKANJI //           - Kanji support stuff.
#define NOHELP //            - Help engine interface.
#define NOPROFILER //        - Profiler interface.
#define NODEFERWINDOWPOS //  - DeferWindowPos routines
#define NOMCX //             - Modem Configuration Extensions
#include <windows.h>
#include <xinput.h>

#define debug_break() __debugbreak();

#define GET_STOCK_OBJECT(name) HGDIOBJ name(int i)
typedef HGDIOBJ (*GetStockObjectPtr)(int i);
GET_STOCK_OBJECT(get_stock_object_stub){ return(0); }

#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dw_user_index, XINPUT_STATE *p_state)
#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dw_user_index, XINPUT_VIBRATION *p_vibration)
typedef DWORD (WINAPI *XinputGetStatePtr)(DWORD dw_user_index, XINPUT_STATE *p_state);
typedef DWORD (WINAPI *XinputSetStatePtr)(DWORD dw_user_index, XINPUT_VIBRATION *p_vibration);

XINPUT_GET_STATE(xinput_get_state_stub){
    return(ERROR_DEVICE_NOT_CONNECTED);
}

XINPUT_SET_STATE(xinput_set_state_stub){
    return(ERROR_DEVICE_NOT_CONNECTED);
}

struct Win32{
    GetStockObjectPtr get_stock_object;
    XinputGetStatePtr xinput_get_state;
    XinputSetStatePtr xinput_set_state;
    
    B32 running;
    HWND window;
    I32 window_width; //width of the client area
    I32 window_height; //height of the client area
    
    HMODULE               app_dll;
    AppUpdateAndRenderPtr app_update_and_render;
    B32                   app_is_loaded;
};

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
function void win32_init_gdi(Win32 *win32);
function void win32_init_xinput(Win32 *win32);
function void win32_load_app_code(Win32 *win32, char *dll_path);
function void win32_unload_app_code(Win32 *win32);
function void win32_register_window_class(Win32 *win32);
function void win32_create_window(Win32 *win32, I32 width, I32 height);
function void win32_show_window(Win32 *win32);
function V2 win32_get_window_metrics(Win32 *win32);
function void win32_switch_fullscreen(Win32 *win32);

//NOTE(oleksii): This struct is not platform specific and should be moved out to a different file.
//Will have to think about it more when we load our application as a .dll
struct FileLoadResult{
    I64 size;
    I64 compressed_size;
    void *data;
};

//NOTE(oleksii): Think more about where to put these functions, and about naming as well.
//Maybe they should live in platform.h file???
//And replace char * with String8 by adding it to basic_types.h file first.
function FileLoadResult win32_load_entire_file_into_memory(char *file_name);
function B32 win32_write_memory_to_file(char *file_name, I64 size, void *memory);
function void win32_free_file_memory(void *memory);

#define WIN32_H
#endif //WIN32_H
