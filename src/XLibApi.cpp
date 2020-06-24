/*=============================
/* XLib API Source
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>

#include "XLibApi.h"

XLibApiPtr XLibApi::m_singletonInstance = 0;

XLibApiPtr XLibApi::getInstance()
{
  if (!m_singletonInstance)
  {
    m_singletonInstance = std::make_shared<XLibApi>();
  }
  return m_singletonInstance;
}

bool XLibApi::sendKeyboardEvent()
{
  // KeyPress or KeyRelease
  return false;
}

bool XLibApi::sendMouseEvent()
{
  // ButtonPress, ButtonRelease, and MotionNotify
  return false;
}
