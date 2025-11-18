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

#ifndef RGFW_H
#define RGFW_H

#if _MSC_VER
	#pragma comment(lib, "gdi32")
	#pragma comment(lib, "shell32")
	#pragma comment(lib, "User32")
    #pragma warning( push )
	#pragma warning( disable : 4996 4191 4127)
    #if _MSC_VER < 600
        #define RGFW_C89
    #endif
#else
    #if defined(__STDC__) && !defined(__STDC_VERSION__)
        #define RGFW_C89
    #endif
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
	/* required for X11 XDnD and X11 Monitor DPI */
	#include <stdlib.h>
    #define RGFW_STRTOL(str, endptr, base) strtol(str, endptr, base)
	#define RGFW_ATOF(num) atof(num)
#endif

#if !_MSC_VER
	#ifndef inline
		#ifndef __APPLE__
			#define inline __inline
		#endif
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


#if defined(__cplusplus) && !defined(__EMSCRIPTEN__)
	extern "C" {
#endif

	/* makes sure the header file part is only defined once by default */
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

/* these OS macros look better & are standardized */
/* plus it helps with cross-compiling */

#ifdef __EMSCRIPTEN__
	#define RGFW_WASM

	#if !defined(RGFW_NO_API) && !defined(RGFW_WEBGPU)
		#define RGFW_OPENGL
	#endif

	#ifdef RGFW_EGL
		#undef RGFW_EGL
	#endif

	#include <emscripten/html5.h>
	#include <emscripten/key_codes.h>

	#ifdef RGFW_WEBGPU
		#include <emscripten/html5_webgpu.h>
	#endif
#endif

#if defined(RGFW_X11) && defined(__APPLE__) && !defined(RGFW_CUSTOM_BACKEND)
	#define RGFW_MACOS_X11
	#define RGFW_UNIX
	#undef __APPLE__
#endif

#if defined(_WIN32) && !defined(RGFW_X11) && !defined(RGFW_UNIX) && !defined(RGFW_WASM) && !defined(RGFW_CUSTOM_BACKEND) /* (if you're using X11 on windows some how) */
	#define RGFW_WINDOWS
	/* make sure the correct architecture is defined */
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
#if defined(RGFW_WAYLAND)
	#define RGFW_DEBUG /* wayland will be in debug mode by default for now */
    #if !defined(RGFW_NO_API) && (!defined(RGFW_BUFFER) || defined(RGFW_OPENGL)) && !defined(RGFW_OSMESA)
		#define RGFW_EGL
		#define RGFW_OPENGL
		#define RGFW_UNIX
		#include <wayland-egl.h>
	#endif

	#include <wayland-client.h>
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

#if (defined(RGFW_OPENGL_ES1) || defined(RGFW_OPENGL_ES2) || defined(RGFW_OPENGL_ES3)) && !defined(RGFW_EGL)
	#define RGFW_EGL
#endif

#ifdef RGFW_EGL
	#include <EGL/egl.h>
#elif defined(RGFW_OSMESA)
	#ifdef RGFW_WINDOWS
	#define OEMRESOURCE
	#include <GL/gl.h>
    	#ifndef GLAPIENTRY
	#define GLAPIENTRY APIENTRY
	#endif
    	#ifndef GLAPI
	#define GLAPI WINGDIAPI
    	#endif
	#endif

	#ifndef __APPLE__
		#include <GL/osmesa.h>
	#else
		#include <OpenGL/osmesa.h>
	#endif
#endif

#if (defined(RGFW_OPENGL) || defined(RGFW_WEGL)) && defined(_MSC_VER)
    #pragma comment(lib, "opengl32")
#endif

#if defined(RGFW_OPENGL) && defined(RGFW_X11)
	#ifndef GLX_MESA_swap_control
		#define  GLX_MESA_swap_control
	#endif
	#include <GL/glx.h> /* GLX defs, xlib.h, gl.h */
#endif

#define RGFW_COCOA_FRAME_NAME NULL

/*! (unix) Toggle use of wayland. This will be on by default if you use `RGFW_WAYLAND` (if you don't use RGFW_WAYLAND, you don't expose WAYLAND functions)
	this is mostly used to allow you to force the use of XWayland
*/
void RGFW_useWayland(RGFW_bool wayland);
RGFW_bool RGFW_usingWayland(void);
/*
	regular RGFW stuff
*/

#define RGFW_key u8

typedef RGFW_ENUM(u8, RGFW_eventType) {
	/*! event codes */
	RGFW_eventNone = 0, /*!< no event has been sent */
 	RGFW_keyPressed, /* a key has been pressed */
	RGFW_keyReleased, /*!< a key has been released */
	/*! key event note
		the code of the key pressed is stored in
		RGFW_event.key

		while a string version is stored in
		RGFW_event.KeyString

		RGFW_event.keyMod holds the current keyMod
		this means if CapsLock, NumLock are active or not
	*/
	RGFW_mouseButtonPressed, /*!< a mouse button has been pressed (left,middle,right) */
	RGFW_mouseButtonReleased, /*!< a mouse button has been released (left,middle,right) */
	RGFW_mousePosChanged, /*!< the position of the mouse has been changed */
	/*! mouse event note
		the x and y of the mouse can be found in the vector, RGFW_event.point

		RGFW_event.button holds which mouse button was pressed
	*/
	RGFW_gamepadConnected, /*!< a gamepad was connected */
	RGFW_gamepadDisconnected, /*!< a gamepad was disconnected */
	RGFW_gamepadButtonPressed, /*!< a gamepad button was pressed */
	RGFW_gamepadButtonReleased, /*!< a gamepad button was released */
	RGFW_gamepadAxisMove, /*!< an axis of a gamepad was moved */
	/*! gamepad event note
		RGFW_event.gamepad holds which gamepad was altered, if any
		RGFW_event.button holds which gamepad button was pressed

		RGFW_event.axis holds the data of all the axises
		RGFW_event.axisesCount says how many axises there are
	*/
	RGFW_windowMoved, /*!< the window was moved (by the user) */
	RGFW_windowResized, /*!< the window was resized (by the user), [on WASM this means the browser was resized] */
	RGFW_focusIn, /*!< window is in focus now */
	RGFW_focusOut, /*!< window is out of focus now */
	RGFW_mouseEnter, /* mouse entered the window */
	RGFW_mouseLeave, /* mouse left the window */
	RGFW_windowRefresh, /* The window content needs to be refreshed */

	/* attribs change event note
		The event data is sent straight to the window structure
		with win->r.x, win->r.y, win->r.w and win->r.h
	*/
	RGFW_quit, /*!< the user clicked the quit button */
	RGFW_DND, /*!< a file has been dropped into the window */
	RGFW_DNDInit, /*!< the start of a dnd event, when the place where the file drop is known */
	/* dnd data note
		The x and y coords of the drop are stored in the vector RGFW_event.point

		RGFW_event.droppedFilesCount holds how many files were dropped

		This is also the size of the array which stores all the dropped file string,
		RGFW_event.droppedFiles
	*/
	RGFW_windowMaximized, /*!< the window was maximized */
	RGFW_windowMinimized, /*!< the window was minimized */
	RGFW_windowRestored, /*!< the window was restored */
	RGFW_scaleUpdated /*!< content scale factor changed */
};

/*! mouse button codes (RGFW_event.button) */
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

/* for RGFW_event.lockstate */
typedef RGFW_ENUM(u8, RGFW_keymod) {
	RGFW_modCapsLock = RGFW_BIT(0),
	RGFW_modNumLock  = RGFW_BIT(1),
	RGFW_modControl  = RGFW_BIT(2),
	RGFW_modAlt = RGFW_BIT(3),
	RGFW_modShift  = RGFW_BIT(4),
	RGFW_modSuper = RGFW_BIT(5),
	RGFW_modScrollLock = RGFW_BIT(6)
};

/*! gamepad button codes (based on xbox/playstation), you may need to change these values per controller */
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

/*! basic vector type, if there's not already a point/vector type of choice */
#ifndef RGFW_point
	typedef struct { i32 x, y; } RGFW_point;
#endif

/*! basic rect type, if there's not already a rect type of choice */
#ifndef RGFW_rect
	typedef struct { i32 x, y, w, h; } RGFW_rect;
#endif

/*! basic area type, if there's not already a area type of choice */
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
	/* monitor mode data | can be changed by the user (with functions)*/
	typedef struct RGFW_monitorMode {
		RGFW_area area; /*!< monitor workarea size */
		u32 refreshRate; /*!< monitor refresh rate */
		u8 red, blue, green;
	} RGFW_monitorMode;

	/*! structure for monitor data */
	typedef struct RGFW_monitor {
		i32 x, y; /*!< x - y of the monitor workarea */
		char name[128]; /*!< monitor name */
		float scaleX, scaleY; /*!< monitor content scale */
		float pixelRatio; /*!< pixel ratio for monitor (1.0 for regular, 2.0 for hiDPI)  */
		float physW, physH; /*!< monitor physical size in inches */

		RGFW_monitorMode mode;
	} RGFW_monitor;

	/*! get an array of all the monitors (max 6) */
	RGFW_monitor* RGFW_getMonitors(size_t* len);
	/*! get the primary monitor */
	RGFW_monitor RGFW_getPrimaryMonitor(void);

	typedef RGFW_ENUM(u8, RGFW_modeRequest) {
		RGFW_monitorScale = RGFW_BIT(0), /*!< scale the monitor size */
		RGFW_monitorRefresh = RGFW_BIT(1), /*!< change the refresh rate */
		RGFW_monitorRGB = RGFW_BIT(2), /*!< change the monitor RGB bits size */
		RGFW_monitorAll = RGFW_monitorScale | RGFW_monitorRefresh | RGFW_monitorRGB
	};

	/*! request a specific mode */
	RGFW_bool RGFW_monitor_requestMode(RGFW_monitor mon, RGFW_monitorMode mode, RGFW_modeRequest request);
	/*! check if 2 monitor modes are the same */
	RGFW_bool RGFW_monitorModeCompare(RGFW_monitorMode mon, RGFW_monitorMode mon2, RGFW_modeRequest request);
#endif

/* RGFW mouse loading */
typedef void RGFW_mouse;

/*!< loads mouse icon from bitmap (similar to RGFW_window_setIcon). Icon NOT resized by default */
RGFW_mouse* RGFW_loadMouse(u8* icon, RGFW_area a, i32 channels);
/*!< frees RGFW_mouse data */
void RGFW_freeMouse(RGFW_mouse* mouse);

/* NOTE: some parts of the data can represent different things based on the event (read comments in RGFW_event struct) */
/*! Event structure for checking/getting events */
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

	/*! drag and drop data */
	/* 260 max paths with a max length of 260 */
	char** droppedFiles; /*!< dropped files */
	size_t droppedFilesCount; /*!< house many files were dropped */

	void* _win; /*!< the window this event applies too (for event queue events) */
} RGFW_event;

/*! source data for the window (used by the APIs) */
#ifdef RGFW_WINDOWS
typedef struct RGFW_window_src {
	HWND window; /*!< source window */
	HDC hdc; /*!< source HDC */
	u32 hOffset; /*!< height offset for window */
	HICON hIconSmall, hIconBig; /*!< source window icons */
	#if (defined(RGFW_OPENGL)) && !defined(RGFW_OSMESA) && !defined(RGFW_EGL)
		HGLRC ctx; /*!< source graphics context */
	#elif defined(RGFW_OSMESA)
		OSMesaContext ctx;
	#elif defined(RGFW_EGL)
		EGLSurface EGL_surface;
		EGLDisplay EGL_display;
		EGLContext EGL_context;
	#endif

	#if defined(RGFW_OSMESA) || defined(RGFW_BUFFER)
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
	#if (defined(RGFW_OPENGL)) && !defined(RGFW_OSMESA) && !defined(RGFW_EGL)
		GLXContext ctx; /*!< source graphics context */
        GLXFBConfig bestFbc;
	#elif defined(RGFW_OSMESA)
		OSMesaContext ctx;
	#elif defined(RGFW_EGL)
		EGLSurface EGL_surface;
		EGLDisplay EGL_display;
		EGLContext EGL_context;
	#endif

	#if defined(RGFW_OSMESA) || defined(RGFW_BUFFER)
			XImage* bitmap;
	#endif
	GC gc;
	XVisualInfo visual;
    #ifdef RGFW_ADVANCED_SMOOTH_RESIZE
        i64 counter_value;
        XID counter;
    #endif
#endif /* RGFW_X11 */
#if defined(RGFW_WAYLAND)
	struct wl_display* wl_display;
	struct wl_surface* surface;
	struct wl_buffer* wl_buffer;
	struct wl_keyboard* keyboard;

	struct wl_compositor* compositor;
	struct xdg_surface* xdg_surface;
	struct xdg_toplevel* xdg_toplevel;
	struct zxdg_toplevel_decoration_v1* decoration;
	struct xdg_wm_base* xdg_wm_base;
	struct wl_shm* shm;
	struct wl_seat *seat;
	u8* buffer;
	#if defined(RGFW_EGL)
		struct wl_egl_window* eglWindow;
	#endif
	#if defined(RGFW_EGL) && !defined(RGFW_X11)
			EGLSurface EGL_surface;
			EGLDisplay EGL_display;
			EGLContext EGL_context;
	#elif defined(RGFW_OSMESA) && !defined(RGFW_X11)
		OSMesaContext ctx;
	#endif
#endif /* RGFW_WAYLAND */
} RGFW_window_src;
#endif /* RGFW_UNIX */
#if defined(RGFW_MACOS)
typedef struct RGFW_window_src {
	void* window;
#if (defined(RGFW_OPENGL)) && !defined(RGFW_OSMESA) && !defined(RGFW_EGL)
		void* ctx; /*!< source graphics context */
#elif defined(RGFW_OSMESA)
		OSMesaContext ctx;
#elif defined(RGFW_EGL)
		EGLSurface EGL_surface;
		EGLDisplay EGL_display;
		EGLContext EGL_context;
#endif

	void* view; /* apple viewpoint thingy */
	void* mouse;
#if defined(RGFW_OSMESA) || defined(RGFW_BUFFER)
#endif
} RGFW_window_src;
#elif defined(RGFW_WASM)
typedef struct RGFW_window_src {
	#if defined(RGFW_WEBGPU)
		WGPUInstance ctx;
        WGPUDevice device;
        WGPUQueue queue;
	#elif defined(RGFW_OSMESA)
		OSMesaContext ctx;
	#else
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
	#endif
} RGFW_window_src;
#endif

/*! Optional arguments for making a windows */
typedef RGFW_ENUM(u32, RGFW_windowFlags) {
	RGFW_windowNoInitAPI = RGFW_BIT(0), /* do NOT init an API (including the software rendering buffer) (mostly for bindings. you can also use `#define RGFW_NO_API`) */
	RGFW_windowNoBorder = RGFW_BIT(1), /*!< the window doesn't have a border */
	RGFW_windowNoResize = RGFW_BIT(2), /*!< the window cannot be resized by the user */
	RGFW_windowAllowDND = RGFW_BIT(3), /*!< the window supports drag and drop */
	RGFW_windowHideMouse = RGFW_BIT(4), /*! the window should hide the mouse (can be toggled later on using `RGFW_window_mouseShow`) */
	RGFW_windowFullscreen = RGFW_BIT(5), /*!< the window is fullscreen by default */
	RGFW_windowTransparent = RGFW_BIT(6), /*!< the window is transparent (only properly works on X11 and MacOS, although it's meant for for windows) */
	RGFW_windowCenter = RGFW_BIT(7), /*! center the window on the screen */
	RGFW_windowOpenglSoftware = RGFW_BIT(8), /*! use OpenGL software rendering */
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

#if defined(RGFW_OSMESA) || defined(RGFW_BUFFER)
	u8* buffer; /*!< buffer for non-GPU systems (OSMesa, basic software rendering) */
	/* when rendering using RGFW_BUFFER, the buffer is in the RGBA format */
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

/*! scale monitor to window size */
RGFW_bool RGFW_monitor_scaleToWindow(RGFW_monitor mon, RGFW_window* win);

/** * @defgroup Window_management
* @{ */


/*!
 * the class name for X11 and WinAPI. apps with the same class will be grouped by the WM
 * by default the class name will == the root window's name
*/
void RGFW_setClassName(const char* name);
void RGFW_setXInstName(const char* name); /*!< X11 instance name (window name will by used by default) */

/*! (cocoa only) change directory to resource folder */
void RGFW_moveToMacOSResourceDir(void);

/* NOTE: (windows) if the executable has an icon resource named RGFW_ICON, it will be set as the initial icon for the window */

RGFW_window* RGFW_createWindow(
	const char* name, /* name of the window */
	RGFW_rect rect, /* rect of window */
	RGFW_windowFlags flags /* extra arguments ((u32)0 means no flags used)*/
); /*!< function to create a window and struct */

RGFW_window* RGFW_createWindowPtr(
	const char* name, /* name of the window */
	RGFW_rect rect, /* rect of window */
	RGFW_windowFlags flags, /* extra arguments (NULL / (u32)0 means no flags used) */
	RGFW_window* win /* ptr to the window struct you want to use */
); /*!< function to create a window (without allocating a window struct) */

void RGFW_window_initBuffer(RGFW_window* win);
void RGFW_window_initBufferSize(RGFW_window* win, RGFW_area area);
void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);

/*! set the window flags (will undo flags if they don't match the old ones) */
void RGFW_window_setFlags(RGFW_window* win, RGFW_windowFlags);

/*! get the size of the screen to an area struct */
RGFW_area RGFW_getScreenSize(void);


/*!
	this function checks an *individual* event (and updates window structure attributes)
	this means, using this function without a while loop may cause event lag

	ex.

	while (RGFW_window_checkEvent(win) != NULL) [this keeps checking events until it reaches the last one]

	this function is optional if you choose to use event callbacks,
	although you still need some way to tell RGFW to process events eg. `RGFW_window_checkEvents`
*/

RGFW_event* RGFW_window_checkEvent(RGFW_window* win); /*!< check current event (returns a pointer to win->event or NULL if there is no event)*/

/*!
	for RGFW_window_eventWait and RGFW_window_checkEvents
	waitMS -> Allows the function to keep checking for events even after `RGFW_window_checkEvent == NULL`
			  if waitMS == 0, the loop will not wait for events
			  if waitMS > 0, the loop will wait that many miliseconds after there are no more events until it returns
			  if waitMS == -1 or waitMS == the max size of an unsigned 32-bit int, the loop will not return until it gets another event
*/
typedef RGFW_ENUM(i32, RGFW_eventWait) {
	RGFW_eventNoWait = 0,
	RGFW_eventWaitNext = -1
};

/*! sleep until RGFW gets an event or the timer ends (defined by OS) */
void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);

/*!
	check all the events until there are none left.
	This should only be used if you're using callbacks only
*/
void RGFW_window_checkEvents(RGFW_window* win, i32 waitMS);

/*!
	tell RGFW_window_eventWait to stop waiting (to be ran from another thread)
*/
void RGFW_stopCheckEvents(void);

/*! window managment functions */
void RGFW_window_close(RGFW_window* win); /*!< close the window and free leftover data */

/*! move a window to a given point */
void RGFW_window_move(RGFW_window* win,
	RGFW_point v /*!< new pos */
);

#ifndef RGFW_NO_MONITOR
	/*! move window to a specific monitor */
	void RGFW_window_moveToMonitor(RGFW_window* win, RGFW_monitor m /* monitor */);
#endif

/*! resize window to a current size/area */
void RGFW_window_resize(RGFW_window* win, /*!< source window */
	RGFW_area a /*!< new size */
);

/*! set window aspect ratio */
void RGFW_window_setAspectRatio(RGFW_window* win, RGFW_area a);
/*! set the minimum dimensions of a window */
void RGFW_window_setMinSize(RGFW_window* win, RGFW_area a);
/*! set the maximum dimensions of a window */
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

/*! if the window should have a border or not (borderless) based on bool value of `border` */
void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);
RGFW_bool RGFW_window_borderless(RGFW_window* win);

/*! turn on / off dnd (RGFW_windowAllowDND stil must be passed to the window)*/
void RGFW_window_setDND(RGFW_window* win, RGFW_bool allow);
/*! check if DND is allowed */
RGFW_bool RGFW_window_allowsDND(RGFW_window* win);


#ifndef RGFW_NO_PASSTHROUGH
	/*! turn on / off mouse passthrough */
	void RGFW_window_setMousePassthrough(RGFW_window* win, RGFW_bool passthrough);
#endif

/*! rename window to a given string */
void RGFW_window_setName(RGFW_window* win,
	const char* name
);

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

/*!< sets mouse to RGFW_mouse icon (loaded from a bitmap struct) */
void RGFW_window_setMouse(RGFW_window* win, RGFW_mouse* mouse);

/*!< sets the mouse to a standard API cursor  */
RGFW_bool RGFW_window_setMouseStandard(RGFW_window* win, u8 mouse);

RGFW_bool RGFW_window_setMouseDefault(RGFW_window* win); /*!< sets the mouse to the default mouse icon */
/*
	Locks cursor at the center of the window
	win->event.point becomes raw mouse movement data

	this is useful for a 3D camera
*/
void RGFW_window_mouseHold(RGFW_window* win, RGFW_area area);
/*! stop holding the mouse and let it move freely */
void RGFW_window_mouseUnhold(RGFW_window* win);

/*! hide the window */
void RGFW_window_hide(RGFW_window* win);
/*! show the window */
void RGFW_window_show(RGFW_window* win);

/*
	makes it so `RGFW_window_shouldClose` returns true or overrides a window close
	by modifying window flags
*/
void RGFW_window_setShouldClose(RGFW_window* win, RGFW_bool shouldClose);

/*! where the mouse is on the screen */
RGFW_point RGFW_getGlobalMousePoint(void);

/*! where the mouse is on the window */
RGFW_point RGFW_window_getMousePoint(RGFW_window* win);

/*! show the mouse or hide the mouse */
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
/*! if the mouse is hidden */
RGFW_bool RGFW_window_mouseHidden(RGFW_window* win);
/*! move the mouse to a given point */
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point v);

/*! if the window should close (RGFW_close was sent or escape was pressed) */
RGFW_bool RGFW_window_shouldClose(RGFW_window* win);
/*! if the window is fullscreen */
RGFW_bool RGFW_window_isFullscreen(RGFW_window* win);
/*! if the window is hidden */
RGFW_bool RGFW_window_isHidden(RGFW_window* win);
/*! if the window is minimized */
RGFW_bool RGFW_window_isMinimized(RGFW_window* win);
/*! if the window is maximized */
RGFW_bool RGFW_window_isMaximized(RGFW_window* win);
/*! if the window is floating */
RGFW_bool RGFW_window_isFloating(RGFW_window* win);
/** @} */

/** * @defgroup Monitor
* @{ */

#ifndef RGFW_NO_MONITOR
/*
	scale the window to the monitor.
	This is run by default if the user uses the arg `RGFW_scaleToMonitor` during window creation
*/
void RGFW_window_scaleToMonitor(RGFW_window* win);
/*! get the struct of the window's monitor  */
RGFW_monitor RGFW_window_getMonitor(RGFW_window* win);
#endif

/** @} */

/** * @defgroup Input
* @{ */

/*! if window == NULL, it checks if the key is pressed globally. Otherwise, it checks only if the key is pressed while the window in focus. */
RGFW_bool RGFW_isPressed(RGFW_window* win, RGFW_key key); /*!< if key is pressed (key code)*/

RGFW_bool RGFW_wasPressed(RGFW_window* win, RGFW_key key); /*!< if key was pressed (checks previous state only) (key code) */

RGFW_bool RGFW_isHeld(RGFW_window* win, RGFW_key key); /*!< if key is held (key code) */
RGFW_bool RGFW_isReleased(RGFW_window* win, RGFW_key key); /*!< if key is released (key code) */

/* if a key is pressed and then released, pretty much the same as RGFW_isReleased */
RGFW_bool RGFW_isClicked(RGFW_window* win, RGFW_key key /*!< key code */);

/*! if a mouse button is pressed */
RGFW_bool RGFW_isMousePressed(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
/*! if a mouse button is held */
RGFW_bool RGFW_isMouseHeld(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
/*! if a mouse button was released */
RGFW_bool RGFW_isMouseReleased(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
/*! if a mouse button was pressed (checks previous state only) */
RGFW_bool RGFW_wasMousePressed(RGFW_window* win, RGFW_mouseButton button /*!< mouse button code */ );
/** @} */

/** * @defgroup Clipboard
* @{ */
typedef ptrdiff_t RGFW_ssize_t;

const char* RGFW_readClipboard(size_t* size); /*!< read clipboard data */
/*! read clipboard data or send a NULL str to just get the length of the clipboard data */
RGFW_ssize_t RGFW_readClipboardPtr(char* str, size_t strCapacity);
void RGFW_writeClipboard(const char* text, u32 textLen); /*!< write text to the clipboard */
/** @} */



/** * @defgroup error handling
* @{ */
typedef RGFW_ENUM(u8, RGFW_debugType) {
	RGFW_typeError = 0, RGFW_typeWarning, RGFW_typeInfo
};

typedef RGFW_ENUM(u8, RGFW_errorCode) {
	RGFW_noError = 0, /*!< no error */
	RGFW_errOpenglContext, RGFW_errEGLContext, /*!< error with the OpenGL context */
	RGFW_errWayland,
	RGFW_errDirectXContext,
	RGFW_errIOKit,
	RGFW_errClipboard,
	RGFW_errFailedFuncLoad,
	RGFW_errBuffer,
	RGFW_infoMonitor, RGFW_infoWindow, RGFW_infoBuffer, RGFW_infoGlobal, RGFW_infoOpenGL,
	RGFW_warningWayland, RGFW_warningOpenGL
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
/** @} */

/**


	event callbacks.
	These are completely optional, so you can use the normal
	RGFW_checkEvent() method if you prefer that

* @defgroup Callbacks
* @{
*/

/*! RGFW_windowMoved, the window and its new rect value  */
typedef void (* RGFW_windowMovedfunc)(RGFW_window* win, RGFW_rect r);
/*! RGFW_windowResized, the window and its new rect value  */
typedef void (* RGFW_windowResizedfunc)(RGFW_window* win, RGFW_rect r);
/*! RGFW_windowRestored, the window and its new rect value  */
typedef void (* RGFW_windowRestoredfunc)(RGFW_window* win, RGFW_rect r);
/*! RGFW_windowMaximized, the window and its new rect value  */
typedef void (* RGFW_windowMaximizedfunc)(RGFW_window* win, RGFW_rect r);
/*! RGFW_windowMinimized, the window and its new rect value  */
typedef void (* RGFW_windowMinimizedfunc)(RGFW_window* win, RGFW_rect r);
/*! RGFW_quit, the window that was closed */
typedef void (* RGFW_windowQuitfunc)(RGFW_window* win);
/*! RGFW_focusIn / RGFW_focusOut, the window who's focus has changed and if its in focus */
typedef void (* RGFW_focusfunc)(RGFW_window* win, RGFW_bool inFocus);
/*! RGFW_mouseEnter / RGFW_mouseLeave, the window that changed, the point of the mouse (enter only) and if the mouse has entered */
typedef void (* RGFW_mouseNotifyfunc)(RGFW_window* win, RGFW_point point, RGFW_bool status);
/*! RGFW_mousePosChanged, the window that the move happened on, and the new point of the mouse  */
typedef void (* RGFW_mousePosfunc)(RGFW_window* win, RGFW_point point, RGFW_point vector);
/*! RGFW_DNDInit, the window, the point of the drop on the windows */
typedef void (* RGFW_dndInitfunc)(RGFW_window* win, RGFW_point point);
/*! RGFW_windowRefresh, the window that needs to be refreshed */
typedef void (* RGFW_windowRefreshfunc)(RGFW_window* win);
/*! RGFW_keyPressed / RGFW_keyReleased, the window that got the event, the mapped key, the physical key, the string version, the state of the mod keys, if it was a press (else it's a release) */
typedef void (* RGFW_keyfunc)(RGFW_window* win, u8 key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool pressed);
/*! RGFW_mouseButtonPressed / RGFW_mouseButtonReleased, the window that got the event, the button that was pressed, the scroll value, if it was a press (else it's a release)  */
typedef void (* RGFW_mouseButtonfunc)(RGFW_window* win, RGFW_mouseButton button, double scroll, RGFW_bool pressed);
/*! RGFW_gamepadButtonPressed, the window that got the event, the button that was pressed, the scroll value, if it was a press (else it's a release) */
typedef void (* RGFW_gamepadButtonfunc)(RGFW_window* win, u16 gamepad, u8 button, RGFW_bool pressed);
/*! RGFW_gamepadAxisMove, the window that got the event, the gamepad in question, the axis values and the axis count */
typedef void (* RGFW_gamepadAxisfunc)(RGFW_window* win, u16 gamepad, RGFW_point axis[2], u8 axisesCount, u8 whichAxis);
/*! RGFW_gamepadConnected / RGFW_gamepadDisconnected, the window that got the event, the gamepad in question, if the controller was connected (else it was disconnected) */
typedef void (* RGFW_gamepadfunc)(RGFW_window* win, u16 gamepad, RGFW_bool connected);
/*! RGFW_dnd, the window that had the drop, the drop data and the number of files dropped */
typedef void (* RGFW_dndfunc)(RGFW_window* win, char** droppedFiles, size_t droppedFilesCount);
/*! RGFW_scaleUpdated, the window the event was sent to, content scaleX, content scaleY */
typedef void (* RGFW_scaleUpdatedfunc)(RGFW_window* win, float scaleX, float scaleY);

/*! set callback for a window move event. Returns previous callback function (if it was set)  */
RGFW_windowMovedfunc RGFW_setWindowMovedCallback(RGFW_windowMovedfunc func);
/*! set callback for a window resize event. Returns previous callback function (if it was set)  */
RGFW_windowResizedfunc RGFW_setWindowResizedCallback(RGFW_windowResizedfunc func);
/*! set callback for a window quit event. Returns previous callback function (if it was set)  */
RGFW_windowQuitfunc RGFW_setWindowQuitCallback(RGFW_windowQuitfunc func);
/*! set callback for a mouse move event. Returns previous callback function (if it was set)  */
RGFW_mousePosfunc RGFW_setMousePosCallback(RGFW_mousePosfunc func);
/*! set callback for a window refresh event. Returns previous callback function (if it was set)  */
RGFW_windowRefreshfunc RGFW_setWindowRefreshCallback(RGFW_windowRefreshfunc func);
/*! set callback for a window focus change event. Returns previous callback function (if it was set)  */
RGFW_focusfunc RGFW_setFocusCallback(RGFW_focusfunc func);
/*! set callback for a mouse notify event. Returns previous callback function (if it was set)  */
RGFW_mouseNotifyfunc RGFW_setMouseNotifyCallback(RGFW_mouseNotifyfunc func);
/*! set callback for a drop event event. Returns previous callback function (if it was set)  */
RGFW_dndfunc RGFW_setDndCallback(RGFW_dndfunc func);
/*! set callback for a start of a drop event. Returns previous callback function (if it was set)  */
RGFW_dndInitfunc RGFW_setDndInitCallback(RGFW_dndInitfunc func);
/*! set callback for a key (press / release) event. Returns previous callback function (if it was set)  */
RGFW_keyfunc RGFW_setKeyCallback(RGFW_keyfunc func);
/*! set callback for a mouse button (press / release) event. Returns previous callback function (if it was set)  */
RGFW_mouseButtonfunc RGFW_setMouseButtonCallback(RGFW_mouseButtonfunc func);
/*! set callback for a controller button (press / release) event. Returns previous callback function (if it was set)  */
RGFW_gamepadButtonfunc RGFW_setGamepadButtonCallback(RGFW_gamepadButtonfunc func);
/*! set callback for a gamepad axis move event. Returns previous callback function (if it was set)  */
RGFW_gamepadAxisfunc RGFW_setGamepadAxisCallback(RGFW_gamepadAxisfunc func);
/*! set callback for when a controller is connected or disconnected. Returns the previous callback function (if it was set) */
RGFW_gamepadfunc RGFW_setGamepadCallback(RGFW_gamepadfunc func);
/*! set call back for when window is maximized. Returns the previous callback function (if it was set) */
RGFW_windowResizedfunc RGFW_setWindowMaximizedCallback(RGFW_windowResizedfunc func);
/*! set call back for when window is minimized. Returns the previous callback function (if it was set) */
RGFW_windowResizedfunc RGFW_setWindowMinimizedCallback(RGFW_windowResizedfunc func);
/*! set call back for when window is restored. Returns the previous callback function (if it was set) */
RGFW_windowResizedfunc RGFW_setWindowRestoredCallback(RGFW_windowResizedfunc func);
/*! set callback for when the DPI changes. Returns previous callback function (if it was set)  */
RGFW_scaleUpdatedfunc RGFW_setScaleUpdatedCallback(RGFW_scaleUpdatedfunc func);
/** @} */

/** * @defgroup Threads
* @{ */

#ifndef RGFW_NO_THREADS
/*! threading functions */

/*! NOTE! (for X11/linux) : if you define a window in a thread, it must be run after the original thread's window is created or else there will be a memory error */
/*
	I'd suggest you use sili's threading functions instead
	if you're going to use sili
	which is a good idea generally
*/

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

/** @} */

/** * @defgroup gamepad
* @{ */

typedef RGFW_ENUM(u8, RGFW_gamepadType) {
	RGFW_gamepadMicrosoft = 0, RGFW_gamepadSony, RGFW_gamepadNintendo, RGFW_gamepadLogitech, RGFW_gamepadUnknown
};

/*! gamepad count starts at 0*/
u32 RGFW_isPressedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_isReleasedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_isHeldGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
u32 RGFW_wasPressedGamepad(RGFW_window* win, u8 controller, RGFW_gamepadCodes button);
RGFW_point RGFW_getGamepadAxis(RGFW_window* win, u16 controller, u16 whichAxis);
const char* RGFW_getGamepadName(RGFW_window* win, u16 controller);
size_t RGFW_getGamepadCount(RGFW_window* win);
RGFW_gamepadType RGFW_getGamepadType(RGFW_window* win, u16 controller);

/** @} */

/** * @defgroup graphics_API
* @{ */

/*!< make the window the current opengl drawing context

	NOTE:
 	if you want to switch the graphics context's thread,
	you have to run RGFW_window_makeCurrent(NULL); on the old thread
	then RGFW_window_makeCurrent(valid_window) on the new thread
*/
void RGFW_window_makeCurrent(RGFW_window* win);

/*! get current RGFW window graphics context */
RGFW_window* RGFW_getCurrent(void);

/* supports openGL, directX, OSMesa, EGL and software rendering */
void RGFW_window_swapBuffers(RGFW_window* win); /*!< swap the rendering buffer */
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
/*!< render the software rendering buffer (this is called by RGFW_window_swapInterval)  */
void RGFW_window_swapBuffers_software(RGFW_window* win);

typedef void (*RGFW_proc)(void); /* function pointer equivalent of void* */

/*! native API functions */
#if defined(RGFW_OPENGL) || defined(RGFW_EGL)
/*!< create an opengl context for the RGFW window, run by createWindow by default (unless the RGFW_windowNoInitAPI is included) */
void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);
/*!< called by `RGFW_window_close` by default (unless the RGFW_windowNoInitAPI is set) */
void RGFW_window_freeOpenGL(RGFW_window* win);

/*! OpenGL init hints */
typedef RGFW_ENUM(u8, RGFW_glHints)  {
	RGFW_glStencil = 0,  /*!< set stencil buffer bit size (8 by default) */
	RGFW_glSamples, /*!< set number of sampiling buffers (4 by default) */
	RGFW_glStereo, /*!< use GL_STEREO (GL_FALSE by default) */
	RGFW_glAuxBuffers, /*!< number of aux buffers (0 by default) */
	RGFW_glDoubleBuffer, /*!< request double buffering */
	RGFW_glRed, RGFW_glGreen, RGFW_glBlue, RGFW_glAlpha, /*!< set RGBA bit sizes */
	RGFW_glDepth,
	RGFW_glAccumRed, RGFW_glAccumGreen, RGFW_glAccumBlue,RGFW_glAccumAlpha, /*!< set accumulated RGBA bit sizes */
	RGFW_glSRGB, /*!< request sRGA */
	RGFW_glRobustness, /*!< request a robust context */
	RGFW_glDebug, /*!< request opengl debugging */
	RGFW_glNoError, /*!< request no opengl errors */
	RGFW_glReleaseBehavior,
	RGFW_glProfile,
	RGFW_glMajor, RGFW_glMinor,
	RGFW_glFinalHint = 32, /*!< the final hint (not for setting) */
	RGFW_releaseFlush = 0,  RGFW_glReleaseNone, /* RGFW_glReleaseBehavior options */
	RGFW_glCore = 0,  RGFW_glCompatibility /*!< RGFW_glProfile options */
};
void RGFW_setGLHint(RGFW_glHints hint, i32 value);
RGFW_proc RGFW_getProcAddress(const char* procname); /*!< get native opengl proc address */
void RGFW_window_makeCurrent_OpenGL(RGFW_window* win); /*!< to be called by RGFW_window_makeCurrent */
void RGFW_window_swapBuffers_OpenGL(RGFW_window* win); /*!< swap opengl buffer (only) called by RGFW_window_swapInterval  */
void* RGFW_getCurrent_OpenGL(void); /*!< get the current context (OpenGL backend (GLX) (WGL) (EGL) (cocoa) (webgl))*/
#endif
	#if defined(RGFW_WAYLAND) && defined(RGFW_X11)
    	#define VK_USE_PLATFORM_WAYLAND_KHR
		#define VK_USE_PLATFORM_XLIB_KHR
        #define RGFW_VK_SURFACE ((RGFW_usingWayland()) ? ("VK_KHR_wayland_surface") : ("VK_KHR_xlib_surface"))
    #elif defined(RGFW_WAYLAND)
		#define VK_USE_PLATFORM_WAYLAND_KHR
		#define VK_USE_PLATFORM_XLIB_KHR
        #define RGFW_VK_SURFACE "VK_KHR_wayland_surface"
    #elif defined(RGFW_X11)
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

/* if you don't want to use the above macros */
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

/** @} */

/** * @defgroup Supporting
* @{ */

/*! optional init/deinit function */
i32 RGFW_init(void); /*!< is called by default when the first window is created by default */
void RGFW_deinit(void); /*!< is called by default when the last open window is closed */

double RGFW_getTime(void); /*!< get time in seconds since RGFW_setTime, which ran when the first window is open  */
u64 RGFW_getTimeNS(void); /*!< get time in nanoseconds RGFW_setTime, which ran when the first window is open */
void RGFW_sleep(u64 milisecond); /*!< sleep for a set time */
void RGFW_setTime(double time); /*!< set timer in seconds */
u64 RGFW_getTimerValue(void); /*!< get API timer value */
u64 RGFW_getTimerFreq(void); /*!< get API time freq */

/*< updates fps / sets fps to cap (must by ran manually by the user at the end of a frame), returns current fps */
u32 RGFW_checkFPS(double startTime, u32 frameCount, u32 fpsCap);

/*!< change which window is the root window */
void RGFW_setRootWindow(RGFW_window* win);
RGFW_window* RGFW_getRootWindow(void);

/*! standard event queue, used for injecting events and returning source API callback events like any other queue check */
/* these are all used internally by RGFW */
void RGFW_eventQueuePush(RGFW_event event);
RGFW_event* RGFW_eventQueuePop(RGFW_window* win);

/*!
	key codes and mouse icon enums
*/
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

/** @} */

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

/*

This is the start of keycode data

	Why not use macros instead of the numbers itself?
	Windows -> Not all scancodes keys are macros
	Linux -> Only symcodes are values, (XK_0 - XK_1, XK_a - XK_z) are larger than 0xFF00, I can't find any way to work with them without making the array an unreasonable size
	MacOS -> windows and linux already don't have keycodes as macros, so there's no point
*/



/*
	the c++ compiler doesn't support setting up an array like,
	we'll have to do it during runtime using a function & this messy setup
*/

#ifndef RGFW_CUSTOM_BACKEND

#if defined(RGFW_X11) || defined(RGFW_WAYLAND)
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

/*
	this is the end of keycode data
*/

/* gamepad data */
inline RGFW_keyState RGFW_gamepadPressed[4][32]; /*!< if a key is currently pressed or not (per gamepad) */
inline RGFW_point RGFW_gamepadAxes[4][4]; /*!< if a key is currently pressed or not (per gamepad) */

inline RGFW_gamepadType RGFW_gamepads_type[4]; /*!< if a key is currently pressed or not (per gamepad) */
inline i32 RGFW_gamepads[4]; /*!< limit of 4 gamepads at a time */
inline char RGFW_gamepads_name[4][128]; /*!< gamepad names */
inline u16 RGFW_gamepadCount ; /*!< the actual amount of gamepads */

/*
	event callback defines start here
*/


/*
	These exist to avoid the
	if (func == NULL) check
	for (allegedly) better performance

	RGFW_EMPTY_DEF exists to prevent the missing-prototypes warning
*/
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

/*
no more event call back defines
*/

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
    #ifdef RGFW_WAYLAND
	    struct wl_display* wl_display;
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

/* do a basic initialization for RGFW_window, this is to standard it for each OS */
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

#if defined(RGFW_OSMESA) || defined(RGFW_BUFFER)
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

#if defined(RGFW_X11) || defined(RGFW_MACOS) || defined(RGFW_WASM) || defined(RGFW_WAYLAND)
#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
#endif
#include <time.h>
struct timespec;
#endif

#if defined(RGFW_X11) || defined(RGFW_WINDOWS)
void RGFW_window_showMouse(RGFW_window* win, RGFW_bool show);
#endif

/*
	graphics API specific code (end of generic code)
	starts here
*/


/*
	OpenGL defines start here   (Normal, EGL, OSMesa)
*/

#if defined(RGFW_OPENGL) || defined(RGFW_EGL)

#ifdef RGFW_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define OEMRESOURCE
	#include <windows.h>
#endif

#if !defined(__APPLE__) && !defined(RGFW_NO_GL_HEADER)
	#include <GL/gl.h>
#elif defined(__APPLE__)
	#ifndef GL_SILENCE_DEPRECATION
		#define GL_SILENCE_DEPRECATION
	#endif
	#include <OpenGL/gl.h>
	#include <OpenGL/OpenGL.h>
#endif

/* EGL, normal OpenGL only */
#ifndef RGFW_EGL
inline i32 RGFW_GL_HINTS[RGFW_glFinalHint];
#else
inline i32 RGFW_GL_HINTS[RGFW_glFinalHint];
#endif

void RGFW_setGLHint(RGFW_glHints hint, i32 value);

/* OPENGL normal only (no EGL / OSMesa) */
#if defined(RGFW_OPENGL) && !defined(RGFW_EGL) && !defined(RGFW_CUSTOM_BACKEND) && !defined(RGFW_WASM)

#define RGFW_GL_RENDER_TYPE 		RGFW_OS_BASED_VALUE(GLX_X_VISUAL_TYPE,    	0x2003,		73, 0)
	#define RGFW_GL_ALPHA_SIZE 		RGFW_OS_BASED_VALUE(GLX_ALPHA_SIZE,       	0x201b,		11,     0)
	#define RGFW_GL_DEPTH_SIZE 		RGFW_OS_BASED_VALUE(GLX_DEPTH_SIZE,       	0x2022,		12,     0)
	#define RGFW_GL_DOUBLEBUFFER 		RGFW_OS_BASED_VALUE(GLX_DOUBLEBUFFER,     	0x2011, 	5,  0)
	#define RGFW_GL_STENCIL_SIZE 		RGFW_OS_BASED_VALUE(GLX_STENCIL_SIZE,	 	0x2023,	13,     0)
	#define RGFW_GL_SAMPLES			RGFW_OS_BASED_VALUE(GLX_SAMPLES, 		 	0x2042,	    55,     0)
	#define RGFW_GL_STEREO 			RGFW_OS_BASED_VALUE(GLX_STEREO,	 		 	0x2012,			6,  0)
	#define RGFW_GL_AUX_BUFFERS		RGFW_OS_BASED_VALUE(GLX_AUX_BUFFERS,	    0x2024,	7, 		    0)

#if defined(RGFW_X11) || defined(RGFW_WINDOWS)
	#define RGFW_GL_DRAW 			RGFW_OS_BASED_VALUE(GLX_X_RENDERABLE,	 	0x2001,					0, 0)
	#define RGFW_GL_DRAW_TYPE 		RGFW_OS_BASED_VALUE(GLX_RENDER_TYPE,     	0x2013,						0, 0)
	#define RGFW_GL_FULL_FORMAT		RGFW_OS_BASED_VALUE(GLX_TRUE_COLOR,   	 	0x2027,						0, 0)
	#define RGFW_GL_RED_SIZE		RGFW_OS_BASED_VALUE(GLX_RED_SIZE,         	0x2015,						0, 0)
	#define RGFW_GL_GREEN_SIZE		RGFW_OS_BASED_VALUE(GLX_GREEN_SIZE,       	0x2017,						0, 0)
	#define RGFW_GL_BLUE_SIZE		RGFW_OS_BASED_VALUE(GLX_BLUE_SIZE, 	 		0x2019,						0, 0)
	#define RGFW_GL_USE_RGBA		RGFW_OS_BASED_VALUE(GLX_RGBA_BIT,   	 	0x202B,						0, 0)
	#define RGFW_GL_ACCUM_RED_SIZE 	RGFW_OS_BASED_VALUE(14,   	 	0x201E,						0, 0)
	#define RGFW_GL_ACCUM_GREEN_SIZE RGFW_OS_BASED_VALUE(15,   	 	0x201F,						0, 0)
	#define RGFW_GL_ACCUM_BLUE_SIZE	 RGFW_OS_BASED_VALUE(16,   	 	0x2020,						0, 0)
	#define RGFW_GL_ACCUM_ALPHA_SIZE	 RGFW_OS_BASED_VALUE(17,   	 	0x2021,						0, 0)
	#define RGFW_GL_SRGB	 RGFW_OS_BASED_VALUE(0x20b2,   	 	0x3089,						0, 0)
	#define RGFW_GL_NOERROR	 RGFW_OS_BASED_VALUE(0x31b3,   	 	0x31b3,						0, 0)
	#define RGFW_GL_FLAGS	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_FLAGS_ARB,   	 	0x2094,						0, 0)
	#define RGFW_GL_RELEASE_BEHAVIOR	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_RELEASE_BEHAVIOR_ARB,   	 	0x2097 ,						0, 0)
	#define RGFW_GL_CONTEXT_RELEASE	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB,   	 	0x2098,						0, 0)
	#define RGFW_GL_CONTEXT_NONE	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB,   	 	0x0000,						0, 0)
	#define RGFW_GL_FLAGS	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_FLAGS_ARB,   	 	0x2094,						0, 0)
	#define RGFW_GL_DEBUG_BIT	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_FLAGS_ARB,   	 	0x2094,						0, 0)
	#define RGFW_GL_ROBUST_BIT	 RGFW_OS_BASED_VALUE(GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB,   	 	0x00000004,						0, 0)
#endif

#ifdef RGFW_WINDOWS
	#define WGL_SUPPORT_OPENGL_ARB                    0x2010
	#define WGL_COLOR_BITS_ARB                        0x2014
	#define WGL_NUMBER_PIXEL_FORMATS_ARB 			0x2000
	#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
	#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
	#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
	#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
	#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
	#define WGL_SAMPLE_BUFFERS_ARB               0x2041
	#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
	#define WGL_PIXEL_TYPE_ARB                        0x2013
	#define WGL_TYPE_RGBA_ARB                         0x202B

	#define WGL_TRANSPARENT_ARB   					  0x200A
#endif

/*  The window'ing api needs to know how to render the data we (or opengl) give it
	MacOS and Windows do this using a structure called a "pixel format"
	X11 calls it a "Visual"
	This function returns the attributes for the format we want */
i32* RGFW_initFormatAttribs(u32 useSoftware);

/* EGL only (no OSMesa nor normal OPENGL) */
#elif defined(RGFW_EGL)

#include <EGL/egl.h>

#if defined(RGFW_LINK_EGL)
	typedef EGLBoolean(EGLAPIENTRY* PFN_eglInitialize)(EGLDisplay, EGLint*, EGLint*);

	PFNEGLINITIALIZEPROC eglInitializeSource;
	PFNEGLGETCONFIGSPROC eglGetConfigsSource;
	PFNEGLCHOOSECONFIgamepadROC eglChooseConfigSource;
	PFNEGLCREATEWINDOWSURFACEPROC eglCreateWindowSurfaceSource;
	PFNEGLCREATECONTEXTPROC eglCreateContextSource;
	PFNEGLMAKECURRENTPROC eglMakeCurrentSource;
	PFNEGLGETDISPLAYPROC eglGetDisplaySource;
	PFNEGLSWAPBUFFERSPROC eglSwapBuffersSource;
	PFNEGLSWAPINTERVALPROC eglSwapIntervalSource;
	PFNEGLBINDAPIPROC eglBindAPISource;
	PFNEGLDESTROYCONTEXTPROC eglDestroyContextSource;
	PFNEGLTERMINATEPROC eglTerminateSource;
	PFNEGLDESTROYSURFACEPROC eglDestroySurfaceSource;

	#define eglInitialize eglInitializeSource
	#define eglGetConfigs eglGetConfigsSource
	#define eglChooseConfig eglChooseConfigSource
	#define eglCreateWindowSurface eglCreateWindowSurfaceSource
	#define eglCreateContext eglCreateContextSource
	#define eglMakeCurrent eglMakeCurrentSource
	#define eglGetDisplay eglGetDisplaySource
	#define eglSwapBuffers eglSwapBuffersSource
	#define eglSwapInterval eglSwapIntervalSource
	#define eglBindAPI eglBindAPISource
	#define eglDestroyContext eglDestroyContextSource
	#define eglTerminate eglTerminateSource
	#define eglDestroySurface eglDestroySurfaceSource;
#endif


#define EGL_SURFACE_MAJOR_VERSION_KHR 0x3098
#define EGL_SURFACE_MINOR_VERSION_KHR 0x30fb

#ifndef RGFW_GL_ADD_ATTRIB
#define RGFW_GL_ADD_ATTRIB(attrib, attVal) \
	if (attVal) { \
		attribs[index] = attrib;\
		attribs[index + 1] = attVal;\
		index += 2;\
	}
#endif


void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);

void RGFW_window_freeOpenGL(RGFW_window* win);

void RGFW_window_makeCurrent_OpenGL(RGFW_window* win);

void RGFW_window_swapBuffers_OpenGL(RGFW_window* win);

void* RGFW_getCurrent_OpenGL(void);

#ifdef RGFW_APPLE
inline void* RGFWnsglFramework = NULL;
#elif defined(RGFW_WINDOWS)
inline HMODULE RGFW_wgl_dll = NULL;
#endif

RGFW_proc RGFW_getProcAddress(const char* procname);

void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);

#endif /* RGFW_EGL */

/*
	end of RGFW_EGL defines
*/
#endif /* end of RGFW_GL (OpenGL, EGL, OSMesa )*/

#ifdef RGFW_MACOS
#include <objc/message.h>
#endif

const char** RGFW_getVKRequiredInstanceExtensions(size_t* count);

VkResult RGFW_window_createVKSurface(RGFW_window* win, VkInstance instance, VkSurfaceKHR* surface);


/*
This is where OS specific stuff starts
*/


#if (defined(RGFW_WAYLAND) || defined(RGFW_X11)) && !defined(RGFW_NO_LINUX)
	inline int RGFW_eventWait_forceStop[3]; /* for wait events */

	#if defined(__linux__)
		#include <linux/joystick.h>
		#include <fcntl.h>
		#include <unistd.h>
		#include <errno.h>

		u32 RGFW_linux_updateGamepad(RGFW_window* win);

	#endif
#endif



/*

	Start of Wayland defines


*/

#ifdef RGFW_WAYLAND
/*
Wayland TODO: (out of date)
- fix RGFW_keyPressed lock state

	RGFW_windowMoved, 		the window was moved (by the user)
	RGFW_windowResized  	the window was resized (by the user), [on WASM this means the browser was resized]
	RGFW_windowRefresh	 	The window content needs to be refreshed

	RGFW_DND 				a file has been dropped into the window
	RGFW_DNDInit

- window args:
	#define RGFW_windowNoResize	 			the window cannot be resized  by the user
	#define RGFW_windowAllowDND     			the window supports drag and drop
	#define RGFW_scaleToMonitor 			scale the window to the screen

- other missing functions functions ("TODO wayland") (~30 functions)
- fix buffer rendering weird behavior
*/
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <dirent.h>
#include <linux/kd.h>
#include <wayland-cursor.h>

inline RGFW_window* RGFW_key_win = NULL;

/* wayland global garbage (wayland bad, X11 is fine (ish) (not really)) */
#include "xdg-shell.h"
#include "xdg-decoration-unstable-v1.h"

struct xkb_context *xkb_context;
struct xkb_keymap *keymap = NULL;
struct xkb_state *xkb_state = NULL;
enum zxdg_toplevel_decoration_v1_mode client_preferred_mode, RGFW_current_mode;
struct zxdg_decoration_manager_v1 *decoration_manager = NULL;

struct wl_cursor_theme* RGFW_wl_cursor_theme = NULL;
struct wl_surface* RGFW_cursor_surface = NULL;
struct wl_cursor_image* RGFW_cursor_image = NULL;

void xdg_wm_base_ping_handler(void *data,
        struct xdg_wm_base *wm_base, uint32_t serial);

const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

inline RGFW_bool RGFW_wl_configured = 0;

void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial);

const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_handler,
};

void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *toplevel, int32_t width, int32_t height,
        struct wl_array *states);

void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *toplevel);

void shm_format_handler(void *data,
        struct wl_shm *shm, uint32_t format);

const struct wl_shm_listener shm_listener = {
    .format = shm_format_handler,
};

const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

inline RGFW_window* RGFW_mouse_win = NULL;

void pointer_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
void pointer_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
void pointer_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
void pointer_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

void RGFW_doNothing(void) { } //??????

void keyboard_keymap (void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
void keyboard_enter (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
void keyboard_leave (void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
void keyboard_key (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
void keyboard_modifiers (void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
struct wl_keyboard_listener keyboard_listener = {&keyboard_keymap, &keyboard_enter, &keyboard_leave, &keyboard_key, &keyboard_modifiers, (void (*)(void *, struct wl_keyboard *, int, int))&RGFW_doNothing};

void seat_capabilities (void *data, struct wl_seat *seat, uint32_t capabilities);
struct wl_seat_listener seat_listener = {&seat_capabilities, (void (*)(void *, struct wl_seat *, const char *))&RGFW_doNothing};

void wl_global_registry_handler(void *data,
		struct wl_registry *registry, uint32_t id, const char *interface,
		uint32_t version);

void wl_global_registry_remove(void *data, struct wl_registry *registry, uint32_t name);
const struct wl_registry_listener registry_listener = {
	.global = wl_global_registry_handler,
	.global_remove = wl_global_registry_remove,
};

void decoration_handle_configure(void *data,
		struct zxdg_toplevel_decoration_v1 *decoration,
		unsigned int mode);

const struct zxdg_toplevel_decoration_v1_listener decoration_listener = {
	.configure = decoration_handle_configure,
};

void randname(char *buf);

size_t wl_stringlen(char* name);

int anonymous_shm_open(void);

int create_shm_file(off_t size);

void wl_surface_frame_done(void *data, struct wl_callback *cb, uint32_t time);

const struct wl_callback_listener wl_surface_frame_listener = {
	.done = wl_surface_frame_done,
};
#endif /* RGFW_WAYLAND */
/*
	End of Wayland defines
*/

/*


Start of Linux / Unix defines


*/

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
#ifdef RGFW_OPENGL
	typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
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

#if defined(RGFW_OPENGL) && !defined(RGFW_EGL)
RGFW_proc RGFW_getProcAddress(const char* procname);
#endif

void RGFW_window_initBufferPtr(RGFW_window* win, u8* buffer, RGFW_area area);

#define RGFW_LOAD_ATOM(name) Atom name = XInternAtom(_RGFW.display, #name, False);

void RGFW_window_setBorder(RGFW_window* win, RGFW_bool border);

void RGFW_releaseCursor(RGFW_window* win);

void RGFW_captureCursor(RGFW_window* win, RGFW_rect r);

#define RGFW_LOAD_LIBRARY(x, lib) if (x == NULL) x = dlopen(lib, RTLD_LAZY | RTLD_LOCAL)
#define RGFW_PROC_DEF(proc, name) if (name##SRC == NULL && proc != NULL) { \
	void* ptr = dlsym(proc, #name); \
	if (ptr != NULL) memcpy(&name##SRC, &ptr, sizeof(PFN_##name)); \
}

void RGFW_window_getVisual(RGFW_window* win, RGFW_bool software);

#ifndef RGFW_EGL
void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);
void RGFW_window_freeOpenGL(RGFW_window* win);
#endif


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

#if defined(RGFW_OPENGL) && !defined(RGFW_EGL)
void RGFW_window_makeCurrent_OpenGL(RGFW_window* win);
void* RGFW_getCurrent_OpenGL(void);
void RGFW_window_swapBuffers_OpenGL(RGFW_window* win);
#endif

void RGFW_window_swapBuffers_software(RGFW_window* win);
#if !defined(RGFW_EGL)

void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
#endif

void RGFW_deinit(void);

void RGFW_window_close(RGFW_window* win);


/*
	End of X11 linux / wayland / unix defines
*/

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

void RGFW_stopCheckEvents(void);

void RGFW_window_eventWait(RGFW_window* win, i32 waitMS);

i32 RGFW_getClock(void);

u64 RGFW_getTimerFreq(void);
#endif /* end of wayland or X11 defines */



/*

	Start of Windows defines


*/

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

/* these two wgl functions need to be preloaded */
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hdc, HGLRC hglrc, const int *attribList);
inline PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

#ifndef RGFW_EGL
	inline HMODULE RGFW_wgl_dll = NULL;
#endif

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

#ifdef RGFW_OPENGL

RGFW_proc RGFW_getProcAddress(const char* procname);

typedef HRESULT (APIENTRY* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
inline PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;

typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALEXTPROC)(int interval);
inline PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
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

void RGFW_win32_loadOpenGLFuncs(HWND dummyWin);

#ifndef RGFW_EGL
void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);

void RGFW_window_freeOpenGL(RGFW_window* win);
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

#ifdef RGFW_OPENGL
void RGFW_window_makeCurrent_OpenGL(RGFW_window* win);
void* RGFW_getCurrent_OpenGL(void);
void RGFW_window_swapBuffers_OpenGL(RGFW_window* win);
#endif

#ifndef RGFW_EGL
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
#endif

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

/*
	End of Windows defines
*/



/*

	Start of MacOS defines


*/

#if defined(RGFW_MACOS)
/*
	based on silicon.h
	start of cocoa wrapper
*/

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
	/* ARM just uses objc_msgSend */
#define abi_objc_msgSend_stret objc_msgSend
#define abi_objc_msgSend_fpret objc_msgSend
#else /* __i386__ */
	/* x86 just uses abi_objc_msgSend_fpret and (NSColor *)objc_msgSend_id respectively */
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

/* Header for the array. */
typedef struct siArrayHeader {
	size_t count;
	/* TODO(EimaMei): Add a `type_width` later on. */
} siArrayHeader;

/* Gets the header of the siArray. */
#define SI_ARRAY_HEADER(s) ((siArrayHeader*)s - 1)
#define si_array_len(array) (SI_ARRAY_HEADER(array)->count)
#define si_func_to_SEL(class_name, function) si_impl_func_to_SEL_with_name(class_name, #function":", (void*)function)
/* Creates an Objective-C method (SEL) from a regular C function with the option to set the register name.*/
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

#define NS_OPENGL_ENUM_DEPRECATED(minVers, maxVers) API_AVAILABLE(macos(minVers))
typedef RGFW_ENUM(NSInteger, NSOpenGLContextParameter) {
	NSOpenGLContextParameterSwapInterval           NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 222, /* 1 param.  0 -> Don't sync, 1 -> Sync to vertical retrace     */
		NSOpenGLContextParametectxaceOrder           NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 235, /* 1 param.  1 -> Above Window (default), -1 -> Below Window    */
		NSOpenGLContextParametectxaceOpacity         NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 236, /* 1 param.  1-> Surface is opaque (default), 0 -> non-opaque   */
		NSOpenGLContextParametectxaceBackingSize     NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 304, /* 2 params.  Width/height of surface backing size              */
		NSOpenGLContextParameterReclaimResources       NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 308, /* 0 params.                                                    */
		NSOpenGLContextParameterCurrentRendererID      NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 309, /* 1 param.   Retrieves the current renderer ID                 */
		NSOpenGLContextParameterGPUVertexProcessing    NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 310, /* 1 param.   Currently processing vertices with GPU (get)      */
		NSOpenGLContextParameterGPUFragmentProcessing  NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 311, /* 1 param.   Currently processing fragments with GPU (get)     */
		NSOpenGLContextParameterHasDrawable            NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 314, /* 1 param.   Boolean returned if drawable is attached          */
		NSOpenGLContextParameterMPSwapsInFlight        NS_OPENGL_ENUM_DEPRECATED(10.0, 10.14) = 315, /* 1 param.   Max number of swaps queued by the MP GL engine    */

		NSOpenGLContextParameterSwapRectangle API_DEPRECATED("", macos(10.0, 10.14)) = 200, /* 4 params.  Set or get the swap rectangle {x, y, w, h} */
		NSOpenGLContextParameterSwapRectangleEnable API_DEPRECATED("", macos(10.0, 10.14)) = 201, /* Enable or disable the swap rectangle */
		NSOpenGLContextParameterRasterizationEnable API_DEPRECATED("", macos(10.0, 10.14)) = 221, /* Enable or disable all rasterization */
		NSOpenGLContextParameterStateValidation API_DEPRECATED("", macos(10.0, 10.14)) = 301, /* Validate state for multi-screen functionality */
		NSOpenGLContextParametectxaceSurfaceVolatile API_DEPRECATED("", macos(10.0, 10.14)) = 306, /* 1 param.   Surface volatile state */
};

typedef RGFW_ENUM(NSInteger, NSWindowButton) {
    NSWindowCloseButton            = 0,
    NSWindowMiniaturizeButton      = 1,
    NSWindowZoomButton             = 2,
    NSWindowToolbarButton          = 3,
    NSWindowDocumentIconButton     = 4,
    NSWindowDocumentVersionsButton = 6,
    NSWindowFullScreenButton       = 7,
};
void NSOpenGLContext_setValues(id context, const int* vals, NSOpenGLContextParameter param);
void* NSOpenGLPixelFormat_initWithAttributes(const uint32_t* attribs);

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

/*
	End of cocoa wrapper
*/

#ifdef RGFW_OPENGL
inline CFBundleRef RGFWnsglFramework = NULL;

RGFW_proc RGFW_getProcAddress(const char* procname);
#endif

id NSWindow_delegate(RGFW_window* win);

u32 RGFW_OnClose(id self);

/* NOTE(EimaMei): Fixes the constant clicking when the app is running under a terminal. */
bool acceptsFirstResponder(void);
bool performKeyEquivalent(id event);

NSDragOperation draggingEntered(id self, SEL sel, id sender);
NSDragOperation draggingUpdated(id self, SEL sel, id sender);
bool prepareForDragOperation(id self);

void RGFW__osxDraggingEnded(id self, SEL sel, id sender);

/* NOTE(EimaMei): Usually, you never need 'id self, SEL cmd' for C -> Obj-C methods. This isn't the case. */
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

#ifndef RGFW_EGL
void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);

void RGFW_window_freeOpenGL(RGFW_window* win);
#endif


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
		/* The following event types are available on some hardware on 10.5.2 and later */
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
	#ifdef RGFW_OPENGL
	void RGFW_window_makeCurrent_OpenGL(RGFW_window* win);
	void* RGFW_getCurrent_OpenGL(void);

	void RGFW_window_swapBuffers_OpenGL(RGFW_window* win);
	#endif

	#if !defined(RGFW_EGL)

	void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);

	#endif

void RGFW_window_swapBuffers_software(RGFW_window* win);

void RGFW_deinit(void);

void RGFW_window_close(RGFW_window* win);

u64 RGFW_getTimerFreq(void);

u64 RGFW_getTimerValue(void);

#endif /* RGFW_MACOS */

/*
	End of MaOS defines
*/

/*
	WASM defines
*/

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

void RGFW_window_initOpenGL(RGFW_window* win, RGFW_bool software);

void RGFW_window_freeOpenGL(RGFW_window* win);

i32 RGFW_init(void);

RGFW_window* RGFW_createWindowPtr(const char* name, RGFW_rect rect, RGFW_windowFlags flags, RGFW_window* win);

RGFW_event* RGFW_window_checkEvent(RGFW_window* win);

void RGFW_window_resize(RGFW_window* win, RGFW_area a);

/* NOTE: I don't know if this is possible */
void RGFW_window_moveMouse(RGFW_window* win, RGFW_point v);
/* this one might be possible but it looks iffy */
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

void RGFW_window_makeCurrent_OpenGL(RGFW_window* win);


void RGFW_window_swapBuffers_OpenGL(RGFW_window* win);

#ifndef RGFW_WEBGPU
void* RGFW_getCurrent_OpenGL(void);
#endif

#ifndef RGFW_EGL
void RGFW_window_swapInterval(RGFW_window* win, i32 swapInterval);
#endif

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

/* unsupported functions */
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

/* end of web asm defines */

/* unix (macOS, linux, web asm) only stuff */
#if defined(RGFW_X11) || defined(RGFW_MACOS) || defined(RGFW_WASM)  || defined(RGFW_WAYLAND)
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

#if defined(__cplusplus) && !defined(__EMSCRIPTEN__)
}
#endif

#if _MSC_VER
	#pragma warning( pop )
#endif
#endif