/*=============================
/* Gesture
/*
/*
/*===========================*/
#pragma once

#include <functional>
#include "configParser.h"
#include "sensorDataHandler.h"

using namespace std;

class Gesture
{
  public:
    Gesture(GestureData& gestureData);
    ~Gesture();

    bool invite(SensorData& sensorData);
    bool performAction(void);

    static bool isMoveUp(SensorData& sensorData);
  private:
    std::uint32_t m_touchPointNumber;
    Evidence m_evidence;
    std::string m_action;
    std::function<bool(SensorData&)> m_evidenceChecker;

};
