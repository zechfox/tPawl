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
    BuildInGesture(): m_name("Build In Gesture") {}
    ~BuildInGesture() {}
    bool invite(SensorData& sensorData);
    bool performAction(void);
    void dump();

  private:
    BuildInAction getOneFingerAction(SensorData& sensorData);
    BuildInAction getTwoFingerAction(SensorData& sensorData);

    BuildInAction m_action;
    std::string m_name;

};
