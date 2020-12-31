// Copyright 2012 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <array>
#include <sstream>

#include "Common/GL/GLInterface/GLX.h"
#include "Common/Logging/Log.h"
#include "../../../../../../include/gui.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSPROC)(Display*, GLXFBConfig, GLXContext, Bool,
                                                     const int*);
typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int);
typedef int (*PFNGLXSWAPINTERVALMESAPROC)(unsigned int);

static PFNGLXCREATECONTEXTATTRIBSPROC glXCreateContextAttribs = nullptr;
static PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXTPtr = nullptr;
static PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESAPtr = nullptr;

static PFNGLXCREATEGLXPBUFFERSGIXPROC glXCreateGLXPbufferSGIX = nullptr;
static PFNGLXDESTROYGLXPBUFFERSGIXPROC glXDestroyGLXPbufferSGIX = nullptr;

static bool s_glxError;
//Marley static int ctxErrorHandler(Display* dpy, XErrorEvent* ev)
//Marley {
//Marley   s_glxError = true;
//Marley   return 0;
//Marley }

GLContextGLX::~GLContextGLX()
{
  DestroyWindowSurface();
  if (m_context)
  {
    SDL_GL_DeleteContext(m_context);
//Marley    if (glXGetCurrentContext() == m_context)
//Marley      glXMakeCurrent(m_display, None, nullptr);
//Marley
//Marley    glXDestroyContext(m_display, m_context);
  }
}

bool GLContextGLX::IsHeadless() const
{
  return !m_render_window;
}

void GLContextGLX::SwapInterval(int Interval)
{
    #define immediate_updates 0
    #define updates_synchronized 1
    SDL_GL_SetSwapInterval(updates_synchronized);
    /*
  if (!m_drawable)
    return;

  // Try EXT_swap_control, then MESA_swap_control.
  if (glXSwapIntervalEXTPtr)
    glXSwapIntervalEXTPtr(m_display, m_drawable, Interval);
  else if (glXSwapIntervalMESAPtr)
    glXSwapIntervalMESAPtr(static_cast<unsigned int>(Interval));
  else
    ERROR_LOG(VIDEO, "No support for SwapInterval (framerate clamped to monitor refresh rate).");
    */
}

void* GLContextGLX::GetFuncAddress(const std::string& name)
{
  return reinterpret_cast<void*>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name.c_str())));
}

void GLContextGLX::Swap()
{
  //glXSwapBuffers(m_display, m_drawable);
  SDL_GL_SwapWindow(gWindow);
}

// Create rendering window.
// Call browser: Core.cpp:EmuThread() > main.cpp:Video_Initialize()
bool GLContextGLX::Initialize(const WindowSystemInfo& wsi, bool stereo, bool core)
{
  #warning "Marley: modified"
  m_display = static_cast<Display*>(wsi.display_connection);
  int screen = SDL_GetWindowDisplayIndex(gWindow);

  // checking glx version
  int glxMajorVersion, glxMinorVersion;
  glXQueryVersion(m_display, &glxMajorVersion, &glxMinorVersion);
  if (glxMajorVersion < 1 || (glxMajorVersion == 1 && glxMinorVersion < 4))
  {
    ERROR_LOG_FMT(VIDEO, "glX-Version {}.{} detected, but need at least 1.4", glxMajorVersion,
                  glxMinorVersion);
    return false;
  }

  // loading core context creation function
  glXCreateContextAttribs =
      (PFNGLXCREATECONTEXTATTRIBSPROC)GetFuncAddress("glXCreateContextAttribsARB");
  if (!glXCreateContextAttribs)
  {
    ERROR_LOG_FMT(VIDEO,
                  "glXCreateContextAttribsARB not found, do you support GLX_ARB_create_context?");
    return false;
  }
  
  /*
  // choosing framebuffer
  int visual_attribs[] = {GLX_X_RENDERABLE,
                          True,
                          GLX_DRAWABLE_TYPE,
                          GLX_WINDOW_BIT,
                          GLX_X_VISUAL_TYPE,
                          GLX_TRUE_COLOR,
                          GLX_RED_SIZE,
                          8,
                          GLX_GREEN_SIZE,
                          8,
                          GLX_BLUE_SIZE,
                          8,
                          GLX_DEPTH_SIZE,
                          0,
                          GLX_STENCIL_SIZE,
                          0,
                          GLX_DOUBLEBUFFER,
                          True,
                          GLX_STEREO,
                          stereo ? True : False,
                          None};
  
  //Marley int fbcount = 0;
  GLXFBConfig* fbc = glXChooseFBConfig(m_display, screen, visual_attribs, &fbcount);
  if (!fbc || !fbcount)
  {
    ERROR_LOG_FMT(VIDEO, "Failed to retrieve a framebuffer config");
    return false;
  }
  m_fbconfig = *fbc;
  XFree(fbc);

  s_glxError = false;
  XErrorHandler oldHandler = XSetErrorHandler(&ctxErrorHandler);
  */
  
  // Create a GLX context.
  if (core)
  {
      m_context = SDL_GL_CreateContext(gWindow);
      /*
    for (const auto& version : s_desktop_opengl_versions)
    {
      std::array<int, 9> context_attribs = {
          {GLX_CONTEXT_MAJOR_VERSION_ARB, version.first, GLX_CONTEXT_MINOR_VERSION_ARB,
           version.second, GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
           GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, None}};

      s_glxError = false;
      m_context = glXCreateContextAttribs(m_display, m_fbconfig, 0, True, &context_attribs[0]);
      XSync(m_display, False);
      if (!m_context || s_glxError)
        continue;

      // Got a context.
      INFO_LOG_FMT(VIDEO, "Created a GLX context with version {}.{}", version.first,
                   version.second);
      m_attribs.insert(m_attribs.end(), context_attribs.begin(), context_attribs.end());
      break;
    }*/
  }
  
  /*
  // Failed to create any core contexts, try for anything.
  if (!m_context || s_glxError)
  {
    std::array<int, 5> context_attribs_legacy = {
        {GLX_CONTEXT_MAJOR_VERSION_ARB, 1, GLX_CONTEXT_MINOR_VERSION_ARB, 0, None}};
    s_glxError = false;
    m_context = glXCreateContextAttribs(m_display, m_fbconfig, 0, True, &context_attribs_legacy[0]);
    XSync(m_display, False);
    m_attribs.clear();
    m_attribs.insert(m_attribs.end(), context_attribs_legacy.begin(), context_attribs_legacy.end());
  }
  if (!m_context || s_glxError)
  {
    ERROR_LOG(VIDEO, "Unable to create GL context.");
   //Marley XSetErrorHandler(oldHandler);
    return false;
  }
  */

  glXSwapIntervalEXTPtr = nullptr;
  glXSwapIntervalMESAPtr = nullptr;
  glXCreateGLXPbufferSGIX = nullptr;
  glXDestroyGLXPbufferSGIX = nullptr;
  m_supports_pbuffer = false;

  std::string tmp;
  std::istringstream buffer(glXQueryExtensionsString(m_display, screen));
  while (buffer >> tmp)
  {
    if (tmp == "GLX_SGIX_pbuffer")
    {
      glXCreateGLXPbufferSGIX = reinterpret_cast<PFNGLXCREATEGLXPBUFFERSGIXPROC>(
          GetFuncAddress("glXCreateGLXPbufferSGIX"));
      glXDestroyGLXPbufferSGIX = reinterpret_cast<PFNGLXDESTROYGLXPBUFFERSGIXPROC>(
          GetFuncAddress("glXDestroyGLXPbufferSGIX"));
      m_supports_pbuffer = glXCreateGLXPbufferSGIX && glXDestroyGLXPbufferSGIX;
    }
    else if (tmp == "GLX_EXT_swap_control")
    {
      glXSwapIntervalEXTPtr =
          reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(GetFuncAddress("glXSwapIntervalEXT"));
    }
    else if (tmp == "GLX_MESA_swap_control")
    {
      glXSwapIntervalMESAPtr =
          reinterpret_cast<PFNGLXSWAPINTERVALMESAPROC>(GetFuncAddress("glXSwapIntervalMESA"));
    }
  }

  if (!CreateWindowSurface(reinterpret_cast<Window>(wsi.render_surface)))
  {
    ERROR_LOG_FMT(VIDEO, "Error: CreateWindowSurface failed\n");
    //Marley XSetErrorHandler(oldHandler);
    return false;
  }

  //Marley XSetErrorHandler(oldHandler);
  m_opengl_mode = Mode::OpenGL;
  return MakeCurrent();
}

std::unique_ptr<GLContext> GLContextGLX::CreateSharedContext()
{
  s_glxError = false;
  //Marley XErrorHandler oldHandler = XSetErrorHandler(&ctxErrorHandler);

  //Marley GLXContext new_glx_context =
  //Marley     glXCreateContextAttribs(m_display, m_fbconfig, m_context, True, &m_attribs[0]);
  //Marley XSync(m_display, False);

  /*if (!new_glx_context || s_glxError)
  {
    ERROR_LOG_FMT(VIDEO, "Unable to create GL context.");
    XSetErrorHandler(oldHandler);
    return nullptr;
  }*/

  std::unique_ptr<GLContextGLX> new_context = std::make_unique<GLContextGLX>();
  new_context->m_context = m_context;
  new_context->m_opengl_mode = m_opengl_mode;
  new_context->m_supports_pbuffer = m_supports_pbuffer;
  new_context->m_display = m_display;
  new_context->m_fbconfig = m_fbconfig;
  new_context->m_is_shared = true;

  if (m_supports_pbuffer && !new_context->CreateWindowSurface(None))
  {
    ERROR_LOG_FMT(VIDEO, "Error: CreateWindowSurface failed");
    //Marley XSetErrorHandler(oldHandler);
    return nullptr;
  }

  //Marley XSetErrorHandler(oldHandler);
  return new_context;
}

bool GLContextGLX::CreateWindowSurface(Window window_handle)
{
    //Marley 
    /*
  if (window_handle)
  {
    // Get an appropriate visual
    XVisualInfo* vi = glXGetVisualFromFBConfig(m_display, m_fbconfig);
    m_render_window = GLX11Window::Create(m_display, window_handle, vi);
    if (!m_render_window)
      return false;

    m_backbuffer_width = m_render_window->GetWidth();
    m_backbuffer_height = m_render_window->GetHeight();
    m_drawable = static_cast<GLXDrawable>(m_render_window->GetWindow());
    XFree(vi);
  }
  else if (m_supports_pbuffer)
  {
    m_pbuffer = glXCreateGLXPbufferSGIX(m_display, m_fbconfig, 1, 1, nullptr);
    if (!m_pbuffer)
      return false;

    m_drawable = static_cast<GLXDrawable>(m_pbuffer);
  }*/
  int w,h;
  SDL_GetWindowSize(gWindow,&w,&h);
  m_backbuffer_width = w;
  m_backbuffer_height = h;
  
  XVisualInfo* vi = NULL;
  m_render_window = GLX11Window::Create(m_display, window_handle, vi);

  return true;
}

void GLContextGLX::DestroyWindowSurface()
{
  //Marley m_render_window.reset();
  if (m_supports_pbuffer && m_pbuffer)
  {
    //Marley glXDestroyGLXPbufferSGIX(m_display, m_pbuffer);
    m_pbuffer = 0;
  }
}

bool GLContextGLX::MakeCurrent()
{
  bool ok = ((SDL_GL_MakeCurrent(gWindow, m_context) == 0));

  //jcreturn glXMakeCurrent(m_display, m_drawable, m_context);
  return ok;
}

bool GLContextGLX::ClearCurrent()
{
  //Marley return glXMakeCurrent(m_display, None, nullptr);
  return true;
}

void GLContextGLX::Update()
{
  m_render_window->UpdateDimensions();
  m_backbuffer_width = m_render_window->GetWidth();
  m_backbuffer_height = m_render_window->GetHeight();
}
