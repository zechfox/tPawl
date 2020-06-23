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
    void dump();

  private:
    bool movementChecker(SensorData& sensorData);
    bool enlargeChecker(SensorData& sensorData);
    bool shrinkChecker(SensorData& sensorData);
    
    checkerFuncPtr assignChecker(Evidence evidence);
    std::uint32_t m_touchPointNumber;
    Evidence m_evidence;
    std::string m_action;
    checkerFuncPtr m_evidenceChecker;
    std::uint32_t m_triggerTimes;
    std::string m_name;

};

