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
#include <gl/gl.h>
#include <shlwapi.h>

#define debug_break() __debugbreak();

#define XINPUT_LEFT_STICK_DEADZONE  7849
#define XINPUT_RIGHT_STICK_DEADZONE 8689
#define XINPUT_TRIGGER_THRESHOLD 30

#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dw_user_index, XINPUT_STATE *p_state)
typedef DWORD (WINAPI *XInputGetStatePtr)(DWORD dw_user_index, XINPUT_STATE *p_state);
XINPUT_GET_STATE(xinput_get_state_stub){ return(ERROR_DEVICE_NOT_CONNECTED); }
global XInputGetStatePtr xinput_get_state_ptr = xinput_get_state_stub;
#define XInputGetState xinput_get_state_ptr

#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dw_user_index, XINPUT_VIBRATION *p_vibration)
typedef DWORD (WINAPI *XInputSetStatePtr)(DWORD dw_user_index, XINPUT_VIBRATION *p_vibration);
XINPUT_SET_STATE(xinput_set_state_stub){ return(ERROR_DEVICE_NOT_CONNECTED); }
global XInputSetStatePtr xinput_set_state_ptr = xinput_set_state_stub;
#define XInputSetState xinput_set_state_ptr

struct Win32State{
    //Just a placeholder for all the global variables.(device_context, global_running ...)
};

struct Win32AppCode{
    HMODULE dll;
    AppUpdateAndRenderPtr update_and_render;
    B32 valid;
};

#define WIN32_H
#endif //WIN32_H
