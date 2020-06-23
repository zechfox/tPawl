/*=============================
/* Gesture Lib
/*
/*
/*===========================*/
#pragma once
#include "typeDef.h"

namespace GestureLib {
  bool isPoint(std::vector<CoordinatorData> coordinatorsData);
  std::pair<PressType, CoordinatorData> isPressed(SensorData& sensorData);
  bool isVerticalMove(std::vector<CoordinatorData> coordinatorsData);
  bool isHorizontalMove(std::vector<CoordinatorData> coordinatorsData);


  bool isMoveUp(SensorData& sensorData);
  bool isMoveDown(SensorData& sensorData);
  bool isMoveLeft(SensorData& sensorData);
  bool isMoveRight(SensorData& sensorData);
  bool isEnlarged(SensorData& sensorData);
  bool isShrinked(SensorData& sensorData);
}

