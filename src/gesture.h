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

enum class Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

class Gesture
{
  public:
    Gesture(GestureData& gestureData);
    Gesture() {}
    ~Gesture();

    typedef bool (Gesture::*checkerFuncPtr)(SensorData&);

    virtual bool invite(SensorData& sensorData);
    virtual bool performAction(void);


  private:
    bool isPoint(std::vector<CoordinatorData> coordinatorsData);
    bool isVerticalMove(std::vector<CoordinatorData> coordinatorsData);
    bool isHorizontalMove(std::vector<CoordinatorData> coordinatorsData);


    bool isMoveUp(SensorData& sensorData);
    bool isMoveDown(SensorData& sensorData);
    bool isMoveLeft(SensorData& sensorData);
    bool isMoveRight(SensorData& sensorData);
    bool isEnlarged(SensorData& sensorData);
    bool isShrinked(SensorData& sensorData);
    bool isPressed(SensorData& sensorData);

    bool movementChecker(SensorData& sensorData);
    std::int32_t convertOrientation(Orientation orientation);
    std::int32_t convertMovementEvidence(Evidence evidence);

    checkerFuncPtr assignChecker(Evidence evidence);
    std::uint32_t m_touchPointNumber;
    Evidence m_evidence;
    std::string m_action;
    checkerFuncPtr m_evidenceChecker;
    std::uint32_t m_triggerTimes;

};

