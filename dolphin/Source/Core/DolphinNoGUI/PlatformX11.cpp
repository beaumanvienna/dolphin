// Copyright 2018 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <unistd.h>

#include "DolphinNoGUI/Platform.h"

#include "Common/MsgHandler.h"
#include "Core/Config/MainSettings.h"
#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/State.h"

#include <climits>
#include <cstdio>
#include <cstring>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "UICommon/X11Utils.h"
#include "VideoCommon/RenderBase.h"
#include "../../../../include/gui.h"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif
extern bool requestShutdownGUIDE;
namespace
{
class PlatformX11 : public Platform
{
public:
  ~PlatformX11() override;

  bool Init() override;
  void SetTitle(const std::string& string) override;
  void MainLoop() override;

  WindowSystemInfo GetWindowSystemInfo() const override;

private:
  void CloseDisplay();
  void UpdateWindowPosition();
  void ProcessEvents();

  Display* m_display = nullptr;
  SDL_Window* m_window;
  Cursor m_blank_cursor = None;
#if defined(HAVE_XRANDR) && HAVE_XRANDR
  X11Utils::XRRConfiguration* m_xrr_config = nullptr;
#endif
  int m_window_x = Config::Get(Config::MAIN_RENDER_WINDOW_XPOS);
  int m_window_y = Config::Get(Config::MAIN_RENDER_WINDOW_YPOS);
  unsigned int m_window_width = Config::Get(Config::MAIN_RENDER_WINDOW_WIDTH);
  unsigned int m_window_height = Config::Get(Config::MAIN_RENDER_WINDOW_HEIGHT);
};

PlatformX11::~PlatformX11()
{
    //modified for Marley
    /*
#if defined(HAVE_XRANDR) && HAVE_XRANDR
  delete m_xrr_config;
#endif

  if (m_display)
  {
    if (SConfig::GetInstance().bHideCursor)
      XFreeCursor(m_display, m_blank_cursor);

    XCloseDisplay(m_display);
  }*/
}
int startup_counter = 0;
bool PlatformX11::Init()
{
  startup_counter = 0;
  XInitThreads();
  m_window = gWindow;
  
  m_display = XOpenDisplay(nullptr);
  if (!m_display)
  {
    PanicAlert("No X11 display found");
    return false;
  }
  // modified for Marley
  /*
  m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), m_window_x, m_window_y,
                                 m_window_width, m_window_height, 0, 0, BlackPixel(m_display, 0));
  XSelectInput(m_display, m_window, StructureNotifyMask | KeyPressMask | FocusChangeMask);
  Atom wmProtocols[1];
  wmProtocols[0] = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(m_display, m_window, wmProtocols, 1);
  pid_t pid = getpid();
  XChangeProperty(m_display, m_window, XInternAtom(m_display, "_NET_WM_PID", False), XA_CARDINAL,
                  32, PropModeReplace, reinterpret_cast<unsigned char*>(&pid), 1);
  char host_name[HOST_NAME_MAX] = "";
  if (!gethostname(host_name, sizeof(host_name)))
  {
    XTextProperty wmClientMachine = {reinterpret_cast<unsigned char*>(host_name), XA_STRING, 8,
                                     strlen(host_name)};
    XSetWMClientMachine(m_display, m_window, &wmClientMachine);
  }
  XMapRaised(m_display, m_window);
  XFlush(m_display);
  XSync(m_display, True);
  ProcessEvents();

  if (Config::Get(Config::MAIN_DISABLE_SCREENSAVER))
    X11Utils::InhibitScreensaver(m_window, true);

#if defined(HAVE_XRANDR) && HAVE_XRANDR
  m_xrr_config = new X11Utils::XRRConfiguration(m_display, m_window);
#endif

  if (SConfig::GetInstance().bHideCursor)
  {
    // make a blank cursor
    Pixmap Blank;
    XColor DummyColor;
    char ZeroData[1] = {0};
    Blank = XCreateBitmapFromData(m_display, m_window, ZeroData, 1, 1);
    m_blank_cursor = XCreatePixmapCursor(m_display, Blank, Blank, &DummyColor, &DummyColor, 0, 0);
    XFreePixmap(m_display, Blank);
    XDefineCursor(m_display, m_window, m_blank_cursor);
  }

  // Enter fullscreen if enabled.
  if (Config::Get(Config::MAIN_FULLSCREEN))
  {
    m_window_fullscreen = X11Utils::ToggleFullscreen(m_display, m_window);
#if defined(HAVE_XRANDR) && HAVE_XRANDR
    m_xrr_config->ToggleDisplayMode(True);
#endif
    ProcessEvents();
  }

  UpdateWindowPosition();
  */
  return true;
}

void PlatformX11::SetTitle(const std::string& string)
{
  //XStoreName(m_display, m_window, string.c_str());
}

void PlatformX11::MainLoop()
{
  while (IsRunning())
  {
    UpdateRunningFlag();
    Core::HostDispatchJobs();
    ProcessEvents();
    UpdateWindowPosition();

    // TODO: Is this sleep appropriate?
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

WindowSystemInfo PlatformX11::GetWindowSystemInfo() const
{
  WindowSystemInfo wsi;
  wsi.type = WindowSystemType::X11;
  wsi.display_connection = static_cast<void*>(m_display);
  wsi.render_window = reinterpret_cast<void*>(m_window);
  wsi.render_surface = reinterpret_cast<void*>(m_window);
  return wsi;
}

void PlatformX11::UpdateWindowPosition()
{
    /*
  if (m_window_fullscreen)
    return;

  Window winDummy;
  unsigned int borderDummy, depthDummy;
  XGetGeometry(m_display, m_window, &winDummy, &m_window_x, &m_window_y, &m_window_width,
               &m_window_height, &borderDummy, &depthDummy);
               */
}

void PlatformX11::ProcessEvents()
{
    //modified for Marley
    SDL_Event event;
    startup_counter++;
    if(requestShutdownGUIDE)
    {
        if (startup_counter > 10)
            RequestShutdown();
        else
            requestShutdownGUIDE = false;
    }
    while(SDL_PollEvent(&event))
    {

        if(event.type==SDL_QUIT)
            RequestShutdown();

        if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_ESCAPE)
            RequestShutdown();
            
        if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_F5)
            State::Save(0);
            
        if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_F7)
            State::Load(0);
        
        if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_F9)
            Core::SaveScreenShot();
        
        if(event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_f)
        {
            Uint32 flags = SDL_GetWindowFlags(gWindow);
            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
            {
                flags &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;
            }
            else
            {
                flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            }
            SDL_SetWindowFullscreen(gWindow,flags);
            if (g_renderer) g_renderer->ResizeSurface();
        }

    }
    /*
  XEvent event;
  KeySym key;
  for (int num_events = XPending(m_display); num_events > 0; num_events--)
  {
    XNextEvent(m_display, &event);
    switch (event.type)
    {
    case KeyPress:
      key = XLookupKeysym((XKeyEvent*)&event, 0);
      if (key == XK_Escape)
      {
        RequestShutdown();
      }
      else if (key == XK_F10)
      {
        if (Core::GetState() == Core::State::Running)
        {
          if (SConfig::GetInstance().bHideCursor)
            XUndefineCursor(m_display, m_window);
          Core::SetState(Core::State::Paused);
        }
        else
        {
          if (SConfig::GetInstance().bHideCursor)
            XDefineCursor(m_display, m_window, m_blank_cursor);
          Core::SetState(Core::State::Running);
        }
      }
      else if ((key == XK_Return) && (event.xkey.state & Mod1Mask))
      {
        m_window_fullscreen = !m_window_fullscreen;
        X11Utils::ToggleFullscreen(m_display, m_window);
#if defined(HAVE_XRANDR) && HAVE_XRANDR
        m_xrr_config->ToggleDisplayMode(m_window_fullscreen);
#endif
        UpdateWindowPosition();
      }
      else if (key >= XK_F1 && key <= XK_F8)
      {
        int slot_number = key - XK_F1 + 1;
        if (event.xkey.state & ShiftMask)
          State::Save(slot_number);
        else
          State::Load(slot_number);
      }
      else if (key == XK_F9)
        Core::SaveScreenShot();
      else if (key == XK_F11)
        State::LoadLastSaved();
      else if (key == XK_F12)
      {
        if (event.xkey.state & ShiftMask)
          State::UndoLoadState();
        else
          State::UndoSaveState();
      }
      break;
    case FocusIn:
    {
      m_window_focus = true;
      if (SConfig::GetInstance().bHideCursor && Core::GetState() != Core::State::Paused)
        XDefineCursor(m_display, m_window, m_blank_cursor);
    }
    break;
    case FocusOut:
    {
      m_window_focus = false;
      if (SConfig::GetInstance().bHideCursor)
        XUndefineCursor(m_display, m_window);
    }
    break;
    case ClientMessage:
    {
      if ((unsigned long)event.xclient.data.l[0] ==
          XInternAtom(m_display, "WM_DELETE_WINDOW", False))
        Stop();
    }
    break;
    case ConfigureNotify:
    {
      if (g_renderer)
        g_renderer->ResizeSurface();
    }
    break;
    }
  }
  */
}
}  // namespace

std::unique_ptr<Platform> Platform::CreateX11Platform()
{
  return std::make_unique<PlatformX11>();
}