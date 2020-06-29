/*=============================
/* XLib API Source
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>

#include "XLibApi.h"
#include "log.h"

XLibApiPtr XLibApi::m_singletonInstance = 0;

XLibApi::XLibApi()
{
  m_displayPtr = XOpenDisplay(":0");

  if (NULL == m_displayPtr)
  {
    LOG("Open Display Failed");
    return;
  }
  m_screenConfigurationPtr = XRRGetScreenInfo(m_displayPtr, DefaultRootWindow(m_displayPtr));
  if (NULL == m_screenConfigurationPtr)
  {
    LOG("Cannot get screen info.");
  }
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
  XButtonEvent xEvent;
  XQueryPointer(m_displayPtr, DefaultRootWindow(m_displayPtr),
                &xEvent.root, &xEvent.window,
                &xEvent.x_root, &xEvent.y_root,
                &xEvent.x, &xEvent.y,
                &xEvent.state);

  // calculate relative position
  xEvent.x = coordinatorData.x - xEvent.x;
  xEvent.y = coordinatorData.y - xEvent.y;

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

  XWarpPointer (m_displayPtr, None, None, 0,0,0,0, xEvent.x, xEvent.y);

  if (BuildInKey::MOUSE_MOVE != key)
  {
    XTestFakeButtonEvent(m_displayPtr, xEvent.button, KeyState::PRESS == keyState, CurrentTime);
  }

  if ( BuildInKey::WHEEL_UP == key
       || BuildInKey::WHEEL_DOWN == key)
  {
    XTestFakeButtonEvent(m_displayPtr, xEvent.button, false, CurrentTime);
  }

  XFlush (m_displayPtr);
  return true;
}

// set integer property, format = 8, 1 value
bool XLibApi::setDeviceIntProps(std::string& devName, std::string& propertyName, std::int8_t value)
{
  XIDeviceInfo *devices;
  std::int32_t numDevices;
  Atom prop = XInternAtom(m_displayPtr, propertyName.c_str(), False);;
  std::int32_t loop = 0;
  bool found = false;
     
  devices = XIQueryDevice(m_displayPtr, XIAllDevices, (int*)&numDevices);

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
    XIChangeProperty(m_displayPtr, devices[loop].deviceid, prop, XA_INTEGER, 8, PropModeReplace,
                     (unsigned char*)&value, 1);
  }
  else
  {
    LOG("ERROR: No Such Device was found. Device: " << devName);
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

  SizeID configurationId = XRRConfigCurrentConfiguration(m_screenConfigurationPtr, &rotation);
  rotation = orientation2rotation[orientation];

  return RRSetConfigSuccess == XRRSetScreenConfig(m_displayPtr,
                                                  m_screenConfigurationPtr,
                                                  DefaultRootWindow(m_displayPtr),
                                                  configurationId, 
                                                  rotation,
                                                  CurrentTime);
}
