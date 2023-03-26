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

typedef BOOL (WINAPI *WglChoosePixelFormatARBPtr)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI *WglCreateContextAttribsARBPtr)(HDC hDC, HGLRC hShareContext, const int *attribList);

global WglChoosePixelFormatARBPtr wglChoosePixelFormatARB;
global WglCreateContextAttribsARBPtr wglCreateContextAttribsARB;

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_TYPE_RGBA_ARB                 0x202B

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C

struct Win32State{
    //Just a placeholder for all the global variables.(device_context, global_running ...)
};

struct Win32AppCode{
    HMODULE dll;
    AppUpdateAndRenderPtr update_and_render;
    B32 loaded;
};

#define WIN32_H
#endif //WIN32_H
