/*=============================
/* Gesture
/*
/*
/*===========================*/

#include "typeDef.h"
#include "gesture.h"
#include "gestureLib.h" 
#include "log.h"

using namespace std;

Gesture::Gesture(GestureData& gestureData):
  m_touchPointNumber(gestureData.touchPointNumber),
  m_evidence(gestureData.evidence),
  m_action(gestureData.action),
  m_triggerTimes(0),
  m_name(gestureData.name)
{
  m_evidenceChecker = assignChecker(m_evidence);
}

Gesture::~Gesture()
{

}

bool Gesture::invite(SensorData& sensorData)
{
  if (m_evidenceChecker)
  {
    return std::invoke(m_evidenceChecker, this, sensorData);
  }
  return false;
}



Gesture::checkerFuncPtr Gesture::assignChecker(Evidence evidence)
{
  Gesture::checkerFuncPtr checkerPtr = nullptr;
  switch (evidence)
  {
    case Evidence::MOVE_UP:
    case Evidence::MOVE_DOWN:
    case Evidence::MOVE_LEFT:
    case Evidence::MOVE_RIGHT:
    case Evidence::MOVE_TOP:
    case Evidence::MOVE_BOTTOM:
    case Evidence::MOVE_LEFT_SIDE:
    case Evidence::MOVE_RIGHT_SIDE:
      checkerPtr = &Gesture::movementChecker;
      break;
    case Evidence::ENLARGE:  
      checkerPtr = &Gesture::enlargeChecker;
      break;
    case Evidence::SHRINK:
      checkerPtr = &Gesture::shrinkChecker;
      break;
    default:
      checkerPtr = &Gesture::movementChecker;
      break;
  }
  return checkerPtr;
}

bool Gesture::enlargeChecker(SensorData& sensorData)
{
  return GestureLib::isEnlarged(sensorData);
}

bool Gesture::shrinkChecker(SensorData& sensorData)
{
  return GestureLib::isShrinked(sensorData);
}

bool Gesture::movementChecker(SensorData& sensorData)
{
  bool ret = false;
  std::int32_t evidence = GestureLib::convertMovementEvidence(m_evidence);
  std::int32_t orientation = GestureLib::convertOrientation(sensorData.orientation);
  std::int32_t dirValue = (evidence + orientation) & 0x3; // mod 4
  LOG("movementChecker is invoked, evidence: " << evidence << " orientation: " << orientation << " dir: " << dirValue);

  switch (dirValue)
  {
    case 0:
      ret = GestureLib::isMoveUp(sensorData);
      break;
    case 1:
      ret = GestureLib::isMoveLeft(sensorData);
      break;
    case 2:
      ret = GestureLib::isMoveDown(sensorData);
      break;
    case 3:
      ret = GestureLib::isMoveRight(sensorData);
      break;
  }
  return ret;
}

bool Gesture::performAction(void)
{
  system(m_action.c_str());
  return true;
}

void Gesture::dump()
{
  LOG(m_name);
}
