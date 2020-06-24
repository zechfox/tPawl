/*=============================
/* XLib API Header 
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>
#pragma once
class XLibApi;

typedef std::shared_ptr<XLibApi> XLibApiPtr;

class XLibApi
{
  public:
    static XLibApiPtr getInstance();
    bool sendMouseEvent();
    bool sendKeyboardEvent();
    bool setDeviceProps();
    bool rotateScreen();
    bool mapInputToOutput();

  private:
    static XLibApiPtr m_singletonInstance;

};
