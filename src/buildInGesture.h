/*=============================
/* Buildin Gesture
/*
/*
/*===========================*/
#pragma once

#include <functional>
#include <string>

#include "gestureIf.h"
#include "typeDef.h"

using namespace std;
class BuildInGesture : public GestureIf
{
  public:
    bool invite(SensorData& sensorData);
    bool performAction(void);

  private:
    std::string m_action;

};
