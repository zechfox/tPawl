/*=============================
/* XLib API Header 
/*
/*
/*===========================*/
#include <memory>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "typeDef.h"
#pragma once
class XLibApi;

typedef std::shared_ptr<XLibApi> XLibApiPtr;

class XLibApi
{
  public:
    XLibApi();
    ~XLibApi();
    static XLibApiPtr getInstance();
    static void freeInstance();
    CoordinatorData getScreenSize();
    bool sendMouseEvent(KeyState keyState, BuildInKey key, CoordinatorData coordinatorData);
    bool sendKeyboardEvent();
    bool setDeviceIntProps(std::string& devName, std::string& propertyName, std::int32_t value);
    bool rotateScreen(Orientation orientation);
    bool mapInputToOutput();

  private:
    static XLibApiPtr m_singletonInstance;
    Display *m_displayPtr;
    XRRScreenConfiguration *m_screenConfigurationPtr;

};
