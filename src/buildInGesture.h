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
    BuildInGesture();
    ~BuildInGesture() {}
    bool invite(SensorData& sensorData);
    bool performAction(void);
    void dump();

  private:
    BuildInAction getOneFingerAction(SensorData& sensorData);
    BuildInAction getTwoFingerAction(SensorData& sensorData);
    CoordinatorData rotatePointerPosition(Orientation orientation, CoordinatorData pointerPosition);

    BuildInAction m_action;
    std::string m_name;
    std::int32_t m_screenWidth;
    std::int32_t m_screenHeight;
    CoordinatorData m_pointerPosition;
    std::int32_t m_gestureInvitedTimes;
    bool m_isPerformAction;
};
