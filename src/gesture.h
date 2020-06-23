/*=============================
/* Gesture
/*
/*
/*===========================*/
#pragma once

#include <functional>
#include <string>

#include "gestureIf.h"
#include "typeDef.h"

using namespace std;



class Gesture : public GestureIf
{
  public:
    Gesture(GestureData& gestureData);
    Gesture() {}
    ~Gesture();

    typedef bool (Gesture::*checkerFuncPtr)(SensorData&);

    bool invite(SensorData& sensorData);
    bool performAction(void);

  private:
    bool movementChecker(SensorData& sensorData);
    bool enlargeChecker(SensorData& sensorData);
    bool shrinkChecker(SensorData& sensorData);
    std::int32_t convertOrientation(Orientation orientation);
    std::int32_t convertMovementEvidence(Evidence evidence);

    checkerFuncPtr assignChecker(Evidence evidence);
    std::uint32_t m_touchPointNumber;
    Evidence m_evidence;
    std::string m_action;
    checkerFuncPtr m_evidenceChecker;
    std::uint32_t m_triggerTimes;

};

