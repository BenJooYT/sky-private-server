/*
 * This file was generated with gl3w_gen.py, part of imgl3w
 * (hosted at https://github.com/dearimgui/gl3w_stripped)
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __gl3w_h_
#define __gl3w_h_

// Adapted from KHR/khrplatform.h to avoid including entire file.
#ifndef __khrplatform_h_
typedef          float         khronos_float_t;
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;
#ifdef _WIN64
typedef signed   long long int khronos_intptr_t;
typedef signed   long long int khronos_ssize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef signed   long  int     khronos_ssize_t;
#endif

#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64       khronos_int64_t;
typedef unsigned __int64       khronos_uint64_t;
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef          int64_t       khronos_int64_t;
typedef          uint64_t      khronos_uint64_t;
#else
typedef signed   long long     khronos_int64_t;
typedef unsigned long long     khronos_uint64_t;
#endif
#endif  // __khrplatform_h_

#ifndef __gl_glcorearb_h_
#define __gl_glcorearb_h_ 1
#ifdef __cplusplus
extern "C" {
#endif
/*
** Copyright 2013-2025 The Khronos Group Inc.
** SPDX-License-Identifier: MIT
**
** This header is generated from the Khronos OpenGL / OpenGL ES XML
** API Registry. The current version of the Registry, generator scripts
** used to make the header, and the header can be found at
**   https://github.com/KhronosGroup/OpenGL-Registry
*/
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif
/* glcorearb.h is for use with OpenGL core profile implementations.
** It should should be placed in the same directory as gl.h and
** included as <GL/glcorearb.h>.
**
** glcorearb.h includes only APIs in the latest OpenGL core profile
** implementation together with APIs in newer ARB extensions which 
** can be supported by the core profile. It does not, and never will
** include functionality removed from the core profile, such as
** fixed-function vertex and fragment processing.
**
** Do not #include both <GL/glcorearb.h> and either of <GL/gl.h> or
** <GL/glext.h> in the same source file.
*/
/* Generated C header for:
 * API: gl
 * Profile: core
 * Versions considered: .*
 * Versions emitted: .*
 * Default extensions included: glcore
 * Additional extensions included: _nomatch_^
 * Extensions removed: _nomatch_^
 */
#ifndef GL_VERSION_1_0
typedef void GLvoid;
typedef unsigned int GLenum;

typedef khronos_float_t GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef khronos_uint8_t GLubyte;
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_TRUE                           1
#define GL_ALWAYS                         0x0207
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_KEEP                           0x1E00
#define GL_VERSION                        0x1F02
typedef void (APIENTRYP PFNGLCLEARPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC) (GLint s);
typedef void (APIENTRYP PFNGLSTENCILMASKPROC) (GLuint mask);
typedef void (APIENTRYP PFNGLCOLORMASKPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC) (GLboolean flag);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC) (GLenum pname, GLint *data);
typedef const GLubyte *(APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearStencil (GLint s);
GLAPI void APIENTRY glStencilMask (GLuint mask);
GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLAPI void APIENTRY glDepthMask (GLboolean flag);
GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *data);
GLAPI const GLubyte *APIENTRY glGetString (GLenum name);
#endif
#endif /* GL_VERSION_1_0 */
#ifndef GL_VERSION_1_1
typedef khronos_float_t GLclampf;
typedef double GLclampd;
#endif /* GL_VERSION_1_1 */
#ifndef GL_VERSION_1_5
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
#endif /* GL_VERSION_1_5 */
#ifndef GL_VERSION_2_0
typedef char GLchar;
typedef khronos_int16_t GLshort;
typedef khronos_int8_t GLbyte;
typedef khronos_uint16_t GLushort;
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC) (GLenum face, GLenum func, GLint ref, GLuint mask);
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
GLAPI void APIENTRY glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask);
#endif
#endif /* GL_VERSION_2_0 */
#ifndef GL_VERSION_3_0
typedef khronos_uint16_t GLhalf;
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_FRAMEBUFFER                    0x8D40
typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC) (GLenum target, GLuint index, GLboolean *data);
typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint *data);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer);
#endif
#endif /* GL_VERSION_3_0 */
#ifndef GL_VERSION_3_2
typedef struct __GLsync *GLsync;
typedef khronos_uint64_t GLuint64;
typedef khronos_int64_t GLint64;
typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC) (GLenum target, GLuint index, GLint64 *data);
#endif /* GL_VERSION_3_2 */
#ifndef GL_VERSION_4_1
typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC) (GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC) (GLfloat d);
typedef void (APIENTRYP PFNGLGETFLOATI_VPROC) (GLenum target, GLuint index, GLfloat *data);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC) (GLenum target, GLuint index, GLdouble *data);
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glDepthRangef (GLfloat n, GLfloat f);
GLAPI void APIENTRY glClearDepthf (GLfloat d);
#endif
#endif /* GL_VERSION_4_1 */
#ifndef GL_VERSION_4_3
typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#endif /* GL_VERSION_4_3 */
#ifndef GL_VERSION_4_5
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint *param);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI64_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint64 *param);
#endif /* GL_VERSION_4_5 */
#ifndef GL_ARB_bindless_texture
typedef khronos_uint64_t GLuint64EXT;
#endif /* GL_ARB_bindless_texture */
#ifndef GL_ARB_cl_event
struct _cl_context;
struct _cl_event;
#endif /* GL_ARB_cl_event */
#ifndef GL_ARB_debug_output
typedef void (APIENTRY  *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#endif /* GL_ARB_debug_output */
#ifndef GL_EXT_EGL_image_storage
typedef void *GLeglImageOES;
#endif /* GL_EXT_EGL_image_storage */
#ifndef GL_EXT_direct_state_access
typedef void (APIENTRYP PFNGLGETFLOATI_VEXTPROC) (GLenum pname, GLuint index, GLfloat *params);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VEXTPROC) (GLenum pname, GLuint index, GLdouble *params);
typedef void (APIENTRYP PFNGLGETPOINTERI_VEXTPROC) (GLenum pname, GLuint index, void **params);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC) (GLuint vaobj, GLuint index, GLenum pname, void **param);
#endif /* GL_EXT_direct_state_access */
#ifndef GL_NV_draw_vulkan_image
typedef void (APIENTRY  *GLVULKANPROCNV)(void);
#endif /* GL_NV_draw_vulkan_image */
#ifndef GL_NV_gpu_shader5
typedef khronos_int64_t GLint64EXT;
#endif /* GL_NV_gpu_shader5 */
#ifndef GL_NV_vertex_buffer_unified_memory
typedef void (APIENTRYP PFNGLGETINTEGERUI64I_VNVPROC) (GLenum value, GLuint index, GLuint64EXT *result);
#endif /* GL_NV_vertex_buffer_unified_memory */
#ifdef __cplusplus
}
#endif
#endif

#ifndef GL3W_API
#define GL3W_API
#endif

#ifndef __gl_h_
#define __gl_h_
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GL3W_OK 0
#define GL3W_ERROR_INIT -1
#define GL3W_ERROR_LIBRARY_OPEN -2
#define GL3W_ERROR_OPENGL_VERSION -3

typedef void (*GL3WglProc)(void);
typedef GL3WglProc (*GL3WGetProcAddressProc)(const char *proc);

/* gl3w api */
GL3W_API int ht_imgl3wInit(void);
GL3W_API int ht_imgl3wInit2(GL3WGetProcAddressProc proc);
GL3W_API void ht_imgl3wShutdown(void);
GL3W_API int ht_imgl3wIsSupported(int major, int minor);
GL3W_API GL3WglProc ht_imgl3wGetProcAddress(const char *proc);

/* gl3w internal state */
union ImGL3WProcs {
    GL3WglProc ptr[12];
    struct {
        PFNGLBINDFRAMEBUFFERPROC     BindFramebuffer;
        PFNGLCLEARPROC               Clear;
        PFNGLCLEARDEPTHFPROC         ClearDepthf;
        PFNGLCLEARSTENCILPROC        ClearStencil;
        PFNGLCOLORMASKPROC           ColorMask;
        PFNGLDEPTHMASKPROC           DepthMask;
        PFNGLDEPTHRANGEFPROC         DepthRangef;
        PFNGLGETINTEGERVPROC         GetIntegerv;
        PFNGLGETSTRINGPROC           GetString;
        PFNGLSTENCILFUNCSEPARATEPROC StencilFuncSeparate;
        PFNGLSTENCILMASKPROC         StencilMask;
        PFNGLSTENCILOPSEPARATEPROC   StencilOpSeparate;
    } gl;
};

GL3W_API extern union ImGL3WProcs ht_imgl3wProcs;

/* OpenGL functions */
#define glBindFramebuffer            ht_imgl3wProcs.gl.BindFramebuffer
#define glClear                      ht_imgl3wProcs.gl.Clear
#define glClearDepthf                ht_imgl3wProcs.gl.ClearDepthf
#define glClearStencil               ht_imgl3wProcs.gl.ClearStencil
#define glColorMask                  ht_imgl3wProcs.gl.ColorMask
#define glDepthMask                  ht_imgl3wProcs.gl.DepthMask
#define glDepthRangef                ht_imgl3wProcs.gl.DepthRangef
#define glGetIntegerv                ht_imgl3wProcs.gl.GetIntegerv
#define glGetString                  ht_imgl3wProcs.gl.GetString
#define glStencilFuncSeparate        ht_imgl3wProcs.gl.StencilFuncSeparate
#define glStencilMask                ht_imgl3wProcs.gl.StencilMask
#define glStencilOpSeparate          ht_imgl3wProcs.gl.StencilOpSeparate

#ifdef __cplusplus
}
#endif

#endif

#ifdef HT_IMGL3W_IMPL
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define GL3W_ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

static HMODULE libgl = NULL;
typedef PROC(__stdcall* GL3WglGetProcAddr)(LPCSTR);
static GL3WglGetProcAddr wgl_get_proc_address;

static int open_libgl(void)
{
    libgl = LoadLibraryA("opengl32.dll");
    if (!libgl)
        return GL3W_ERROR_LIBRARY_OPEN;
    wgl_get_proc_address = (GL3WglGetProcAddr)GetProcAddress(libgl, "wglGetProcAddress");
    return GL3W_OK;
}

static void close_libgl(void) { FreeLibrary(libgl); libgl = NULL; }
static GL3WglProc get_proc(const char *proc)
{
    GL3WglProc res;
    res = (GL3WglProc)wgl_get_proc_address(proc);
    if (!res)
        res = (GL3WglProc)GetProcAddress(libgl, proc);
    return res;
}
#elif defined(__APPLE__)
#include <dlfcn.h>

static void *libgl = NULL;
static int open_libgl(void)
{
    libgl = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
    if (!libgl)
        return GL3W_ERROR_LIBRARY_OPEN;
    return GL3W_OK;
}

static void close_libgl(void) { dlclose(libgl); libgl = NULL; }

static GL3WglProc get_proc(const char *proc)
{
    GL3WglProc res;
    *(void **)(&res) = dlsym(libgl, proc);
    return res;
}
#else
#include <dlfcn.h>

static void* libgl;  // OpenGL library
static void* libglx;  // GLX library
static void* libegl;  // EGL library
static GL3WGetProcAddressProc gl_get_proc_address;

static void close_libgl(void)
{
    if (libgl) {
        dlclose(libgl);
        libgl = NULL;
    }
    if (libegl) {
        dlclose(libegl);
        libegl = NULL;
    }
    if (libglx) {
        dlclose(libglx);
        libglx = NULL;
    }
}

static int is_library_loaded(const char* name, void** lib)
{
#if defined(__HAIKU__)
    *lib = NULL; // no support for RTLD_NOLOAD on Haiku.
#else
    *lib = dlopen(name, RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
#endif
    return *lib != NULL;
}

static int open_libs(void)
{
    // On Linux we have two APIs to get process addresses: EGL and GLX.
    // EGL is supported under both X11 and Wayland, whereas GLX is X11-specific.

    libgl = NULL;
    libegl = NULL;
    libglx = NULL;

    // First check what's already loaded, the windowing library might have
    // already loaded either EGL or GLX and we want to use the same one.

    if (is_library_loaded("libEGL.so.1", &libegl) ||
        is_library_loaded("libGLX.so.0", &libglx)) {
        libgl = dlopen("libOpenGL.so.0", RTLD_LAZY | RTLD_LOCAL);
        if (libgl)
            return GL3W_OK;
        else
            close_libgl();
    }

    if (is_library_loaded("libGL.so", &libgl))
        return GL3W_OK;
    if (is_library_loaded("libGL.so.1", &libgl))
        return GL3W_OK;
    if (is_library_loaded("libGL.so.3", &libgl))
        return GL3W_OK;

    // Neither is already loaded, so we have to load one.  Try EGL first
    // because it is supported under both X11 and Wayland.

    // Load OpenGL + EGL
    libgl = dlopen("libOpenGL.so.0", RTLD_LAZY | RTLD_LOCAL);
    libegl = dlopen("libEGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (libgl && libegl)
        return GL3W_OK;
    else
        close_libgl();

    // Fall back to legacy libGL, which includes GLX
    // While most systems use libGL.so.1, NetBSD seems to use that libGL.so.3. See https://github.com/ocornut/imgui/issues/6983
    libgl = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
    if (!libgl)
        libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!libgl)
        libgl = dlopen("libGL.so.3", RTLD_LAZY | RTLD_LOCAL);

    if (libgl)
        return GL3W_OK;

    return GL3W_ERROR_LIBRARY_OPEN;
}

static int open_libgl(void)
{
    int res = open_libs();
    if (res)
        return res;

    if (libegl)
        *(void**)(&gl_get_proc_address) = dlsym(libegl, "eglGetProcAddress");
    else if (libglx)
        *(void**)(&gl_get_proc_address) = dlsym(libglx, "glXGetProcAddressARB");
    else
        *(void**)(&gl_get_proc_address) = dlsym(libgl, "glXGetProcAddressARB");

    if (!gl_get_proc_address) {
        close_libgl();
        return GL3W_ERROR_LIBRARY_OPEN;
    }

    return GL3W_OK;
}

static GL3WglProc get_proc(const char* proc)
{
    GL3WglProc res = NULL;

    // Before EGL version 1.5, eglGetProcAddress doesn't support querying core
    // functions and may return a dummy function if we try, so try to load the
    // function from the GL library directly first.
    if (libegl)
        *(void**)(&res) = dlsym(libgl, proc);

    if (!res)
        res = gl_get_proc_address(proc);

    if (!libegl && !res)
        *(void**)(&res) = dlsym(libgl, proc);

    return res;
}
#endif

static struct { int major, minor; } version;

static int parse_version(void)
{
    if (!glGetIntegerv)
        return GL3W_ERROR_INIT;
    glGetIntegerv(GL_MAJOR_VERSION, &version.major);
    glGetIntegerv(GL_MINOR_VERSION, &version.minor);
    if (version.major == 0 && version.minor == 0)
    {
        // Query GL_VERSION in desktop GL 2.x, the string will start with "<major>.<minor>"
        if (const char* gl_version = (const char*)glGetString(GL_VERSION))
            sscanf(gl_version, "%d.%d", &version.major, &version.minor);
    }
    if (version.major < 2)
        return GL3W_ERROR_OPENGL_VERSION;
    return GL3W_OK;
}

static void load_procs(GL3WGetProcAddressProc proc);

int ht_imgl3wInit(void)
{
    int res = open_libgl();
    if (res)
        return res;
    atexit(close_libgl);
    return ht_imgl3wInit2(get_proc);
}

int ht_imgl3wInit2(GL3WGetProcAddressProc proc)
{
    load_procs(proc);
    return parse_version();
}

void ht_imgl3wShutdown(void)
{
    close_libgl();
}

int ht_imgl3wIsSupported(int major, int minor)
{
    if (major < 2)
        return 0;
    if (version.major == major)
        return version.minor >= minor;
    return version.major >= major;
}

GL3WglProc ht_imgl3wGetProcAddress(const char *proc) { return get_proc(proc); }

static const char *proc_names[] = {
    "glBindFramebuffer",
    "glClear",
    "glClearDepthf",
    "glClearStencil",
    "glColorMask",
    "glDepthMask",
    "glDepthRangef",
    "glGetIntegerv",
    "glGetString",
    "glStencilFuncSeparate",
    "glStencilMask",
    "glStencilOpSeparate",
};

GL3W_API union ImGL3WProcs ht_imgl3wProcs;

static void load_procs(GL3WGetProcAddressProc proc)
{
    size_t i;
    for (i = 0; i < GL3W_ARRAY_SIZE(proc_names); i++)
        ht_imgl3wProcs.ptr[i] = proc(proc_names[i]);
}

#ifdef __cplusplus
}
#endif
#endif
