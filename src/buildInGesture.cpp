/*=============================
/* Gesture
/*
/*
/*===========================*/

#include "typeDef.h"
#include "buildInGesture.h"
#include "gestureLib.h" 
#include "log.h"
#include "XLibApi.h"

using namespace std;

bool BuildInGesture::invite(SensorData& sensorData)
{
  BuildInAction action = BuildInAction::NOT_AVAILABLE;

  if (1 == sensorData.fingerNumber)
  {
    action = getOneFingerAction(sensorData);
  }
  else if (2 == sensorData.fingerNumber)
  {
    action = getTwoFingerAction(sensorData);
  }

  m_action = action;
  return m_action != BuildInAction::NOT_AVAILABLE;
}

bool BuildInGesture::performAction(void)
{
  std::string command;
  KeyState keyState;
  BuildInKey key;
  CoordinatorData coordinatorData;
  switch (m_action)
  {
    case BuildInAction::ONE_FINGER_UP:
      command = "echo \"1 finger up\"";
      keyState = KeyState::PRESS;
      key = BuildInKey::WHEEL_UP;
      XLibApi::getInstance()->sendMouseEvent(keyState, key, coordinatorData);
      break;
    case BuildInAction::ONE_FINGER_LEFT:
      command = "echo \"1 finger left\"";
      break;
    case BuildInAction::ONE_FINGER_DOWN:
      command = "echo \"1 finger down\"";
      break;
    case BuildInAction::ONE_FINGER_RIGHT:
      command = "echo \"1 finger right\"";
      break;
    case BuildInAction::PRESS:
      command = "echo \"1 finger press\"";
      keyState = KeyState::PRESS;
      key = BuildInKey::LEFT_CLICK;
      XLibApi::getInstance()->sendMouseEvent(keyState, key, coordinatorData);
      break;
    case BuildInAction::SHORT_PRESS:
      command = "echo \"1 finger short press\"";
      break;
    case BuildInAction::LONG_PRESS:
      command = "echo \"1 finger long press\"";
      break;
    case BuildInAction::TWO_FINGER_ENLARGED:
      command = "echo \"2 finger enlarged\"";
      break;
    case BuildInAction::TWO_FINGER_SHRINKED:
      command = "echo \"2 finger shrinked\"";
      break;
    default:
      break;
  }
  system(command.c_str());
  return true;
}

BuildInAction BuildInGesture::getOneFingerAction(SensorData& sensorData)
{
  BuildInAction action = BuildInAction::NOT_AVAILABLE;
  // wrong element order will cause problem. 
  BuildInAction moveAction[4] = {BuildInAction::ONE_FINGER_UP,
    BuildInAction::ONE_FINGER_LEFT,
    BuildInAction::ONE_FINGER_DOWN,
    BuildInAction::ONE_FINGER_RIGHT};

  std::pair<PressType, CoordinatorData> pressData{PressType::NOT_AVAILABLE, {0, 0}};
  pressData = GestureLib::isPressed(sensorData);

  if (PressType::NOT_AVAILABLE != pressData.first)
  {
    if (PressType::PRESS == pressData.first)
    {
      action = BuildInAction::PRESS;
    }
    else if (PressType::SHORT_PRESS == pressData.first)
    {
      action = BuildInAction::SHORT_PRESS;
    }
    else if (PressType::LONG_PRESS == pressData.first)
    {
      action = BuildInAction::LONG_PRESS;
    }
  }
  else if (GestureLib::isMoveUp(sensorData))
  {
    std::int32_t evidence = GestureLib::convertMovementEvidence(Evidence::MOVE_UP);
    std::int32_t orientation = GestureLib::convertOrientation(sensorData.orientation);
    action = moveAction[(4 + evidence - orientation) & 0x3]; // mod 4
  }
  else if (GestureLib::isMoveDown(sensorData))
  {
    std::int32_t evidence = GestureLib::convertMovementEvidence(Evidence::MOVE_DOWN);
    std::int32_t orientation = GestureLib::convertOrientation(sensorData.orientation);
    action = moveAction[(4 + evidence - orientation) & 0x3]; // mod 4
  }
  else if (GestureLib::isMoveRight(sensorData))
  {
    std::int32_t evidence = GestureLib::convertMovementEvidence(Evidence::MOVE_RIGHT);
    std::int32_t orientation = GestureLib::convertOrientation(sensorData.orientation);
    action = moveAction[(4 + evidence - orientation) & 0x3]; // mod 4
  }
  else if (GestureLib::isMoveLeft(sensorData))
  {
    std::int32_t evidence = GestureLib::convertMovementEvidence(Evidence::MOVE_LEFT);
    std::int32_t orientation = GestureLib::convertOrientation(sensorData.orientation);
    action = moveAction[(4 + evidence - orientation) & 0x3]; // mod 4
  }

  return action;
}

BuildInAction BuildInGesture::getTwoFingerAction(SensorData& sensorData)
{
  BuildInAction action = BuildInAction::NOT_AVAILABLE;
  if (GestureLib::isEnlarged(sensorData))
  {
    action = BuildInAction::TWO_FINGER_ENLARGED;
  }
  else if (GestureLib::isShrinked(sensorData))
  {
    action = BuildInAction::TWO_FINGER_SHRINKED;
  }

  return action;
}

void BuildInGesture::dump()
{
  LOG(m_name);
}
