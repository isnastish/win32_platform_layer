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

#define debug_break() __debugbreak();

#define GET_STOCK_OBJECT(name) HGDIOBJ name(int i)
typedef HGDIOBJ (*GetStockObjectPtr)(int i);
GET_STOCK_OBJECT(get_stock_object_stub){ return(0); }

struct Win32{
    HMODULE gdi_dll;
    GetStockObjectPtr get_stock_object;
    B32 running;
    
    HWND window;
    I32 window_width; //width of the client area
    I32 window_height; //height of the client area
};

LRESULT CALLBACK win32_main_window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
function void win32_init_gdi(Win32 *win32_state);
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
function FileLoadResult win32_load_entire_file(char *file_path);
function void win32_write_to_file(void *memory);

#define WIN32_H
#endif //WIN32_H
