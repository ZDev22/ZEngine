/*
*
*	RGFW 1.7
*
* Copyright (C) 2022-25 ColleagueRiley
*
* libpng license
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.

* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*
*/

//MODIFIED
#pragma once

#if defined(__STDC__) && !defined(__STDC_VERSION__)
    #define RGFW_C89
#endif

#ifndef RGFW_USERPTR
	#define RGFW_USERPTR NULL
#endif

#ifndef RGFW_UNUSED
	#define RGFW_UNUSED(x) (void)(x)
#endif

#ifndef RGFW_ROUND
	#define RGFW_ROUND(x) (i32)((x) >= 0 ? (x) + 0.5f : (x) - 0.5f)
#endif

#ifndef RGFW_ALLOC
	#include <stdlib.h>
	#define RGFW_ALLOC malloc
	#define RGFW_FREE free
#endif

#ifndef RGFW_ASSERT
	#include <assert.h>
	#define RGFW_ASSERT assert
#endif

#if !defined(RGFW_MEMCPY) || !defined(RGFW_STRNCMP) || !defined(RGFW_STRNCPY)
    #include <string.h>
#endif

#ifndef RGFW_MEMCPY
	#define RGFW_MEMCPY(dist, src, len) memcpy(dist, src, len)
#endif

#ifndef RGFW_STRNCMP
	#define RGFW_STRNCMP(s1, s2, max) strncmp(s1, s2, max)
#endif

#ifndef RGFW_STRNCPY
	#define RGFW_STRNCPY(dist, src, len) strncpy(dist, src, len)
#endif

#ifndef RGFW_STRSTR
	#define RGFW_STRSTR(str, substr) strstr(str, substr)
#endif

#ifndef RGFW_STRTOL
	#include <stdlib.h>
    #define RGFW_STRTOL(str, endptr, base) strtol(str, endptr, base)
	#define RGFW_ATOF(num) atof(num)
#endif

#ifndef inline
	#ifndef __APPLE__
		#define inline __inline
	#endif
#endif

#ifdef RGFW_WIN95 /* for windows 95 testing (not that it really works) */
	#define RGFW_NO_MONITOR
	#define RGFW_NO_PASSTHROUGH
#endif

#if defined(RGFW_EXPORT) ||  defined(RGFW_IMPORT)
	#if defined(_WIN32)
		#if defined(__TINYC__) && (defined(RGFW_EXPORT) ||  defined(RGFW_IMPORT))
			#define __declspec(x) __attribute__((x))
		#endif

		#if defined(RGFW_EXPORT)
			#define __declspec(dllexport)
		#else
			#define __declspec(dllimport)
		#endif
	#else
		#if defined(RGFW_EXPORT)
			#define __attribute__((visibility("default")))
		#endif
	#endif
#endif

#ifndef RGFW_ENUM
	#define RGFW_ENUM(type, name) type name; enum
#endif

#include <stddef.h>
#ifndef RGFW_INT_DEFINED
	#ifdef RGFW_USE_INT /* optional for any system that might not have stdint.h */
		typedef unsigned char 	u8;
		typedef signed char		i8;
		typedef unsigned short  u16;
		typedef signed short 	i16;
		typedef unsigned long int 	u32;
		typedef signed long int		i32;
		typedef unsigned long long	u64;
		typedef signed long long		i64;
	#else /* use stdint standard types instead of c ""standard"" types */
		#include <stdint.h>

		typedef uint8_t     u8;
		typedef int8_t      i8;
		typedef uint16_t   u16;
		typedef int16_t    i16;
		typedef uint32_t   u32;
		typedef int32_t    i32;
		typedef uint64_t   u64;
		typedef int64_t    i64;
	#endif
    #define RGFW_INT_DEFINED
#endif

#ifndef RGFW_BOOL_DEFINED
    #define RGFW_BOOL_DEFINED
    typedef u8 RGFW_bool;
#endif

#define RGFW_BOOL(x) (RGFW_bool)((x) ? RGFW_TRUE : RGFW_FALSE) /* force an value to be 0 or 1 */
#define RGFW_TRUE (RGFW_bool)1
#define RGFW_FALSE (RGFW_bool)0

#if defined(RGFW_X11) && defined(__APPLE__) && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS_X11
	#define RGFW_UNIX
	#undef __APPLE__
#endif

#if defined(_WIN32) && !defined(RGFW_X11) && !defined(RGFW_UNIX) && !defined(RGFW_WASM) && !defined(RGFW_CUSTOM_BACKEND) /* (if you're using X11 on windows some how) */
	#define RGFW_WINDOWS
	#if defined(_WIN64)
		#define _AMD64_
		#undef _X86_
	#else
		#undef _AMD64_
		#ifndef _X86_
			#define _X86_
		#endif
	#endif

	#ifndef RGFW_NO_XINPUT
		#ifdef __MINGW32__ /* try to find the right header */
			#include <xinput.h>
		#else
			#include <XInput.h>
		#endif
	#endif
#endif
#if !defined(RGFW_NO_X11) && !defined(RGFW_NO_X11) && (defined(__unix__) || defined(RGFW_MACOS_X11) || defined(RGFW_X11))  && !defined(RGFW_WASM)  && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS_X11
	#define RGFW_X11
	#define RGFW_UNIX
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
#elif defined(__APPLE__) && !defined(RGFW_MACOS_X11) && !defined(RGFW_X11)  && !defined(RGFW_WASM)  && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS
	#if !defined(RGFW_BUFFER_BGR)
		#define RGFW_BUFFER_BGR
	#else
		#undef RGFW_BUFFER_BGR
	#endif
#endif

#define RGFW_COCOA_FRAME_NAME NULL

void RGFW_useWayland(RGFW_bool wayland);
RGFW_bool RGFW_usingWayland(void);

#define RGFW_key u8

typedef RGFW_ENUM(u8, RGFW_eventType) {
	RGFW_eventNone = 0, /*!< no event has been sent */
 	RGFW_keyPressed, /* a key has been pressed */
	RGFW_keyReleased, /*!< a key has been released */
	RGFW_mouseButtonPressed, /*!< a mouse button has been pressed (left,middle,right) */
	RGFW_mouseButtonReleased, /*!< a mouse button has been released (left,middle,right) */
	RGFW_mousePosChanged, /*!< the position of the mouse has been changed */
	RGFW_gamepadConnected, /*!< a gamepad was connected */
	RGFW_gamepadDisconnected, /*!< a gamepad was disconnected */
	RGFW_gamepadButtonPressed, /*!< a gamepad button was pressed */
	RGFW_gamepadButtonReleased, /*!< a gamepad button was released */
	RGFW_gamepadAxisMove, /*!< an axis of a gamepad was moved */
	RGFW_windowMoved, /*!< the window was moved (by the user) */
	RGFW_windowResized, /*!< the window was resized (by the user), [on WASM this means the browser was resized] */
	RGFW_focusIn, /*!< window is in focus now */
	RGFW_focusOut, /*!< window is out of focus now */
	RGFW_mouseEnter, /* mouse entered the window */
	RGFW_mouseLeave, /* mouse left the window */
	RGFW_windowRefresh, /* The window content needs to be refreshed */
	RGFW_quit, /*!< the user clicked the quit button */
	RGFW_DND, /*!< a file has been dropped into the window */
	RGFW_DNDInit, /*!< the start of a dnd event, when the place where the file drop is known */
	RGFW_windowMaximized, /*!< the window was maximized */
	RGFW_windowMinimized, /*!< the window was minimized */
	RGFW_windowRestored, /*!< the window was restored */
	RGFW_scaleUpdated /*!< content scale factor changed */
};

typedef RGFW_ENUM(u8, RGFW_mouseButton) {
	RGFW_mouseLeft = 0, /*!< left mouse button is pressed */
	RGFW_mouseMiddle, /*!< mouse-wheel-button is pressed */
	RGFW_mouseRight, /*!< right mouse button is pressed */
	RGFW_mouseScrollUp, /*!< mouse wheel is scrolling up */
	RGFW_mouseScrollDown, /*!< mouse wheel is scrolling down */
	RGFW_mouseMisc1, RGFW_mouseMisc2, RGFW_mouseMisc3, RGFW_mouseMisc4, RGFW_mouseMisc5,
	RGFW_mouseFinal
};

#ifndef RGFW_MAX_PATH
#define RGFW_MAX_PATH 260 /* max length of a path (for dnd) */
#endif
#ifndef RGFW_MAX_DROPS
#define RGFW_MAX_DROPS 260 /* max items you can drop at once */
#endif

#define RGFW_BIT(x) (1 << x)

typedef RGFW_ENUM(u8, RGFW_keymod) {
	RGFW_modCapsLock = RGFW_BIT(0),
	RGFW_modNumLock  = RGFW_BIT(1),
	RGFW_modControl  = RGFW_BIT(2),
	RGFW_modAlt = RGFW_BIT(3),
	RGFW_modShift  = RGFW_BIT(4),
	RGFW_modSuper = RGFW_BIT(5),
	RGFW_modScrollLock = RGFW_BIT(6)
};

typedef RGFW_ENUM(u8, RGFW_gamepadCodes) {
	RGFW_gamepadNone = 0, /*!< or PS X button */
	RGFW_gamepadA, /*!< or PS X button */
	RGFW_gamepadB, /*!< or PS circle button */
	RGFW_gamepadY, /*!< or PS triangle button */
	RGFW_gamepadX, /*!< or PS square button */
	RGFW_gamepadStart, /*!< start button */
	RGFW_gamepadSelect, /*!< select button */
	RGFW_gamepadHome, /*!< home button */
	RGFW_gamepadUp, /*!< dpad up */
	RGFW_gamepadDown, /*!< dpad down */
	RGFW_gamepadLeft, /*!< dpad left */
	RGFW_gamepadRight, /*!< dpad right */
	RGFW_gamepadL1, /*!< left bump */
	RGFW_gamepadL2, /*!< left trigger */
	RGFW_gamepadR1, /*!< right bumper */
	RGFW_gamepadR2, /*!< right trigger */
	RGFW_gamepadL3,  /* left thumb stick */
	RGFW_gamepadR3, /*!< right thumb stick */
	RGFW_gamepadFinal
};

#ifndef RGFW_point
	typedef struct { i32 x, y; } RGFW_point;
#endif
#ifndef RGFW_rect
	typedef struct { i32 x, y, w, h; } RGFW_rect;
#endif
#ifndef RGFW_area
	typedef struct { u32 w, h; } RGFW_area;
#endif

#if defined(__cplusplus) && !defined(__APPLE__)
#define RGFW_POINT(x, y) {(i32)x, (i32)y}
#define RGFW_RECT(x, y, w, h) {(i32)x, (i32)y, (i32)w, (i32)h}
#define RGFW_AREA(w, h) {(u32)w, (u32)h}
#else
#define RGFW_POINT(x, y) (RGFW_point){(i32)(x), (i32)(y)}
#define RGFW_RECT(x, y, w, h) (RGFW_rect){(i32)(x), (i32)(y), (i32)(w), (i32)(h)}
#define RGFW_AREA(w, h) (RGFW_area){(u32)(w), (u32)(h)}
#endif

#ifndef RGFW_NO_MONITOR
	typedef struct RGFW_monitorMode {
		RGFW_area area; /*!< monitor workarea size */
		u32 refreshRate; /*!< monitor refresh rate */
		u8 red, blue, green;
	} RGFW_monitorMode;

	typedef struct RGFW_monitor {
		i32 x, y; /*!< x - y of the monitor workarea */
		char name[128]; /*!< monitor name */
		float scaleX, scaleY; /*!< monitor content scale */
		float pixelRatio; /*!< pixel ratio for monitor (1.0 for regular, 2.0 for hiDPI)  */
		float physW, physH; /*!< monitor physical size in inches */

		RGFW_monitorMode mode;
	} RGFW_monitor;

	RGFW_monitor* RGFW_getMonitors(size_t* len);
	RGFW_monitor RGFW_getPrimaryMonitor(void);

	typedef RGFW_ENUM(u8, RGFW_modeRequest) {
		RGFW_monitorScale = RGFW_BIT(0), /*!< scale the monitor size */
		RGFW_monitorRefresh = RGFW_BIT(1), /*!< change the refresh rate */
		RGFW_monitorRGB = RGFW_BIT(2), /*!< change the monitor RGB bits size */
		RGFW_monitorAll = RGFW_monitorScale | RGFW_monitorRefresh | RGFW_monitorRGB
	};

	RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);
	RGFW_bool RGFW_monitorModeCompare(RGFW_monitorMode mon, RGFW_monitorMode mon2, RGFW_modeRequest request);
#endif

typedef void RGFW_mouse;

RGFW_mouse* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
void RGFW_freeMouse(RGFW_mouse* mouse);

typedef struct RGFW_event {
	RGFW_eventType type; /*!< which event has been sent?*/
	RGFW_point point; /*!< mouse x, y of event (or drop point) */
	RGFW_point vector; /*!< raw mouse movement */
	float scaleX, scaleY; /*!< DPI scaling */
	RGFW_key key; /*!< the physical key of the event, refers to where key is physically*/
	u8 keyChar; /*!< mapped key char of the event */
	RGFW_bool repeat; /*!< key press event repeated (the key is being held) */
	RGFW_keymod keyMod;
	u8 button; /* !< which mouse (or gamepad) button was pressed */
	double scroll; /*!< the raw mouse scroll value */
	u16 gamepad; /*! which gamepad this event applies to (if applicable to any) */
	u8 axisesCount; /*!< number of axises */
	u8 whichAxis; /* which axis was effected */
	RGFW_point axis[4]; /*!< x, y of axises (-100 to 100) */
	char** droppedFiles; /*!< dropped files */
	size_t droppedFilesCount; /*!< house many files were dropped */
	void* _win; /*!< the window this event applies too (for event queue events) */
} RGFW_event;

#ifdef RGFW_WINDOWS
typedef struct RGFW_window_src {
	HWND window; /*!< source window */
	HDC hdc; /*!< source HDC */
	u32 hOffset; /*!< height offset for window */
	HICON hIconSmall, hIconBig; /*!< source window icons */
	#if defined(RGFW_BUFFER)
		HDC hdcMem;
		HBITMAP bitmap;
		u8* bitmapBits;
	#endif
	RGFW_area maxSize, minSize, aspectRatio; /*!< for setting max/min resize (RGFW_WINDOWS) */
} RGFW_window_src;
#elif defined(RGFW_UNIX)
typedef struct RGFW_window_src {
#if defined(RGFW_X11)
	Display* display; /*!< source display */
	Window window; /*!< source window */
	#if defined(RGFW_BUFFER)
			XImage* bitmap;
	#endif
	GC gc;
	XVisualInfo visual;
    #ifdef RGFW_ADVANCED_SMOOTH_RESIZE
        i64 counter_value;
        XID counter;
    #endif
#endif /* RGFW_X11 */
} RGFW_window_src;
#endif /* RGFW_UNIX */
#if defined(RGFW_MACOS)
typedef struct RGFW_window_src {
	void* window;
	void* view; /* apple viewpoint thingy */
	void* mouse;
} RGFW_window_src;
#elif defined(RGFW_WASM)
typedef struct RGFW_window_src {
	#if defined(RGFW_WEBGPU)
		WGPUInstance ctx;
        WGPUDevice device;
        WGPUQueue queue;
	#else
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
	#endif
} RGFW_window_src;
#endif

typedef RGFW_ENUM(u32, RGFW_windowFlags) {
	RGFW_windowNoInitAPI = RGFW_BIT(0), /* do NOT init an API (including the software rendering buffer) (mostly for bindings. you can also use `#define RGFW_NO_API`) */
	RGFW_windowNoBorder = RGFW_BIT(1), /*!< the window doesn't have a border */
	RGFW_windowNoResize = RGFW_BIT(2), /*!< the window cannot be resized by the user */
	RGFW_windowAllowDND = RGFW_BIT(3), /*!< the window supports drag and drop */
	RGFW_windowHideMouse = RGFW_BIT(4), /*! the window should hide the mouse (can be toggled later on using `RGFW_window_mouseShow`) */
	RGFW_windowFullscreen = RGFW_BIT(5), /*!< the window is fullscreen by default */
	RGFW_windowTransparent = RGFW_BIT(6), /*!< the window is transparent (only properly works on X11 and MacOS, although it's meant for for windows) */
	RGFW_windowCenter = RGFW_BIT(7), /*! center the window on the screen */
	RGFW_windowCocoaCHDirToRes = RGFW_BIT(9), /*! (cocoa only), change directory to resource folder */
	RGFW_windowScaleToMonitor = RGFW_BIT(10), /*! scale the window to the screen */
	RGFW_windowHide = RGFW_BIT(11), /*! the window is hidden */
	RGFW_windowMaximize = RGFW_BIT(12),
	RGFW_windowCenterCursor = RGFW_BIT(13),
	RGFW_windowFloating = RGFW_BIT(14), /*!< create a floating window */
	RGFW_windowFreeOnClose = RGFW_BIT(15), /*!< free (RGFW_window_close) the RGFW_window struct when the window is closed (by the end user) */
	RGFW_windowFocusOnShow = RGFW_BIT(16), /*!< focus the window when it's shown */
	RGFW_windowMinimize = RGFW_BIT(17), /*!< focus the window when it's shown */
	RGFW_windowFocus = RGFW_BIT(18), /*!< if the window is in focus */
	RGFW_windowedFullscreen = RGFW_windowNoBorder | RGFW_windowMaximize
};

typedef struct RGFW_window {
	RGFW_window_src src; /*!< src window data */
#if defined(RGFW_BUFFER)
	u8* buffer; /*!< buffer for non-GPU systems (OSMesa, basic software rendering) */
	RGFW_area bufferSize;
#endif
	void* userPtr; /* ptr for usr data */
	RGFW_event event; /*!< current event */
	RGFW_rect r; /*!< the x, y, w and h of the struct */
	RGFW_point _lastMousePoint; /*!< last cusor point (for raw mouse data) */
	u32 _flags; /*!< windows flags (for RGFW to check) */
	RGFW_rect _oldRect; /*!< rect before fullscreen */
} RGFW_window; /*!< window structure for managing the window */

#if defined(RGFW_X11) || defined(RGFW_MACOS)
	typedef u64 RGFW_thread; /*!< thread type unix */
#else
	typedef void* RGFW_thread; /*!< thread type for windows */
#endif

RGFW_bool RGFW_monitor_scaleToWindow(RGFW_monitor mon, RGFW_window* win);

void RGFW_setClassName(const char* name);
void RGFW_setXInstName(const char* name); /*!< X11 instance name (window name will by used by default) */
void RGFW_moveToMacOSResourceDir(void);

RGFW_window* RGFW_createWindow(const char* name, RGFW_rect rect, RGFW_windowFlags flag);
RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);

void RGFW_window_initBuffer(RGFW_window* win);
void RGFW_window_initBufferSize(RGFW_window* win, RGFW_area area);
void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);
void RGFW_window_setFlags(RGFW_window* win, RGFW_windowFlags);

RGFW_area RGFW_getScreenSize(void);
RGFW_event* RGFW_window_checkEvent(RGFW_window* win); /*!< check current event (returns a pointer to win->event or NULL if there is no event)*/

typedef RGFW_ENUM(i32, RGFW_eventWait) {
	RGFW_eventNoWait = 0,
	RGFW_eventWaitNext = -1
};

void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);
void RGFW_window_checkEvents(RGFW_window* win, i32 waitMS);
void RGFW_stopCheckEvents(void);
void RGFW_window_close(RGFW_window* win); /*!< close the window and free leftover data */
void RGFW_window_move(RGFW_window* win, RGFW_point v);

#ifndef RGFW_NO_MONITOR
	void RGFW_window_moveToMonitor(RGFW_window* win, RGFW_monitor m /* monitor */);
#endif

void RGFW_window_resize(RGFW_window* win, RGFW_area a);
void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
void RGFW_window_setMaxSize(RGFW_window* win, RGFW_area a);
void RGFW_window_focus(RGFW_window* win); /*!< sets the focus to this window */
RGFW_bool RGFW_window_isInFocus(RGFW_window* win); /*!< checks the focus to this window */
void RGFW_window_raise(RGFW_window* win); /*!< raise the window (to the top) */
void RGFW_window_maximize(RGFW_window* win); /*!< maximize the window */
void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen); /*!< turn fullscreen on / off for a window */
void RGFW_window_center(RGFW_window* win); /*!< center the window */
void RGFW_window_minimize(RGFW_window* win); /*!< minimize the window (in taskbar (per OS))*/
void RGFW_window_restore(RGFW_window* win); /*!< restore the window from minimized (per OS)*/
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating); /*!< make the window a floating window */
void RGFW_window_setOpacity(RGFW_window* win, u8 opacity); /*!< sets the opacity of a window */
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
RGFW_bool RGFW_window_borderless(RGFW_window* win);
void RGFW_window_setDND(RGFW_window* win, RGFW_bool allow);
RGFW_bool RGFW_window_allowsDND(RGFW_window* win);
#ifndef RGFW_NO_PASSTHROUGH
	void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
#endif

void RGFW_window_setName(RGFW_window* win, const char* name);

RGFW_bool RGFW_window_setIcon(RGFW_window* win, /*!< source window */
	u8* icon /*!< icon bitmap */,
	RGFW_area a /*!< width and height of the bitmap */,
	i32 channels /*!< how many channels the bitmap has (rgb : 3, rgba : 4) */
); /*!< image MAY be resized by default, set both the taskbar and window icon */

typedef RGFW_ENUM(u8, RGFW_icon) {
	RGFW_iconTaskbar = RGFW_BIT(0),
	RGFW_iconWindow = RGFW_BIT(1),
	RGFW_iconBoth = RGFW_iconTaskbar | RGFW_iconWindow
};
RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* icon, RGFW_area a, i32 channels, u8 type);

void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);

RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse);
RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win); /*!< sets the mouse to the default mouse icon */

void RGFW_window_mouseHold(RGFW_window* win, RGFW_area area);
void RGFW_window_mouseUnhold(RGFW_window* win);
void RGFW_window_hide(RGFW_window* win);
void RGFW_window_show(RGFW_window* win);
void RGFW_window_setShouldClose(RGFW_window* win, RGFW_bool shouldClose);

RGFW_point RGFW_getGlobalMousePoint(void);
RGFW_point RGFW_window_getMousePoint(RGFW_window* win);

void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
RGFW_bool RGFW_window_mouseHidden(RGFW_window* win);
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point v);
RGFW_bool RGFW_window_shouldClose(RGFW_window* win);
RGFW_bool RGFW_window_isFullscreen(RGFW_window* win);
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);
RGFW_bool RGFW_window_isFloating(RGFW_window* win);

#ifndef RGFW_NO_MONITOR
void RGFW_window_scaleToMonitor(RGFW_window* win);
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);
#endif

RGFW_bool RGFW_isPressed(RGFW_window* win, RGFW_key key); /*!< if key is pressed (key code)*/
RGFW_bool RGFW_wasPressed(RGFW_window* win, RGFW_key key); /*!< if key was pressed (checks previous state only) (key code) */
RGFW_bool RGFW_isHeld(RGFW_window* win, RGFW_key key); /*!< if key is held (key code) */
RGFW_bool RGFW_isReleased(RGFW_window* win, RGFW_key key); /*!< if key is released (key code) */
RGFW_bool RGFW_isClicked(RGFW_window* win, RGFW_key key /*!< key code */);
RGFW_bool RGFW_isMousePressed(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
RGFW_bool RGFW_isMouseHeld(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
RGFW_bool RGFW_isMouseReleased(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
RGFW_bool RGFW_wasMousePressed(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );

typedef ptrdiff_t RGFW_ssize_t;

const char* RGFW_readClipboard(size_t* size); /*!< read clipboard data */
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);
void RGFW_writeClipboard(const char* text, u32 textLen); /*!< write text to the clipboard */

typedef RGFW_ENUM(u8, RGFW_debugType) {
	RGFW_typeError = 0, RGFW_typeWarning, RGFW_typeInfo
};

typedef RGFW_ENUM(u8, RGFW_errorCode) {
	RGFW_noError = 0, /*!< no error */
	RGFW_errWayland,
	RGFW_errDirectXContext,
	RGFW_errIOKit,
	RGFW_errClipboard,
	RGFW_errFailedFuncLoad,
	RGFW_errBuffer,
	RGFW_infoMonitor, RGFW_infoWindow, RGFW_infoBuffer, RGFW_infoGlobal,
	RGFW_warningWayland
};

typedef struct RGFW_debugContext { RGFW_window* win; RGFW_monitor monitor; u32 srcError; } RGFW_debugContext;

#if defined(__cplusplus) && !defined(__APPLE__)
#define RGFW_DEBUG_CTX(win, err) {win, { 0 }, err}
#define RGFW_DEBUG_CTX_MON(monitor) {_RGFW.root, monitor, 0}
#else
#define RGFW_DEBUG_CTX(win, err) (RGFW_debugContext){win, (RGFW_monitor){ 0 }, err}
#define RGFW_DEBUG_CTX_MON(monitor) (RGFW_debugContext){_RGFW.root, monitor, 0}
#endif

typedef void (* RGFW_debugfunc)(RGFW_debugType type, RGFW_errorCode err, RGFW_debugContext ctx, const char* msg);
RGFW_debugfunc RGFW_setDebugCallback(RGFW_debugfunc func);
void RGFW_sendDebugInfo(RGFW_debugType type, RGFW_errorCode err, RGFW_debugContext ctx, const char* msg);

typedef void (* RGFW_windowMovedfunc)(RGFW_window* win, RGFW_rect r);
typedef void (* RGFW_windowResizedfunc)(RGFW_window* win, RGFW_rect r);
typedef void (* RGFW_windowRestoredfunc)(RGFW_window* win, RGFW_rect r);
typedef void (* RGFW_windowMaximizedfunc)(RGFW_window* win, RGFW_rect r);
typedef void (* RGFW_windowMinimizedfunc)(RGFW_window* win, RGFW_rect r);
typedef void (* RGFW_windowQuitfunc)(RGFW_window* win);
typedef void (* RGFW_focusfunc)(RGFW_window* win, RGFW_bool inFocus);
typedef void (* RGFW_mouseNotifyfunc)(RGFW_window* win, RGFW_point point, RGFW_bool status);
typedef void (* RGFW_mousePosfunc)(RGFW_window* win, RGFW_point point, RGFW_point vector);
typedef void (* RGFW_dndInitfunc)(RGFW_window* win, RGFW_point point);
typedef void (* RGFW_windowRefreshfunc)(RGFW_window* win);
typedef void (* RGFW_keyfunc)(RGFW_window* win, u8 key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool pressed);
typedef void (* RGFW_mouseButtonfunc)(RGFW_window* win, RGFW_mouseButton button, double scroll, RGFW_bool pressed);
typedef void (* RGFW_gamepadButtonfunc)(RGFW_window* win, u16 gamepad, u8 button, RGFW_bool pressed);
typedef void (* RGFW_gamepadAxisfunc)(RGFW_window* win, u16 gamepad, RGFW_point axis[2], u8 axisesCount, u8 whichAxis);
typedef void (* RGFW_gamepadfunc)(RGFW_window* win, u16 gamepad, RGFW_bool connected);
typedef void (* RGFW_dndfunc)(RGFW_window* win, char** droppedFiles, size_t droppedFilesCount);
typedef void (* RGFW_scaleUpdatedfunc)(RGFW_window* win, float scaleX, float scaleY);

RGFW_windowMovedfunc RGFW_setWindowMovedCallback(RGFW_windowMovedfunc func);
RGFW_windowResizedfunc RGFW_setWindowResizedCallback(RGFW_windowResizedfunc func);
RGFW_windowQuitfunc RGFW_setWindowQuitCallback(RGFW_windowQuitfunc func);
RGFW_mousePosfunc RGFW_setMousePosCallback(RGFW_mousePosfunc func);
RGFW_windowRefreshfunc RGFW_setWindowRefreshCallback(RGFW_windowRefreshfunc func);
RGFW_focusfunc RGFW_setFocusCallback(RGFW_focusfunc func);
RGFW_mouseNotifyfunc RGFW_setMouseNotifyCallback(RGFW_mouseNotifyfunc func);
RGFW_dndfunc RGFW_setDndCallback(RGFW_dndfunc func);
RGFW_dndInitfunc RGFW_setDndInitCallback(RGFW_dndInitfunc func);
RGFW_keyfunc RGFW_setKeyCallback(RGFW_keyfunc func);
RGFW_mouseButtonfunc RGFW_setMouseButtonCallback(RGFW_mouseButtonfunc func);
RGFW_gamepadButtonfunc RGFW_setGamepadButtonCallback(RGFW_gamepadButtonfunc func);
RGFW_gamepadAxisfunc RGFW_setGamepadAxisCallback(RGFW_gamepadAxisfunc func);
RGFW_gamepadfunc RGFW_setGamepadCallback(RGFW_gamepadfunc func);
RGFW_windowResizedfunc RGFW_setWindowMaximizedCallback(RGFW_windowResizedfunc func);
RGFW_windowResizedfunc RGFW_setWindowMinimizedCallback(RGFW_windowResizedfunc func);
RGFW_windowResizedfunc RGFW_setWindowRestoredCallback(RGFW_windowResizedfunc func);
RGFW_scaleUpdatedfunc RGFW_setScaleUpdatedCallback(RGFW_scaleUpdatedfunc func);

#ifndef RGFW_NO_THREADS
#if defined(__unix__) || defined(__APPLE__) || defined(RGFW_WASM) || defined(RGFW_CUSTOM_BACKEND)
	typedef void* (* RGFW_threadFunc_ptr)(void*);
#else
	typedef DWORD (__stdcall *RGFW_threadFunc_ptr) (LPVOID lpThreadParameter);
#endif

RGFW_thread RGFW_createThread(RGFW_threadFunc_ptr ptr, void* args); /*!< create a thread */
void RGFW_cancelThread(RGFW_thread thread); /*!< cancels a thread */
void RGFW_joinThread(RGFW_thread thread); /*!< join thread to current thread */
void RGFW_setThreadPriority(RGFW_thread thread, u8 priority); /*!< sets the priority priority  */
#endif

typedef RGFW_ENUM(u8, RGFW_gamepadType) {
	RGFW_gamepadMicrosoft = 0, RGFW_gamepadSony, RGFW_gamepadNintendo, RGFW_gamepadLogitech, RGFW_gamepadUnknown
};

u32 RGFW_isPressedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_isReleasedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_isHeldGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_wasPressedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
RGFW_point RGFW_getGamepadAxis(RGFW_window* win, u16 controller, u16 whichAxis);
const char* RGFW_getGamepadName(RGFW_window* win, u16 controller);
size_t RGFW_getGamepadCount(RGFW_window* win);
RGFW_gamepadType RGFW_getGamepadType(RGFW_window* win, u16 controller);

void RGFW_window_makeCurrent(RGFW_window* win);
RGFW_window* RGFW_getCurrent(void);

void RGFW_window_swapBuffers(RGFW_window* win); /*!< swap the rendering buffer */
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
void RGFW_window_swapBuffers_software(RGFW_window* win);

typedef void (*RGFW_proc)(void); /* function pointer equivalent of void* */
#if defined(RGFW_X11)
	#define VK_USE_PLATFORM_XLIB_KHR
	#define RGFW_VK_SURFACE "VK_KHR_xlib_surface"
#elif defined(RGFW_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
	#define OEMRESOURCE
	#define RGFW_VK_SURFACE "VK_KHR_win32_surface"
#elif defined(RGFW_MACOS) && !defined(RGFW_MACOS_X11)
	#define VK_USE_PLATFORM_MACOS_MVK
	#define RGFW_VK_SURFACE "VK_MVK_macos_surface"
#else
	#define RGFW_VK_SURFACE NULL
#endif

const char** RGFW_getVKRequiredInstanceExtensions(size_t* count); /*!< gets (static) extension array (and size (which will be 2)) */

#include <vulkan/vulkan.h>

VkResult RGFW_window_createVKSurface(RGFW_window* win, VkInstance instance, VkSurfaceKHR* surface);
#ifdef RGFW_DIRECTX
#ifndef RGFW_WINDOWS
	#undef RGFW_DIRECTX
#else
	#define OEMRESOURCE
	#include <dxgi.h>

	#ifndef __cplusplus
		#define __uuidof(T) IID_##T
	#endif
int RGFW_window_createDXSwapChain(RGFW_window* win, IDXGIFactory* pFactory, IUnknown* pDevice, IDXGISwapChain** swapchain);
#endif
#endif

i32 RGFW_init(void); /*!< is called by default when the first window is created by default */
void RGFW_deinit(void); /*!< is called by default when the last open window is closed */
double RGFW_getTime(void); /*!< get time in seconds since RGFW_setTime, which ran when the first window is open  */
u64 RGFW_getTimeNS(void); /*!< get time in nanoseconds RGFW_setTime, which ran when the first window is open */
void RGFW_sleep(u64 milisecond); /*!< sleep for a set time */
void RGFW_setTime(double time); /*!< set timer in seconds */
u64 RGFW_getTimerValue(void); /*!< get API timer value */
u64 RGFW_getTimerFreq(void); /*!< get API time freq */
u32 RGFW_checkFPS(double startTime, u32 frameCount, u32 fpsCap);
void RGFW_setRootWindow(RGFW_window* win);
RGFW_window* RGFW_getRootWindow(void);
void RGFW_eventQueuePush(RGFW_event event);
RGFW_event* RGFW_eventQueuePop(RGFW_window* win);

#undef RGFW_key
typedef RGFW_ENUM(u8, RGFW_key) {
	RGFW_keyNULL = 0,
	RGFW_escape = '\033',
	RGFW_backtick = '`',
	RGFW_0 = '0',
	RGFW_1 = '1',
	RGFW_2 = '2',
	RGFW_3 = '3',
	RGFW_4 = '4',
	RGFW_5 = '5',
	RGFW_6 = '6',
	RGFW_7 = '7',
	RGFW_8 = '8',
	RGFW_9 = '9',
	RGFW_minus = '-',
	RGFW_equals = '=',
	RGFW_backSpace = '\b',
	RGFW_tab = '\t',
	RGFW_space = ' ',
	RGFW_a = 'a',
	RGFW_b = 'b',
	RGFW_c = 'c',
	RGFW_d = 'd',
	RGFW_e = 'e',
	RGFW_f = 'f',
	RGFW_g = 'g',
	RGFW_h = 'h',
	RGFW_i = 'i',
	RGFW_j = 'j',
	RGFW_k = 'k',
	RGFW_l = 'l',
	RGFW_m = 'm',
	RGFW_n = 'n',
	RGFW_o = 'o',
	RGFW_p = 'p',
	RGFW_q = 'q',
	RGFW_r = 'r',
	RGFW_s = 's',
	RGFW_t = 't',
	RGFW_u = 'u',
	RGFW_v = 'v',
	RGFW_w = 'w',
	RGFW_x = 'x',
	RGFW_y = 'y',
	RGFW_z = 'z',
	RGFW_period = '.',
	RGFW_comma = ',',
	RGFW_slash = '/',
	RGFW_bracket = '{',
	RGFW_closeBracket = '}',
	RGFW_semicolon = ';',
	RGFW_apostrophe = '\'',
	RGFW_backSlash = '\\',
	RGFW_return = '\n',
	RGFW_delete = '\177', /* 127 */
	RGFW_F1,
	RGFW_F2,
	RGFW_F3,
	RGFW_F4,
	RGFW_F5,
	RGFW_F6,
	RGFW_F7,
	RGFW_F8,
	RGFW_F9,
	RGFW_F10,
	RGFW_F11,
	RGFW_F12,
	RGFW_capsLock,
	RGFW_shiftL,
	RGFW_controlL,
	RGFW_altL,
	RGFW_superL,
	RGFW_shiftR,
	RGFW_controlR,
	RGFW_altR,
	RGFW_superR,
	RGFW_up,
	RGFW_down,
	RGFW_left,
	RGFW_right,
	RGFW_insert,
	RGFW_end,
	RGFW_home,
	RGFW_pageUp,
	RGFW_pageDown,
	RGFW_numLock,
	RGFW_KP_Slash,
	RGFW_multiply,
	RGFW_KP_Minus,
	RGFW_KP_1,
	RGFW_KP_2,
	RGFW_KP_3,
	RGFW_KP_4,
	RGFW_KP_5,
	RGFW_KP_6,
	RGFW_KP_7,
	RGFW_KP_8,
	RGFW_KP_9,
	RGFW_KP_0,
	RGFW_KP_Period,
	RGFW_KP_Return,
	RGFW_scrollLock,
	RGFW_keyLast = 256 /* padding for alignment ~(175 by default) */
};

u32 RGFW_apiKeyToRGFW(u32 keycode);

typedef RGFW_ENUM(u8, RGFW_mouseIcons) {
	RGFW_mouseNormal = 0,
	RGFW_mouseArrow,
	RGFW_mouseIbeam,
	RGFW_mouseCrosshair,
	RGFW_mousePointingHand,
	RGFW_mouseResizeEW,
	RGFW_mouseResizeNS,
	RGFW_mouseResizeNWSE,
	RGFW_mouseResizeNESW,
	RGFW_mouseResizeAll,
	RGFW_mouseNotAllowed,
    RGFW_mouseIconFinal = 16 /* padding for alignment */
};

inline RGFW_bool RGFW_useWaylandBool;
void RGFW_useWayland(RGFW_bool wayland);
RGFW_bool RGFW_usingWayland(void);

inline char* RGFW_clipboard_data;
void RGFW_clipboard_switch(char* newstr);

#define RGFW_CHECK_CLIPBOARD() \
	if (size <= 0 && RGFW_clipboard_data != NULL) \
		return (const char*)RGFW_clipboard_data; \
	else if (size <= 0) \
		return "\0";

const char* RGFW_readClipboard(size_t* len);

inline RGFW_debugfunc RGFW_debugCallback;
RGFW_debugfunc RGFW_setDebugCallback(RGFW_debugfunc func);

#ifdef RGFW_DEBUG
#include <stdio.h>
#endif

void RGFW_sendDebugInfo(RGFW_debugType type, RGFW_errorCode err, RGFW_debugContext ctx, const char* msg);

inline u64 RGFW_timerOffset;
void RGFW_setTime(double time);

double RGFW_getTime(void);

u64 RGFW_getTimeNS(void);

#ifndef RGFW_CUSTOM_BACKEND
#if defined(RGFW_X11)
	#define RGFW_OS_BASED_VALUE(l, w, m, h) l
#elif defined(RGFW_WINDOWS)
	#define RGFW_OS_BASED_VALUE(l, w, m, h) w
#elif defined(RGFW_MACOS)
	#define RGFW_OS_BASED_VALUE(l, w, m, h) m
#elif defined(RGFW_WASM)
	#define RGFW_OS_BASED_VALUE(l, w, m, h) h
#endif

#if !defined(__cplusplus) && !defined(RGFW_C89)
#define RGFW_NEXT ,
#define RGFW_MAP
#else
#define RGFW_NEXT ;
#define RGFW_MAP RGFW_keycodes
#endif

inline u8 RGFW_keycodes [RGFW_OS_BASED_VALUE(256, 512, 128, 256)];
void RGFW_init_keys(void);

u32 RGFW_apiKeyToRGFW(u32 keycode);
#endif /* RGFW_CUSTOM_BACKEND */

typedef struct {
	RGFW_bool current  : 1;
	RGFW_bool prev  : 1;
} RGFW_keyState;

inline RGFW_keyState RGFW_keyboard[RGFW_keyLast];

void RGFW_resetKey(void);

inline RGFW_keyState RGFW_gamepadPressed[4][32]; /*!< if a key is currently pressed or not (per gamepad) */
inline RGFW_point RGFW_gamepadAxes[4][4]; /*!< if a key is currently pressed or not (per gamepad) */
inline RGFW_gamepadType RGFW_gamepads_type[4]; /*!< if a key is currently pressed or not (per gamepad) */
inline i32 RGFW_gamepads[4]; /*!< limit of 4 gamepads at a time */
inline char RGFW_gamepads_name[4][128]; /*!< gamepad names */
inline u16 RGFW_gamepadCount ; /*!< the actual amount of gamepads */

void RGFW_windowMovedfuncEMPTY(RGFW_window* win, RGFW_rect r);
void RGFW_windowResizedfuncEMPTY(RGFW_window* win, RGFW_rect r);
void RGFW_windowRestoredfuncEMPTY(RGFW_window* win, RGFW_rect r);
void RGFW_windowMinimizedfuncEMPTY(RGFW_window* win, RGFW_rect r);
void RGFW_windowMaximizedfuncEMPTY(RGFW_window* win, RGFW_rect r);
void RGFW_windowQuitfuncEMPTY(RGFW_window* win);
void RGFW_focusfuncEMPTY(RGFW_window* win, RGFW_bool inFocus);
void RGFW_mouseNotifyfuncEMPTY(RGFW_window* win, RGFW_point point, RGFW_bool status);
void RGFW_mousePosfuncEMPTY(RGFW_window* win, RGFW_point point, RGFW_point vector);
void RGFW_dndInitfuncEMPTY(RGFW_window* win, RGFW_point point);
void RGFW_windowRefreshfuncEMPTY(RGFW_window* win);
void RGFW_keyfuncEMPTY(RGFW_window* win, RGFW_key key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool pressed);
void RGFW_mouseButtonfuncEMPTY(RGFW_window* win, RGFW_mouseButton button, double scroll, RGFW_bool pressed);
void RGFW_gamepadButtonfuncEMPTY(RGFW_window* win, u16 gamepad, u8 button, RGFW_bool pressed);
void RGFW_gamepadAxisfuncEMPTY(RGFW_window* win, u16 gamepad, RGFW_point axis[2], u8 axisesCount, u8 whichAxis);
void RGFW_gamepadfuncEMPTY(RGFW_window* win, u16 gamepad, RGFW_bool connected);
void RGFW_dndfuncEMPTY(RGFW_window* win, char** droppedFiles, size_t droppedFilesCount);
void RGFW_scaleUpdatedfuncEMPTY(RGFW_window* win, float scaleX, float scaleY);

#define RGFW_CALLBACK_DEFINE(x, x2) \
inline RGFW_##x##func RGFW_##x##Callback = RGFW_##x##funcEMPTY; \
inline RGFW_##x##func RGFW_set##x2##Callback(RGFW_##x##func func) { \
    RGFW_##x##func prev = RGFW_##x##Callback; \
    RGFW_##x##Callback = func; \
    return prev; \
}
RGFW_CALLBACK_DEFINE(windowMaximized, WindowMaximized)
RGFW_CALLBACK_DEFINE(windowMinimized, WindowMinimized)
RGFW_CALLBACK_DEFINE(windowRestored, WindowRestored)
RGFW_CALLBACK_DEFINE(windowMoved, WindowMoved)
RGFW_CALLBACK_DEFINE(windowResized, WindowResized)
RGFW_CALLBACK_DEFINE(windowQuit, WindowQuit)
RGFW_CALLBACK_DEFINE(mousePos, MousePos)
RGFW_CALLBACK_DEFINE(windowRefresh, WindowRefresh)
RGFW_CALLBACK_DEFINE(focus, Focus)
RGFW_CALLBACK_DEFINE(mouseNotify, MouseNotify)
RGFW_CALLBACK_DEFINE(dnd, Dnd)
RGFW_CALLBACK_DEFINE(dndInit, DndInit)
RGFW_CALLBACK_DEFINE(key, Key)
RGFW_CALLBACK_DEFINE(mouseButton, MouseButton)
RGFW_CALLBACK_DEFINE(gamepadButton, GamepadButton)
RGFW_CALLBACK_DEFINE(gamepadAxis, GamepadAxis)
RGFW_CALLBACK_DEFINE(gamepad, Gamepad)
RGFW_CALLBACK_DEFINE(scaleUpdated, ScaleUpdated)
#undef RGFW_CALLBACK_DEFINE

void RGFW_window_checkEvents(RGFW_window* win, i32 waitMS);
void RGFW_window_checkMode(RGFW_window* win);

#define SET_ATTRIB(a, v) { \
    RGFW_ASSERT(((size_t) index + 1) < sizeof(attribs) / sizeof(attribs[0])); \
    attribs[index++] = a; \
    attribs[index++] = v; \
}

#define RGFW_EVENT_PASSED 		RGFW_BIT(24) /* if a queued event was passed */
#define RGFW_EVENT_QUIT 		RGFW_BIT(25) /* the window close button was pressed */
#define RGFW_HOLD_MOUSE			RGFW_BIT(26) /*!< hold the moues still */
#define RGFW_MOUSE_LEFT 		RGFW_BIT(27) /* if mouse left the window */
#define RGFW_WINDOW_ALLOC 		RGFW_BIT(28) /* if window was allocated by RGFW */
#define RGFW_BUFFER_ALLOC 		RGFW_BIT(29) /* if window.buffer was allocated by RGFW */
#define RGFW_WINDOW_INIT 		RGFW_BIT(30) /* if window.buffer was allocated by RGFW */
#define RGFW_INTERNAL_FLAGS (RGFW_EVENT_QUIT | RGFW_EVENT_PASSED | RGFW_HOLD_MOUSE |  RGFW_MOUSE_LEFT | RGFW_WINDOW_ALLOC | RGFW_BUFFER_ALLOC | RGFW_windowFocus)

RGFW_window* RGFW_createWindow(const char* name, RGFW_rect rect, RGFW_windowFlags flags);

#if defined(RGFW_USE_XDL) && defined(RGFW_X11)
	#define XDL_IMPLEMENTATION
	#include "XDL.h"
#endif

#define RGFW_MAX_EVENTS 32
typedef struct RGFW_globalStruct {
    RGFW_window* root;
    RGFW_window* current;
    i32 windowCount;
    i32 eventLen;
    i32 eventIndex;

    #ifdef RGFW_X11
        Display* display;
        Window helperWindow;
    	char* clipboard; /* for writing to the clipboard selection */
	    size_t clipboard_len;
    #endif
    #if defined(RGFW_X11) || defined(RGFW_WINDOWS)
        RGFW_mouse* hiddenMouse;
    #endif
    RGFW_event events[RGFW_MAX_EVENTS];

} RGFW_globalStruct;
#ifndef RGFW_C89
inline RGFW_globalStruct _RGFW;
#else
inline RGFW_globalStruct _RGFW;
#endif

void RGFW_eventQueuePush(RGFW_event event);
RGFW_event* RGFW_eventQueuePop(RGFW_window* win);
RGFW_event* RGFW_window_checkEventCore(RGFW_window* win);
void RGFW_window_basic_init(RGFW_window* win, RGFW_rect rect, RGFW_windowFlags flags);
void RGFW_setRootWindow(RGFW_window* win);
RGFW_window* RGFW_getRootWindow(void);

void RGFW_window_basic_init(RGFW_window* win, RGFW_rect rect, RGFW_windowFlags flags);
void RGFW_window_setFlags(RGFW_window* win, RGFW_windowFlags flags);
RGFW_bool RGFW_window_isInFocus(RGFW_window* win);
void RGFW_window_initBuffer(RGFW_window* win);
void RGFW_window_initBufferSize(RGFW_window* win, RGFW_area area);

#ifdef RGFW_MACOS
void RGFW_window_cocoaSetLayer(RGFW_window* win, void* layer);
void* RGFW_cocoaGetLayer(void);
#endif

inline const char* RGFW_className;
void RGFW_setClassName(const char* name);

#ifndef RGFW_X11
void RGFW_setXInstName(const char* name);
#endif

inline RGFW_keyState RGFW_mouseButtons[RGFW_mouseFinal];

RGFW_bool RGFW_isMousePressed(RGFW_window* win, RGFW_mouseButton button);
RGFW_bool RGFW_wasMousePressed(RGFW_window* win, RGFW_mouseButton button);
RGFW_bool RGFW_isMouseHeld(RGFW_window* win, RGFW_mouseButton button);
RGFW_bool RGFW_isMouseReleased(RGFW_window* win, RGFW_mouseButton button);
RGFW_point RGFW_window_getMousePoint(RGFW_window* win);
RGFW_bool RGFW_isPressed(RGFW_window* win, RGFW_key key);
RGFW_bool RGFW_wasPressed(RGFW_window* win, RGFW_key key);
RGFW_bool RGFW_isHeld(RGFW_window* win, RGFW_key key);
RGFW_bool RGFW_isClicked(RGFW_window* win, RGFW_key key);
RGFW_bool RGFW_isReleased(RGFW_window* win, RGFW_key key);
void RGFW_window_makeCurrent(RGFW_window* win);
RGFW_window* RGFW_getCurrent(void);
void RGFW_window_swapBuffers(RGFW_window* win);
void RGFW_setBit(u32* data, u32 bit, RGFW_bool value);
void RGFW_window_center(RGFW_window* win);
RGFW_bool RGFW_monitor_scaleToWindow(RGFW_monitor mon, RGFW_window* win);
void RGFW_splitBPP(u32 bpp, RGFW_monitorMode* mode);
RGFW_bool RGFW_monitorModeCompare(RGFW_monitorMode mon, RGFW_monitorMode mon2, RGFW_modeRequest request);
RGFW_bool RGFW_window_shouldClose(RGFW_window* win);
void RGFW_window_setShouldClose(RGFW_window* win, RGFW_bool shouldClose);

#ifndef RGFW_NO_MONITOR
void RGFW_window_scaleToMonitor(RGFW_window* win);
void RGFW_window_moveToMonitor(RGFW_window* win, RGFW_monitor m);
#endif

RGFW_bool RGFW_window_setIcon(RGFW_window* win, u8* icon, RGFW_area a, i32 channels);

void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);
void RGFW_releaseCursor(RGFW_window* win);
void RGFW_window_mouseHold(RGFW_window* win, RGFW_area area);
void RGFW_window_mouseUnhold(RGFW_window* win);
u32 RGFW_checkFPS(double startTime, u32 frameCount, u32 fpsCap);

#if defined(RGFW_BUFFER)
void RGFW_RGB_to_BGR(RGFW_window* win, u8* data);
#endif

u32 RGFW_isPressedGamepad(RGFW_window* win, u8 c, RGFW_gamepadCodes button);
u32 RGFW_wasPressedGamepad(RGFW_window* win, u8 c, RGFW_gamepadCodes button);
u32 RGFW_isReleasedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_isHeldGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
RGFW_point RGFW_getGamepadAxis(RGFW_window* win, u16 controller, u16 whichAxis);
const char* RGFW_getGamepadName(RGFW_window* win, u16 controller);
size_t RGFW_getGamepadCount(RGFW_window* win);
RGFW_gamepadType RGFW_getGamepadType(RGFW_window* win, u16 controller);
void RGFW_updateKeyMod(RGFW_window* win, RGFW_keymod mod, RGFW_bool value);
void RGFW_updateKeyMod(RGFW_window* win, RGFW_keymod mod, RGFW_bool value);
void RGFW_updateKeyModsPro(RGFW_window* win, RGFW_bool capital, RGFW_bool numlock, RGFW_bool control, RGFW_bool alt, RGFW_bool shift, RGFW_bool super, RGFW_bool scroll);
void RGFW_updateKeyMods(RGFW_window* win, RGFW_bool capital, RGFW_bool numlock, RGFW_bool scroll);
void RGFW_window_showMouseFlags(RGFW_window* win, RGFW_bool show);
RGFW_bool RGFW_window_mouseHidden(RGFW_window* win);
RGFW_bool RGFW_window_borderless(RGFW_window* win);
RGFW_bool RGFW_window_isFullscreen(RGFW_window* win);
RGFW_bool RGFW_window_allowsDND(RGFW_window* win);

#ifndef RGFW_WINDOWS
void RGFW_window_setDND(RGFW_window* win, RGFW_bool allow);
#endif

#if defined(RGFW_X11) || defined(RGFW_MACOS) || defined(RGFW_WASM)
#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
#endif
#include <time.h>
struct timespec;
#endif

#if defined(RGFW_X11) || defined(RGFW_WINDOWS)
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
#endif

#ifdef RGFW_MACOS
#include <objc/message.h>
#endif

const char** RGFW_getVKRequiredInstanceExtensions(size_t* count);

VkResult RGFW_window_createVKSurface(RGFW_window* win, VkInstance instance, VkSurfaceKHR* surface);

#if defined(RGFW_X11) && !defined(RGFW_NO_LINUX)
	inline int RGFW_eventWait_forceStop[3]; /* for wait events */

	#if defined(__linux__)
		#include <linux/joystick.h>
		#include <fcntl.h>
		#include <unistd.h>
		#include <errno.h>

		u32 RGFW_linux_updateGamepad(RGFW_window* win);

	#endif
#endif

#ifdef RGFW_UNIX
#if !defined(RGFW_NO_X11_CURSOR) && defined(RGFW_X11)
#include <X11/Xcursor/Xcursor.h>
#endif

#include <dlfcn.h>

#ifndef RGFW_NO_DPI
#include <X11/extensions/Xrandr.h>
#include <X11/Xresource.h>
#endif

#include <X11/Xatom.h>
#include <X11/keysymdef.h>
#include <X11/extensions/sync.h>
#include <unistd.h>

#include <X11/XKBlib.h> /* for converting keycode to string */
#include <X11/cursorfont.h> /* for hiding */
#include <X11/extensions/shapeconst.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XInput2.h>

#include <limits.h> /* for data limits (mainly used in drag and drop functions) */
#include <poll.h>

/* atoms needed for drag and drop */
inline Atom XdndAware, XtextPlain, XtextUriList;
inline Atom RGFW_XUTF8_STRING;

inline Atom wm_delete_window, RGFW_XCLIPBOARD;

#if !defined(RGFW_NO_X11_CURSOR) && !defined(RGFW_NO_X11_CURSOR_PRELOAD)
	typedef XcursorImage* (*PFN_XcursorImageCreate)(int, int);
	typedef void (*PFN_XcursorImageDestroy)(XcursorImage*);
	typedef Cursor(*PFN_XcursorImageLoadCursor)(Display*, const XcursorImage*);
#endif

#if !defined(RGFW_NO_X11_XI_PRELOAD)
	typedef int (* PFN_XISelectEvents)(Display*,Window,XIEventMask*,int);
	inline PFN_XISelectEvents XISelectEventsSRC;
	#define XISelectEvents XISelectEventsSRC

	inline void* X11Xihandle;
#endif

#if !defined(RGFW_NO_X11_EXT_PRELOAD)
	typedef void (* PFN_XSyncIntToValue)(XSyncValue*, int);
	inline PFN_XSyncIntToValue XSyncIntToValueSRC;
    #define XSyncIntToValue XSyncIntToValueSRC

	typedef Status (* PFN_XSyncSetCounter)(Display*, XSyncCounter, XSyncValue);
	inline PFN_XSyncSetCounter XSyncSetCounterSRC;
    #define XSyncSetCounter XSyncSetCounterSRC

	typedef XSyncCounter (* PFN_XSyncCreateCounter)(Display*, XSyncValue);
	inline PFN_XSyncCreateCounter XSyncCreateCounterSRC;
    #define XSyncCreateCounter XSyncCreateCounterSRC

	typedef void (* PFN_XShapeCombineMask)(Display*,Window,int,int,int,Pixmap,int);
	inline PFN_XShapeCombineMask XShapeCombineMaskSRC;
    #define XShapeCombineMask XShapeCombineMaskSRC

	typedef void (* PFN_XShapeCombineRegion)(Display*,Window,int,int,int,Region,int);
    inline PFN_XShapeCombineRegion XShapeCombineRegionSRC;
    #define XShapeCombineRegion XShapeCombineRegionSRC
	inline void* X11XEXThandle;
#endif

#if !defined(RGFW_NO_X11_CURSOR) && !defined(RGFW_NO_X11_CURSOR_PRELOAD)
	inline PFN_XcursorImageLoadCursor XcursorImageLoadCursorSRC;
	inline PFN_XcursorImageCreate XcursorImageCreateSRC;
	inline PFN_XcursorImageDestroy XcursorImageDestroySRC;

	#define XcursorImageLoadCursor XcursorImageLoadCursorSRC
	#define XcursorImageCreate XcursorImageCreateSRC
	#define XcursorImageDestroy XcursorImageDestroySRC

	inline void* X11Cursorhandle;
#endif

inline const char* RGFW_instName;
void RGFW_setXInstName(const char* name);

#define RGFW_LOAD_ATOM(name) Atom name = XInternAtom(_RGFW.display, #name, False);
#define RGFW_LOAD_LIBRARY(x, lib) if (x == NULL) x = dlopen(lib, RTLD_LAZY | RTLD_LOCAL)
#define RGFW_PROC_DEF(proc, name) if (name##SRC == NULL && proc != NULL) { \
	void* ptr = dlsym(proc, #name); \
	if (ptr != NULL) memcpy(&name##SRC, &ptr, sizeof(PFN_##name)); \
}

void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
void RGFW_releaseCursor(RGFW_window* win);
void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);
void RGFW_window_getVisual(RGFW_window* win, RGFW_bool software);

i32 RGFW_init(void);

RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);
RGFW_area RGFW_getScreenSize(void);
RGFW_point RGFW_getGlobalMousePoint(void);
void RGFW_XHandleClipboardSelection(XEvent* event);
char* RGFW_strtok(char* str, const char* delimStr);
i32 RGFW_XHandleClipboardSelectionHelper(void);
RGFW_event* RGFW_window_checkEvent(RGFW_window* win);
void RGFW_window_move(RGFW_window* win, RGFW_point v);
void RGFW_window_resize(RGFW_window* win, RGFW_area a);
void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
void RGFW_window_setMaxSize(RGFW_window* win, RGFW_area a);
void RGFW_toggleXMaximized(RGFW_window* win, RGFW_bool maximized);
void RGFW_window_maximize(RGFW_window* win);
void RGFW_window_focus(RGFW_window* win);
void RGFW_window_raise(RGFW_window* win);
void RGFW_window_setXAtom(RGFW_window* win, Atom netAtom, RGFW_bool fullscreen);
void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen);
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating);
void RGFW_window_setOpacity(RGFW_window* win, u8 opacity);
void RGFW_window_minimize(RGFW_window* win);
void RGFW_window_restore(RGFW_window* win);
RGFW_bool RGFW_window_isFloating(RGFW_window* win);
void RGFW_window_setName(RGFW_window* win, const char* name);

#ifndef RGFW_NO_PASSTHROUGH
void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
#endif /* RGFW_NO_PASSTHROUGH */

RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* icon, RGFW_area a, i32 channels, u8 type);
RGFW_mouse* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);
void RGFW_freeMouse(RGFW_mouse* mouse);
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point p);
RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win);
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse);
void RGFW_window_hide(RGFW_window* win);
void RGFW_window_show(RGFW_window* win);

RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);

i32 RGFW_XHandleClipboardSelectionHelper(void);
void RGFW_writeClipboard(const char* text, u32 textLen);
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);

#ifndef RGFW_NO_DPI
u32 RGFW_XCalculateRefreshRate(XRRModeInfo mi);
#endif

static float XGetSystemContentDPI(Display* display, i32 screen);

RGFW_monitor RGFW_XCreateMonitor(i32 screen);
RGFW_monitor* RGFW_getMonitors(size_t* len);
RGFW_monitor RGFW_getPrimaryMonitor(void);
RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);

void RGFW_window_swapBuffers_software(RGFW_window* win);

void RGFW_deinit(void);
void RGFW_window_close(RGFW_window* win);

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

void RGFW_stopCheckEvents(void);
void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);

i32 RGFW_getClock(void);
u64 RGFW_getTimerFreq(void);
#endif /* end of wayland or X11 defines */

#ifdef RGFW_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE

#include <windows.h>
#include <processthreadsapi.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <wchar.h>
#include <locale.h>
#include <winuser.h>

#ifndef WM_DPICHANGED
#define WM_DPICHANGED       0x02E0
#endif

#ifndef RGFW_NO_XINPUT
	typedef DWORD (WINAPI * PFN_XInputGetState)(DWORD,XINPUT_STATE*);
	inline PFN_XInputGetState XInputGetStateSRC = NULL;
	#define XInputGetState XInputGetStateSRC

	typedef DWORD (WINAPI * PFN_XInputGetKeystroke)(DWORD, DWORD, PXINPUT_KEYSTROKE);
	inline PFN_XInputGetKeystroke XInputGetKeystrokeSRC = NULL;
	#define XInputGetKeystroke XInputGetKeystrokeSRC

	inline HMODULE RGFW_XInput_dll = NULL;
#endif

char* RGFW_createUTF8FromWideStringWin32(const WCHAR* source);

#define GL_FRONT				0x0404
#define GL_BACK					0x0405
#define GL_LEFT					0x0406
#define GL_RIGHT				0x0407

typedef int (*PFN_wglGetSwapIntervalEXT)(void);
inline PFN_wglGetSwapIntervalEXT wglGetSwapIntervalEXTSrc = NULL;
#define wglGetSwapIntervalEXT wglGetSwapIntervalEXTSrc

inline void* RGFWgamepadApi = NULL;

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hdc, HGLRC hglrc, const int *attribList);
inline PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
inline HMODULE RGFW_wgl_dll = NULL;

#ifndef RGFW_NO_LOAD_WGL
	typedef HGLRC(WINAPI* PFN_wglCreateContext)(HDC);
	typedef BOOL(WINAPI* PFN_wglDeleteContext)(HGLRC);
	typedef PROC(WINAPI* PFN_wglGetProcAddress)(LPCSTR);
	typedef BOOL(WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);
	typedef HDC(WINAPI* PFN_wglGetCurrentDC)(void);
	typedef HGLRC(WINAPI* PFN_wglGetCurrentContext)(void);
	typedef BOOL(WINAPI* PFN_wglShareLists)(HGLRC, HGLRC);
	inline PFN_wglCreateContext wglCreateContextSRC;
	inline PFN_wglDeleteContext wglDeleteContextSRC;
	inline PFN_wglGetProcAddress wglGetProcAddressSRC;
	inline PFN_wglMakeCurrent wglMakeCurrentSRC;
	inline PFN_wglGetCurrentDC wglGetCurrentDCSRC;
	inline PFN_wglGetCurrentContext wglGetCurrentContextSRC;
	inline PFN_wglShareLists wglShareListsSRC;
	#define wglCreateContext wglCreateContextSRC
	#define wglDeleteContext wglDeleteContextSRC
	#define wglGetProcAddress wglGetProcAddressSRC
	#define wglMakeCurrent wglMakeCurrentSRC
	#define wglGetCurrentDC wglGetCurrentDCSRC
	#define wglGetCurrentContext wglGetCurrentContextSRC
	#define wglShareLists wglShareListsSRC
#endif

#ifndef RGFW_NO_DWM
inline HMODULE RGFW_dwm_dll = NULL;
typedef struct { DWORD dwFlags; int fEnable; HRGN hRgnBlur; int fTransitionOnMaximized;} DWM_BLURBEHIND;
typedef HRESULT (WINAPI * PFN_DwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND*);
inline PFN_DwmEnableBlurBehindWindow DwmEnableBlurBehindWindowSRC = NULL;
#endif
void RGFW_win32_makeWindowTransparent(RGFW_window* win);

LRESULT CALLBACK WndProcW(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifndef RGFW_NO_DPI
	inline HMODULE RGFW_Shcore_dll = NULL;
	typedef HRESULT (WINAPI *PFN_GetDpiForMonitor)(HMONITOR,MONITOR_DPI_TYPE,UINT*,UINT*);
	inline PFN_GetDpiForMonitor GetDpiForMonitorSRC = NULL;
	#define GetDpiForMonitor GetDpiForMonitorSRC
#endif

#if !defined(RGFW_NO_LOAD_WINMM) && !defined(RGFW_NO_WINMM)
	inline HMODULE RGFW_winmm_dll = NULL;
	typedef u32 (WINAPI * PFN_timeBeginPeriod)(u32);
	typedef PFN_timeBeginPeriod PFN_timeEndPeriod;
	inline PFN_timeBeginPeriod timeBeginPeriodSRC, timeEndPeriodSRC;
	#define timeBeginPeriod timeBeginPeriodSRC
	#define timeEndPeriod timeEndPeriodSRC
#elif !defined(RGFW_NO_WINMM)
	__declspec(dllimport) u32 __stdcall timeBeginPeriod(u32 uPeriod);
	__declspec(dllimport) u32 __stdcall timeEndPeriod(u32 uPeriod);
#endif
#define RGFW_PROC_DEF(proc, name) if (name##SRC == NULL && proc != NULL) name##SRC = (PFN_##name)(RGFW_proc)GetProcAddress((proc), (#name));

#ifndef RGFW_NO_XINPUT
void RGFW_loadXInput(void);
#endif

void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);
void RGFW_releaseCursor(RGFW_window* win);
void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);

#define RGFW_LOAD_LIBRARY(x, lib) if (x == NULL) x = LoadLibraryA(lib)

#ifdef RGFW_DIRECTX
int RGFW_window_createDXSwapChain(RGFW_window* win, IDXGIFactory* pFactory, IUnknown* pDevice, IDXGISwapChain** swapchain);
#endif

i32 RGFW_init(void);

RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
void RGFW_window_setDND(RGFW_window* win, RGFW_bool allow);
RGFW_area RGFW_getScreenSize(void);
RGFW_point RGFW_getGlobalMousePoint(void);
void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
void RGFW_window_setMaxSize(RGFW_window* win, RGFW_area a);
void RGFW_window_focus(RGFW_window* win);
void RGFW_window_raise(RGFW_window* win);
void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen);
void RGFW_window_maximize(RGFW_window* win);
void RGFW_window_minimize(RGFW_window* win);
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating);
void RGFW_window_setOpacity(RGFW_window* win, u8 opacity);
void RGFW_window_restore(RGFW_window* win);
RGFW_bool RGFW_window_isFloating(RGFW_window* win);

inline u8 RGFW_xinput2RGFW[] = {
	RGFW_gamepadA, /* or PS X button */
	RGFW_gamepadB, /* or PS circle button */
	RGFW_gamepadX, /* or PS square button */
	RGFW_gamepadY, /* or PS triangle button */
	RGFW_gamepadR1, /* right bumper */
	RGFW_gamepadL1, /* left bump */
	RGFW_gamepadL2, /* left trigger */
	RGFW_gamepadR2, /* right trigger */
	0, 0, 0, 0, 0, 0, 0, 0,
	RGFW_gamepadUp, /* dpad up */
	RGFW_gamepadDown, /* dpad down */
	RGFW_gamepadLeft, /* dpad left */
	RGFW_gamepadRight, /* dpad right */
	RGFW_gamepadStart, /* start button */
	RGFW_gamepadSelect,/* select button */
	RGFW_gamepadL3,
	RGFW_gamepadR3,
};

i32 RGFW_checkXInput(RGFW_window* win, RGFW_event* e);
void RGFW_stopCheckEvents(void);
void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);
RGFW_event* RGFW_window_checkEvent(RGFW_window* win);
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);

typedef struct { int iIndex; HMONITOR hMonitor; RGFW_monitor* monitors; } RGFW_mInfo;
#ifndef RGFW_NO_MONITOR
RGFW_monitor win32CreateMonitor(HMONITOR src);
#endif /* RGFW_NO_MONITOR */

#ifndef RGFW_NO_MONITOR
BOOL CALLBACK GetMonitorHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

RGFW_monitor RGFW_getPrimaryMonitor(void);
RGFW_monitor* RGFW_getMonitors(size_t* len);
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);
RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);

#endif
HICON RGFW_loadHandleImage(u8* src, i32 c, RGFW_area a, BOOL icon);

void* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);
void RGFW_freeMouse(RGFW_mouse* mouse);

RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win);
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse);

void RGFW_window_hide(RGFW_window* win);
void RGFW_window_show(RGFW_window* win);

#define RGFW_FREE_LIBRARY(x) if (x != NULL) FreeLibrary(x); x = NULL;
void RGFW_deinit(void);
void RGFW_window_close(RGFW_window* win);
void RGFW_window_move(RGFW_window* win, RGFW_point v);
void RGFW_window_resize(RGFW_window* win, RGFW_area a);
void RGFW_window_setName(RGFW_window* win, const char* name);

#ifndef RGFW_NO_PASSTHROUGH
void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
#endif

RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* src, RGFW_area a, i32 channels, u8 type);
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);

void RGFW_writeClipboard(const char* text, u32 textLen);
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point p);
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);

void RGFW_window_swapBuffers_software(RGFW_window* win);
char* RGFW_createUTF8FromWideStringWin32(const WCHAR* source);
u64 RGFW_getTimerFreq(void);
u64 RGFW_getTimerValue(void);
void RGFW_sleep(u64 ms) ;

#ifndef RGFW_NO_THREADS

RGFW_thread RGFW_createThread(RGFW_threadFunc_ptr ptr, void* args);
void RGFW_cancelThread(RGFW_thread thread);
void RGFW_joinThread(RGFW_thread thread);
void RGFW_setThreadPriority(RGFW_thread thread, u8 priority);

#endif
#endif /* RGFW_WINDOWS */

#if defined(RGFW_MACOS)

#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include <mach/mach_time.h>
#include <CoreVideo/CoreVideo.h>

typedef CGRect NSRect;
typedef CGPoint NSPoint;
typedef CGSize NSSize;
typedef const char* NSPasteboardType;
typedef unsigned long NSUInteger;
typedef long NSInteger;
typedef NSInteger NSModalResponse;

#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#define abi_objc_msgSend_fpret objc_msgSend
#else /* __i386__ */
#define abi_objc_msgSend_stret objc_msgSend_stret
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#endif

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))
#define objc_msgSend_bool(x, y)			((BOOL (*)(id, SEL))objc_msgSend) ((id)(x), (SEL)y)
#define objc_msgSend_void(x, y)			((void (*)(id, SEL))objc_msgSend) ((id)(x), (SEL)y)
#define objc_msgSend_void_id(x, y, z)		((void (*)(id, SEL, id))objc_msgSend) ((id)x, (SEL)y, (id)z)
#define objc_msgSend_uint(x, y)			((NSUInteger (*)(id, SEL))objc_msgSend)  ((id)(x), (SEL)y)
#define objc_msgSend_void_bool(x, y, z)		((void (*)(id, SEL, BOOL))objc_msgSend)  ((id)(x), (SEL)y, (BOOL)z)
#define objc_msgSend_bool_void(x, y)		((BOOL (*)(id, SEL))objc_msgSend)  ((id)(x), (SEL)y)
#define objc_msgSend_void_SEL(x, y, z)		((void (*)(id, SEL, SEL))objc_msgSend)  ((id)(x), (SEL)y, (SEL)z)
#define objc_msgSend_id(x, y)				((id (*)(id, SEL))objc_msgSend)  ((id)(x), (SEL)y)
#define objc_msgSend_id_id(x, y, z)			((id (*)(id, SEL, id))objc_msgSend)  ((id)(x), (SEL)y, (id)z)
#define objc_msgSend_id_bool(x, y, z)			((BOOL (*)(id, SEL, id))objc_msgSend)  ((id)(x), (SEL)y, (id)z)
#define objc_msgSend_int(x, y, z) 				((id (*)(id, SEL, int))objc_msgSend)  ((id)(x), (SEL)y, (int)z)
#define objc_msgSend_arr(x, y, z)				 	((id (*)(id, SEL, int))objc_msgSend)  ((id)(x), (SEL)y, (int)z)
#define objc_msgSend_ptr(x, y, z) 					((id (*)(id, SEL, void*))objc_msgSend)  ((id)(x), (SEL)y, (void*)z)
#define objc_msgSend_class(x, y) 					((id (*)(Class, SEL))objc_msgSend)  ((Class)(x), (SEL)y)
#define objc_msgSend_class_char(x, y, z) 			((id (*)(Class, SEL, char*))objc_msgSend)  ((Class)(x), (SEL)y, (char*)z)

inline id NSApp = NULL;

#define NSRelease(obj) objc_msgSend_void((id)obj, sel_registerName("release"))
id NSString_stringWithUTF8String(const char* str);

const char* NSString_to_char(id str);
void si_impl_func_to_SEL_with_name(const char* class_name, const char* register_name, void* function);

typedef struct siArrayHeader {
	size_t count;
	/* TODO(EimaMei): Add a `type_width` later on. */
} siArrayHeader;

#define SI_ARRAY_HEADER(s) ((siArrayHeader*)s - 1)
#define si_array_len(array) (SI_ARRAY_HEADER(array)->count)
#define si_func_to_SEL(class_name, function) si_impl_func_to_SEL_with_name(class_name, #function":", (void*)function)
#define si_func_to_SEL_with_name(class_name, register_name, function) si_impl_func_to_SEL_with_name(class_name, register_name":", (void*)function)

unsigned char* NSBitmapImageRep_bitmapData(id imageRep);

typedef RGFW_ENUM(NSUInteger, NSBitmapFormat) {
	NSBitmapFormatAlphaFirst = 1 << 0,       /* 0 means is alpha last (RGBA, CMYKA, etc.) */
		NSBitmapFormatAlphaNonpremultiplied = 1 << 1,       /* 0 means is premultiplied */
		NSBitmapFormatFloatingpointSamples = 1 << 2,  /* 0 is integer */
		NSBitmapFormatSixteenBitLittleEndian API_AVAILABLE(macos(10.10)) = (1 << 8),
		NSBitmapFormatThirtyTwoBitLittleEndian API_AVAILABLE(macos(10.10)) = (1 << 9),
		NSBitmapFormatSixteenBitBigEndian API_AVAILABLE(macos(10.10)) = (1 << 10),
		NSBitmapFormatThirtyTwoBitBigEndian API_AVAILABLE(macos(10.10)) = (1 << 11)
};

id NSBitmapImageRep_initWithBitmapData(unsigned char** planes, NSInteger width, NSInteger height, NSInteger bps, NSInteger spp, bool alpha, bool isPlanar, const char* colorSpaceName, NSBitmapFormat bitmapFormat, NSInteger rowBytes, NSInteger pixelBits);
id NSColor_colorWithSRGB(CGFloat red, CGFloat green, CGFloat blue, CGFloat alpha);

typedef RGFW_ENUM(NSInteger, NSWindowButton) {
    NSWindowCloseButton            = 0,
    NSWindowMiniaturizeButton      = 1,
    NSWindowZoomButton             = 2,
    NSWindowToolbarButton          = 3,
    NSWindowDocumentIconButton     = 4,
    NSWindowDocumentVersionsButton = 6,
    NSWindowFullScreenButton       = 7,
};

id NSPasteboard_generalPasteboard(void);
id* cstrToNSStringArray(char** strs, size_t len);
const char* NSPasteboard_stringForType(id pasteboard, NSPasteboardType dataType, size_t* len);
id c_array_to_NSArray(void* array, size_t len);
void NSregisterForDraggedTypes(id view, NSPasteboardType* newTypes, size_t len);
NSInteger NSPasteBoard_declareTypes(id pasteboard, NSPasteboardType* newTypes, size_t len, void* owner);

#define NSRetain(obj) objc_msgSend_void((id)obj, sel_registerName("retain"))

typedef enum NSApplicationActivationPolicy {
	NSApplicationActivationPolicyRegular,
	NSApplicationActivationPolicyAccessory,
	NSApplicationActivationPolicyProhibited
} NSApplicationActivationPolicy;

typedef RGFW_ENUM(u32, NSBackingStoreType) {
	NSBackingStoreRetained = 0,
		NSBackingStoreNonretained = 1,
		NSBackingStoreBuffered = 2
};

typedef RGFW_ENUM(u32, NSWindowStyleMask) {
	NSWindowStyleMaskBorderless = 0,
		NSWindowStyleMaskTitled = 1 << 0,
		NSWindowStyleMaskClosable = 1 << 1,
		NSWindowStyleMaskMiniaturizable = 1 << 2,
		NSWindowStyleMaskResizable = 1 << 3,
		NSWindowStyleMaskTexturedBackground = 1 << 8, /* deprecated */
		NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
		NSWindowStyleMaskFullScreen = 1 << 14,
		NSWindowStyleMaskFullSizeContentView = 1 << 15,
		NSWindowStyleMaskUtilityWindow = 1 << 4,
		NSWindowStyleMaskDocModalWindow = 1 << 6,
		NSWindowStyleMaskNonactivatingpanel = 1 << 7,
		NSWindowStyleMaskHUDWindow = 1 << 13
};

inline NSPasteboardType const NSPasteboardTypeString = "public.utf8-plain-text"; /* Replaces NSStringPasteboardType */

typedef RGFW_ENUM(i32, NSDragOperation) {
	NSDragOperationNone = 0,
		NSDragOperationCopy = 1,
		NSDragOperationLink = 2,
		NSDragOperationGeneric = 4,
		NSDragOperationPrivate = 8,
		NSDragOperationMove = 16,
		NSDragOperationDelete = 32,
		NSDragOperationEvery = (int)ULONG_MAX
};

void* NSArray_objectAtIndex(id array, NSUInteger index);
id NSWindow_contentView(id window);

id NSWindow_delegate(RGFW_window* win);
u32 RGFW_OnClose(id self);
bool acceptsFirstResponder(void);
bool performKeyEquivalent(id event);
NSDragOperation draggingEntered(id self, SEL sel, id sender);
NSDragOperation draggingUpdated(id self, SEL sel, id sender);
bool prepareForDragOperation(id self);
void RGFW__osxDraggingEnded(id self, SEL sel, id sender);
bool performDragOperation(id self, SEL sel, id sender);

#ifndef RGFW_NO_IOKIT
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDManager.h>

inline IOHIDDeviceRef RGFW_osxControllers[4] = {NULL};
size_t findControllerIndex(IOHIDDeviceRef device);
void RGFW__osxInputValueChangedCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value);
void RGFW__osxDeviceAddedCallback(void* context, IOReturn result, void *sender, IOHIDDeviceRef device);
void RGFW__osxDeviceRemovedCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
void RGFW_osxInitIOKit(void);
void RGFW_osxInitIOKit(void);
#endif

void RGFW_moveToMacOSResourceDir(void);
void RGFW__osxWindowDeminiaturize(id self, SEL sel);
void RGFW__osxWindowMiniaturize(id self, SEL sel);
void RGFW__osxWindowBecameKey(id self, SEL sel);
void RGFW__osxWindowResignKey(id self, SEL sel);
NSSize RGFW__osxWindowResize(id self, SEL sel, NSSize frameSize);
void RGFW__osxWindowMove(id self, SEL sel);
void RGFW__osxViewDidChangeBackingProperties(id self, SEL _cmd);
void RGFW__osxDrawRect(id self, SEL _cmd, CGRect rect);
void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);
void RGFW_window_cocoaSetLayer(RGFW_window* win, void* layer);
void* RGFW_cocoaGetLayer(void);
inline NSPasteboardType const NSPasteboardTypeURL = "public.url";
inline NSPasteboardType const NSPasteboardTypeFileURL  = "public.file-url";
id RGFW__osx_generateViewClass(const char* subclass, RGFW_window* win);

i32 RGFW_init(void);

RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
RGFW_area RGFW_getScreenSize(void);
RGFW_point RGFW_getGlobalMousePoint(void);
typedef RGFW_ENUM(u32, NSEventType) {        /* various types of events */
	NSEventTypeLeftMouseDown = 1,
		NSEventTypeLeftMouseUp = 2,
		NSEventTypeRightMouseDown = 3,
		NSEventTypeRightMouseUp = 4,
		NSEventTypeMouseMoved = 5,
		NSEventTypeLeftMouseDragged = 6,
		NSEventTypeRightMouseDragged = 7,
		NSEventTypeMouseEntered = 8,
		NSEventTypeMouseExited = 9,
		NSEventTypeKeyDown = 10,
		NSEventTypeKeyUp = 11,
		NSEventTypeFlagsChanged = 12,
		NSEventTypeAppKitDefined = 13,
		NSEventTypeSystemDefined = 14,
		NSEventTypeApplicationDefined = 15,
		NSEventTypePeriodic = 16,
		NSEventTypeCursorUpdate = 17,
		NSEventTypeScrollWheel = 22,
		NSEventTypeTabletPoint = 23,
		NSEventTypeTabletProximity = 24,
		NSEventTypeOtherMouseDown = 25,
		NSEventTypeOtherMouseUp = 26,
		NSEventTypeOtherMouseDragged = 27,
		NSEventTypeGesture API_AVAILABLE(macos(10.5)) = 29,
		NSEventTypeMagnify API_AVAILABLE(macos(10.5)) = 30,
		NSEventTypeSwipe   API_AVAILABLE(macos(10.5)) = 31,
		NSEventTypeRotate  API_AVAILABLE(macos(10.5)) = 18,
		NSEventTypeBeginGesture API_AVAILABLE(macos(10.5)) = 19,
		NSEventTypeEndGesture API_AVAILABLE(macos(10.5)) = 20,
		NSEventTypeSmartMagnify API_AVAILABLE(macos(10.8)) = 32,
		NSEventTypeQuickLook API_AVAILABLE(macos(10.8)) = 33,
		NSEventTypePressure API_AVAILABLE(macos(10.10.3)) = 34,
		NSEventTypeDirectTouch API_AVAILABLE(macos(10.10)) = 37,
		NSEventTypeChangeMode API_AVAILABLE(macos(10.15)) = 38,
};

typedef unsigned long long NSEventMask;

typedef enum NSEventModifierFlags {
	NSEventModifierFlagCapsLock = 1 << 16,
	NSEventModifierFlagShift = 1 << 17,
	NSEventModifierFlagControl = 1 << 18,
	NSEventModifierFlagOption = 1 << 19,
	NSEventModifierFlagCommand = 1 << 20,
	NSEventModifierFlagNumericPad = 1 << 21
} NSEventModifierFlags;

void RGFW_stopCheckEvents(void);
void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);
RGFW_event* RGFW_window_checkEvent(RGFW_window* win);
void RGFW_window_move(RGFW_window* win, RGFW_point v);
void RGFW_window_resize(RGFW_window* win, RGFW_area a);
void RGFW_window_focus(RGFW_window* win);
void RGFW_window_raise(RGFW_window* win);
void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen);
void RGFW_window_maximize(RGFW_window* win);
void RGFW_window_minimize(RGFW_window* win);
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating);
void RGFW_window_setOpacity(RGFW_window* win, u8 opacity);
void RGFW_window_restore(RGFW_window* win);
RGFW_bool RGFW_window_isFloating(RGFW_window* win);
void RGFW_window_setName(RGFW_window* win, const char* name);

#ifndef RGFW_NO_PASSTHROUGH
void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
#endif

void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
void RGFW_window_setMaxSize(RGFW_window* win, RGFW_area a);
RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* data, RGFW_area area, i32 channels, u8 type);
id NSCursor_arrowStr(const char* str);
RGFW_mouse* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);
void RGFW_freeMouse(RGFW_mouse* mouse);
RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win);
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 stdMouses);
void RGFW_releaseCursor(RGFW_window* win);
void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point v);
void RGFW_window_hide(RGFW_window* win);
void RGFW_window_show(RGFW_window* win);
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);
id RGFW_getNSScreenForDisplayID(CGDirectDisplayID display);
u32 RGFW_osx_getRefreshRate(CGDirectDisplayID display, CGDisplayModeRef mode);
RGFW_monitor RGFW_NSCreateMonitor(CGDirectDisplayID display, id screen);
RGFW_monitor* RGFW_getMonitors(size_t* len);
RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);
RGFW_monitor RGFW_getPrimaryMonitor(void);
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);

void RGFW_writeClipboard(const char* text, u32 textLen);
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);

void RGFW_window_swapBuffers_software(RGFW_window* win);
void RGFW_deinit(void);
void RGFW_window_close(RGFW_window* win);
u64 RGFW_getTimerFreq(void);
u64 RGFW_getTimerValue(void);
#endif /* RGFW_MACOS */

#ifdef RGFW_WASM
EM_BOOL Emscripten_on_resize(int eventType, const EmscriptenUiEvent* e, void* userData);
EM_BOOL Emscripten_on_fullscreenchange(int eventType, const EmscriptenFullscreenChangeEvent* e, void* userData);
EM_BOOL Emscripten_on_focusin(int eventType, const EmscriptenFocusEvent* e, void* userData);
EM_BOOL Emscripten_on_focusout(int eventType, const EmscriptenFocusEvent* e, void* userData);
EM_BOOL Emscripten_on_mousemove(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL Emscripten_on_mousedown(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL Emscripten_on_mouseup(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL Emscripten_on_wheel(int eventType, const EmscriptenWheelEvent* e, void* userData);
EM_BOOL Emscripten_on_touchstart(int eventType, const EmscriptenTouchEvent* e, void* userData);
EM_BOOL Emscripten_on_touchmove(int eventType, const EmscriptenTouchEvent* e, void* userData);
EM_BOOL Emscripten_on_touchend(int eventType, const EmscriptenTouchEvent* e, void* userData);
EM_BOOL Emscripten_on_touchcancel(int eventType, const EmscriptenTouchEvent* e, void* userData);
EM_BOOL Emscripten_on_gamepad(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData);
u32 RGFW_wASMPhysicalToRGFW(u32 hash);

void EMSCRIPTEN_KEEPALIVE RGFW_handleKeyEvent(char* key, char* code, RGFW_bool press);
void EMSCRIPTEN_KEEPALIVE RGFW_handleKeyMods(RGFW_bool capital, RGFW_bool numlock, RGFW_bool control, RGFW_bool alt, RGFW_bool shift, RGFW_bool super, RGFW_bool scroll);
void EMSCRIPTEN_KEEPALIVE Emscripten_onDrop(size_t count);
inline RGFW_bool RGFW_stopCheckEvents_bool = RGFW_FALSE;
void RGFW_stopCheckEvents(void);
void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);
void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);
void EMSCRIPTEN_KEEPALIVE RGFW_makeSetValue(size_t index, char* file);

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

void EMSCRIPTEN_KEEPALIVE RGFW_mkdir(char* name) ;
void EMSCRIPTEN_KEEPALIVE RGFW_writeFile(const char *path, const char *data, size_t len);

i32 RGFW_init(void);

RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);
RGFW_event* RGFW_window_checkEvent(RGFW_window* win);
void RGFW_window_resize(RGFW_window* win, RGFW_area a);
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point v);
RGFW_mouse* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);
void RGFW_freeMouse(RGFW_mouse* mouse);
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse);
RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win);
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
RGFW_point RGFW_getGlobalMousePoint(void);
void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
void RGFW_writeClipboard(const char* text, u32 textLen);
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);
void RGFW_window_swapBuffers_software(RGFW_window* win);
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
void RGFW_deinit(void);
void RGFW_window_close(RGFW_window* win);
int RGFW_innerWidth(void);
int RGFW_innerHeight(void);
RGFW_area RGFW_getScreenSize(void);
RGFW_proc RGFW_getProcAddress(const char* procname);
void RGFW_sleep(u64 milisecond);
u64 RGFW_getTimerFreq(void);
u64 RGFW_getTimerValue(void);
void RGFW_releaseCursor(RGFW_window* win);
void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);
void RGFW_window_setName(RGFW_window* win, const char* name);
void RGFW_window_maximize(RGFW_window* win);
void RGFW_window_setFullscreen(RGFW_window* win, RGFW_bool fullscreen);
void RGFW_window_setOpacity(RGFW_window* win, u8 opacity);
void RGFW_window_focus(RGFW_window* win);
void RGFW_window_raise(RGFW_window* win);
RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);
RGFW_monitor* RGFW_getMonitors(size_t* len);
RGFW_monitor RGFW_getPrimaryMonitor(void);
void RGFW_window_move(RGFW_window* win, RGFW_point v);
void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
void RGFW_window_setMaxSize(RGFW_window* win, RGFW_area a);
void RGFW_window_minimize(RGFW_window* win);
void RGFW_window_restore(RGFW_window* win);
void RGFW_window_setFloating(RGFW_window* win, RGFW_bool floating);
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
RGFW_bool RGFW_window_setIconEx(RGFW_window* win, u8* icon, RGFW_area a, i32 channels, u8 type);
void RGFW_window_hide(RGFW_window* win);
void RGFW_window_show(RGFW_window* win);
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);
RGFW_bool RGFW_window_isFloating(RGFW_window* win);
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);
#endif

#if defined(RGFW_X11) || defined(RGFW_MACOS) || defined(RGFW_WASM)
#ifndef RGFW_NO_THREADS
#include <pthread.h>

RGFW_thread RGFW_createThread(RGFW_threadFunc_ptr ptr, void* args);
void RGFW_cancelThread(RGFW_thread thread);
void RGFW_joinThread(RGFW_thread thread);

#if defined(__linux__)
void RGFW_setThreadPriority(RGFW_thread thread, u8 priority);
#else
void RGFW_setThreadPriority(RGFW_thread thread, u8 priority);
#endif
#endif

#ifndef RGFW_WASM
void RGFW_sleep(u64 ms);
#endif
#endif /* end of unix / mac stuff */