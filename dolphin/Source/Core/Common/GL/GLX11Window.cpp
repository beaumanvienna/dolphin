// Copyright 2012 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "Common/GL/GLX11Window.h"
#include "Common/GL/GLContext.h"
#include "../../../../../include/gui.h"

GLX11Window::GLX11Window(Display* display, Window parent_window, Colormap color_map, Window window,
                         int width, int height)
    : m_display(display), m_parent_window(parent_window), m_color_map(color_map), m_window(window),
      m_width(width), m_height(height)
{
}

GLX11Window::~GLX11Window()
{
  //Marley XUnmapWindow(m_display, m_window);
  //Marley XDestroyWindow(m_display, m_window);
  //Marley XFreeColormap(m_display, m_color_map);
}

void GLX11Window::UpdateDimensions()
{
  //Marley XWindowAttributes attribs;
  //Marley XGetWindowAttributes(m_display, m_parent_window, &attribs);
  //Marley XResizeWindow(m_display, m_window, attribs.width, attribs.height);
  //Marley m_width = attribs.width;
  //Marley m_height = attribs.height;
  //it reports the old screen dimension, hence the 100ms delay
  SDL_Delay(100);
  SDL_GetWindowSize(gWindow,&m_width,&m_height);
}

std::unique_ptr<GLX11Window> GLX11Window::Create(Display* display, Window parent_window,
                                                 XVisualInfo* vi)
{
  // Set color map for the new window based on the visual.
  //Marley Colormap color_map = XCreateColormap(display, parent_window, vi->visual, AllocNone);
  Colormap color_map;
  Window window;
  //Marley XSetWindowAttributes attribs = {};
  //Marley attribs.colormap = color_map;

  // Get the dimensions from the parent window.
  XWindowAttributes parent_attribs = {};
  //Marley XGetWindowAttributes(display, parent_window, &parent_attribs);

  // Create the window
  //Marley Window window =
  //Marley     XCreateWindow(display, parent_window, 0, 0, parent_attribs.width, parent_attribs.height, 0,
  //Marley                   vi->depth, InputOutput, vi->visual, CWColormap, &attribs);
  //Marley XSelectInput(display, parent_window, StructureNotifyMask);
  //Marley XMapWindow(display, window);
  //Marley XSync(display, True);

  return std::make_unique<GLX11Window>(display, parent_window, color_map, window,
                                       parent_attribs.width, parent_attribs.height);
}
