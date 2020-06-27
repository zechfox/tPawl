/*=============================
/* XLib API Source
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>

#include "XLibApi.h"
#include "log.h"

XLibApiPtr XLibApi::m_singletonInstance = 0;

XLibApi::XLibApi()
{
  m_displayPtr = XOpenDisplay(":0.0");
  if (NULL == m_displayPtr)
  {
    LOG("Open Display Failed");
  }
  LOG("XLibApi Start!");
}

XLibApi::~XLibApi()
{
  XCloseDisplay(m_displayPtr);
}

XLibApiPtr XLibApi::getInstance()
{
  if (!m_singletonInstance)
  {
    m_singletonInstance = std::make_shared<XLibApi>();
  }
  return m_singletonInstance;
}

void XLibApi::freeInstance()
{
  if (m_singletonInstance)
  {
    m_singletonInstance.reset();
  }
}

bool XLibApi::sendKeyboardEvent()
{
  // KeyPress or KeyRelease
  return false;
}

bool XLibApi::sendMouseEvent(KeyState keyState, BuildInKey key, CoordinatorData coordinatorData)
{
  //typedef struct {
  //  int type;  /* ButtonPress or ButtonRelease */
  //  unsigned long serial; /* # of last request processed by server */
  //  Bool send_event; /* true if this came from a SendEvent request */
  //  Display *display; /* Display the event was read from */
  //  Window window;  /* ``event'' window it is reported relative to */
  //  Window root;  /* root window that the event occurred on */
  //  Window subwindow; /* child window */
  //  Time time;  /* milliseconds */
  //  int x, y;  /* pointer x, y coordinates in event window */
  //  int x_root, y_root; /* coordinates relative to root */
  //  unsigned int state; /* key or button mask */
  //  unsigned int button; /* detail */
  //  Bool same_screen; /* same screen flag */
  //} XButtonEvent;
  //typedef XButtonEvent XButtonPressedEvent;
  //typedef XButtonEvent XButtonReleasedEvent;

  // ButtonPress, ButtonRelease, and MotionNotify
  XButtonEvent xEvent;
  std::intptr_t focusState;
  

  XQueryPointer(m_displayPtr, DefaultRootWindow (m_displayPtr),
                &xEvent.root, &xEvent.window,
                &xEvent.x_root, &xEvent.y_root,
                &xEvent.x, &xEvent.y,
                &xEvent.state);

  if (!XGetInputFocus(m_displayPtr, &xEvent.window, (int*)focusState))
  {
    LOG("Get X focus windows failed!");
    return false;
  }
  xEvent.display = m_displayPtr;
  // calculate relative position
  xEvent.x = coordinatorData.x - xEvent.x;
  xEvent.y = coordinatorData.y - xEvent.y;

  if (KeyState::PRESS == keyState)
  {
    xEvent.type = ButtonPress;
  }
  else if (KeyState::RELEASE == keyState)
  {
    xEvent.type = ButtonRelease;
  }

  if (BuildInKey::LEFT_CLICK == key)
  {
    xEvent.button = Button1;
  }
  else if (BuildInKey::RIGHT_CLICK == key)
  {
    xEvent.button = Button3;
  }
  else if (BuildInKey::WHEEL_UP == key)
  {
    xEvent.button = Button4;
  }
  else if (BuildInKey::WHEEL_DOWN == key)
  {
    xEvent.button = Button5;
  }

  if (BuildInKey::MOUSE_MOVE == key)
  {
    XWarpPointer (m_displayPtr, None, None, 0,0,0,0, xEvent.x, xEvent.y);
    XFlush(m_displayPtr);
    return true;
  }

  if (XSendEvent (m_displayPtr, PointerWindow, True, NoEventMask, (XEvent*)&xEvent) == 0)
  {
    LOG("Error to send the event!");
  }

  XFlush (m_displayPtr);
  return true;
}
