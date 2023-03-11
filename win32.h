#ifndef WIN32_H

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS //     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // - VK_*
#define NOWINMESSAGES //     - WM_*, EM_*, LB_*, CB_*
#define NOSYSMETRICS //      - SM_*
#define NOMENUS //           - MF_*
#define NOICONS //           - IDI_*
#define NOKEYSTATES //       - MK_*
#define NOSYSCOMMANDS //     - SC_*
#define NORASTEROPS //       - Binary and Tertiary raster ops
#define NOSHOWWINDOW //      - SW_*
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
#define NOMSG //             - typedef MSG and associated routines
#define NOSCROLL //          - SB_* and scrolling routines
#define NOSERVICE //         - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND //           - Sound driver routines
#define NOTEXTMETRIC //      - typedef TEXTMETRIC and associated routines
#define NOWH //              - SetWindowsHook and WH_*
#define NOWINOFFSETS //      - GWL_*, GCL_*, associated routines
#define NOCOMM //            - COMM driver routines
#define NOKANJI //           - Kanji support stuff.
#define NOHELP //            - Help engine interface.
#define NOPROFILER //        - Profiler interface.
#define NODEFERWINDOWPOS //  - DeferWindowPos routines
#define NOMCX //             - Modem Configuration Extensions
#include <windows.h>

#define GET_STOCK_OBJECT(name) HGDIOBJ name(int i)
typedef HGDIOBJ (*GetStockObjectPtr)(int i);
GET_STOCK_OBJECT(get_stock_object_stub){ return(0); }

struct Win32{
    HMODULE gdi_dll;
    GetStockObjectPtr get_stock_object;
};

function void win32_init_gdi(Win32 *win32_state);

#define WIN32_H
#endif //WIN32_H
