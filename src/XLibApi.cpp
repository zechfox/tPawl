/*=============================
/* XLib API Source
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>

#include "XLibApi.h"
#include "log.h"

XLibApiPtr XLibApi::m_singletonInstance = 0;

XLibApi::XLibApi()
{
  m_displayPtr = XOpenDisplay(":0.0");

  if (NULL == m_displayPtr)
  {
    LOG("Open Display Failed");
    return;
  }
  m_screenConfigurationPtr = XRRGetScreenInfo(m_displayPtr, DefaultRootWindow(m_displayPtr));
  LOG("XLibApi Start!");
}

XLibApi::~XLibApi()
{
  XRRFreeScreenConfigInfo(m_screenConfigurationPtr);
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

CoordinatorData XLibApi::getScreenSize()
{
  Screen* s = DefaultScreenOfDisplay(m_displayPtr);
  CoordinatorData screenSize{s->width, s->height};
  return screenSize;
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

// set integer property, format = 32, 1 value
bool XLibApi::setDeviceIntProps(std::string& devName, std::string& propertyName, std::int32_t value)
{
  XIDeviceInfo *devices;
  std::intptr_t numDevices;
  Atom prop = XInternAtom(m_displayPtr, propertyName.c_str(), False);;
  std::int32_t loop = 0;
  bool found = false;
     
  devices = XIQueryDevice(m_displayPtr, XIAllDevices, (int *)numDevices);
  for(loop = 0; loop < numDevices; loop++)
  {
    if (strcmp(devices[loop].name, devName.c_str()) == 0)
    {
      found = true;
      break;
    }
  }
  
  if (found)
  {
    XIChangeProperty(m_displayPtr, devices[loop].deviceid, prop, XA_INTEGER, 32, PropModeReplace,
                     (unsigned char*)&value, 1);
  }
  else
  {
    LOG("ERROR: No Such Device was found.");
  }

  return found;
}

bool XLibApi::rotateScreen(Orientation orientation)
{
  Rotation rotation;
  std::map<Orientation, Rotation> orientation2rotation {
    {Orientation::NORMAL, RR_Rotate_0},
        {Orientation::INVERT, RR_Rotate_180},
        {Orientation::LEFT, RR_Rotate_90},
        {Orientation::RIGHT, RR_Rotate_270},
  };
  rotation = orientation2rotation[orientation];
  
  SizeID configurationId = XRRConfigCurrentConfiguration(m_screenConfigurationPtr, &rotation);

  return XRRSetScreenConfig(m_displayPtr,
                            m_screenConfigurationPtr,
                            DefaultRootWindow(m_displayPtr),
                            configurationId, 
                            rotation,
                            CurrentTime);
}
