/*=============================
/* Gesture
/*
/*
/*===========================*/
#include <iostream>
#include <stdlib.h>

#include "gesture.h"
 
using namespace std;

Gesture::Gesture(GestureData& gestureData):
  m_touchPointNumber(gestureData.touchPointNumber),
  m_evidence(gestureData.evidence),
  m_action(gestureData.action)
{
  //m_evidenceChecker = 
}

Gesture::~Gesture()
{

}

bool Gesture::invite(SensorData& sensorData)
{

  return true;
}

bool Gesture::isMoveUp(SensorData& sensorData)
{

  return true;
}
bool Gesture::performAction(void)
{
  system(m_action.c_str());
  return true;
}
