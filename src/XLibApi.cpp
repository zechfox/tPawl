/*=============================
/* XLib API Source
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>

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

bool XLibApi::sendMouseEvent()
{
  // ButtonPress, ButtonRelease, and MotionNotify
  return false;
}
