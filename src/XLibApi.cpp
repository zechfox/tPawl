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

bool XLibApi::sendMouseEvent(KeyState keyState, std::int32_t button)
{
  // ButtonPress, ButtonRelease, and MotionNotify
  return false;
}
