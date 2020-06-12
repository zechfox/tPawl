/*=============================
/* Gesture
/*
/*
/*===========================*/
#include <iostream>
#include <algorithm>
#include <execution>
#include <numeric>
#include <stdlib.h>

#include "gesture.h"
 
using namespace std;

Gesture::Gesture(GestureData& gestureData):
  m_touchPointNumber(gestureData.touchPointNumber),
  m_evidence(gestureData.evidence),
  m_action(gestureData.action)
{
  m_evidenceChecker = assignChecker(m_evidence);
}

Gesture::~Gesture()
{

}

bool Gesture::invite(SensorData& sensorData)
{

  return true;
}

std::int32_t Gesture::convertOrientation(Orientation orientation)
{
  std::int32_t value = 0;
  switch (orientation)
  {
    case Orientation::NORMAL:
      value = 0;
      break;
    case Orientation::LEFT:
      value = 1;
      break;
    case Orientation::INVERT:
      value = 2;
      break;
    case Orientation::RIGHT:
      value = 3;
      break;
  }
  return value;
}

std::int32_t Gesture::convertMovementEvidence(Evidence evidence)
{
  std::int32_t value = 0;
  switch (evidence)
  {
    case Evidence::MOVE_UP:
      value = 0;
      break;
    case Evidence::MOVE_LEFT:
      value = 1;
      break;
    case Evidence::MOVE_DOWN:
      value = 2;
      break;
    case Evidence::MOVE_RIGHT:
      value = 3;
      break;
  }
  return value;
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
    case Evidence::SHRINK:
      checkerPtr = &Gesture::distanceChecker;
      break;
    default:
      checkerPtr = &Gesture::movementChecker;
      break;
  }
  return checkerPtr;
}

bool Gesture::movementChecker(SensorData& sensorData)
{
  bool ret = false;
  std::int32_t evidence = convertMovementEvidence(m_evidence);
  std::int32_t orientation = convertOrientation(sensorData.orientation);
  std::int32_t dirValue = evidence + orientation;

  switch (dirValue)
  {
    case 0:
      ret = isMoveUp(sensorData);
      break;
    case 1:
      ret = isMoveLeft(sensorData);
      break;
    case 2:
      ret = isMoveDown(sensorData);
      break;
    case 3:
      ret = isMoveRight(sensorData);
      break;
  }
  return ret;
}

bool Gesture::distanceChecker(SensorData& sensorData)
{

  return true;
}

// TODO: most of movement code was same,
// use more common code.
bool Gesture::isMoveDown(SensorData& sensorData)
{
  // Down direction only care about y.
  // first y data
  std::int32_t greater = sensorData.coordinatorsData.at(0).at(0).y; 
  // allow some data less than previous
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 4;

  // y increase
  auto largerThan = [&] (CoordinatorData data) {
    if (greater < data.y)
    {
      greater = data.y;
      return true;
    }
    // always update greater, due to we measure the trend.
    // for example, one touch point was left screen, and contact
    // the screen again, the new touch point maybe lower than before leave.
    // We also treat it as down trend if up coming touch pointer above it.
    greater = data.y;
    return false;
  };
  auto isGreater = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    auto result = std::count_if(coordinatorsData.second.begin(),
                                coordinatorsData.second.end(),
                                largerThan);
    if ((result + sensetive) > coordinatorsData.second.size())
    {
      return true;
    }
    return false;
  };

  // all touch point should be moved down.
  return std::all_of(sensorData.coordinatorsData.begin(),
                     sensorData.coordinatorsData.end(),
                     isGreater);
}

bool Gesture::isMoveUp(SensorData& sensorData)
{
  // Up direction only care about y.
  // first y data
  std::int32_t smaller = sensorData.coordinatorsData.at(0).at(0).y; 
  // allow some data greater than previous
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 4;

  // y decrease;
  auto smallerThan = [&] (CoordinatorData data) {
    if (smaller > data.y)
    {
      smaller = data.y;
      return true;
    }
    // always update greater, due to we measure the trend.
    // for example, one touch point was left screen, and contact
    // the screen again, the new touch point maybe greater than before leave.
    // We also treat it as up trend if up coming touch pointer below it.
    smaller = data.y;
    return false;
  };
  auto isSmaller = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    auto result = std::count_if(coordinatorsData.second.begin(),
                                coordinatorsData.second.end(),
                                smallerThan);
    if ((result + sensetive) > coordinatorsData.second.size())
    {
      return true;
    }
    return false;
  };

  // all touch point should be moved up.
  return std::all_of(sensorData.coordinatorsData.begin(),
                     sensorData.coordinatorsData.end(),
                     isSmaller);
}

bool Gesture::isMoveLeft(SensorData& sensorData)
{
  // Left direction only care about x.
  // first x data
  std::int32_t smaller = sensorData.coordinatorsData.at(0).at(0).x; 
  // allow some data greater than previous
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 4;

  // x decrease
  auto smallerThan = [&] (CoordinatorData data) {
    if (smaller > data.x)
    {
      smaller = data.x;
      return true;
    }
    // always update greater, due to we measure the trend.
    // for example, one touch point was left screen, and contact
    // the screen again, the new touch point may be moved left.
    // We also treat it as left trend if up coming touch pointer was left of it.
    smaller = data.x;
    return false;
  };
  auto isSmaller = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    auto result = std::count_if(coordinatorsData.second.begin(),
                                coordinatorsData.second.end(),
                                smallerThan);
    if ((result + sensetive) > coordinatorsData.second.size())
    {
      return true;
    }
    return false;
  };

  // all touch point should be moved down.
  return std::all_of(sensorData.coordinatorsData.begin(),
                     sensorData.coordinatorsData.end(),
                     isSmaller);
}

bool Gesture::isMoveRight(SensorData& sensorData)
{
  // Right direction only care about x.
  // first x data
  std::int32_t greater = sensorData.coordinatorsData.at(0).at(0).x; 
  // allow some data less than previous
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 4;

  // x increase
  auto largerThan = [&] (CoordinatorData data) {
    if (greater < data.x)
    {
      greater = data.x;
      return true;
    }
    // always update greater, due to we measure the trend.
    // for example, one touch point was left screen, and contact
    // the screen again, the new touch point may be moved right. 
    // We also treat it as right trend if up coming touch pointer was right of it.
    greater = data.x;
    return false;
  };
  auto isGreater = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    auto result = std::count_if(coordinatorsData.second.begin(),
                                coordinatorsData.second.end(),
                                largerThan);
    if ((result + sensetive) > coordinatorsData.second.size())
    {
      return true;
    }
    return false;
  };

  // all touch point should be moved right.
  return std::all_of(sensorData.coordinatorsData.begin(),
                     sensorData.coordinatorsData.end(),
                     isGreater);
}

bool Gesture::isEnlarged(SensorData& sensorData)
{
  std::int32_t largeDistance = 0;
  std::vector<bool> distanceResult;
  auto isEnlarge = [&] (CoordinatorData &first, CoordinatorData &second) {
    std::int32_t distance = first.x - second.x + first.y - second.y;
    if (distance > largeDistance)
    {
      distance = largeDistance;
      return true;
    }
    return false;
  };
  auto isEnlargedVec = [&] (std::pair<std::uint32_t, std::vector<CoordinatorData>> *coordinatorsDataPtr) {
    std::vector<CoordinatorData>& first = coordinatorsDataPtr->second;
    std::vector<CoordinatorData>& second = (coordinatorsDataPtr++)->second;
    return std::equal(first.begin(), first.end(), second.begin(), second.end(), isEnlarge);
  };
  auto extractVector = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    return coordinatorsData.second;
  };

  for (auto itr = sensorData.coordinatorsData.begin();itr != sensorData.coordinatorsData.end();++itr)
  {

  }
    
    /*
  calcDistance(sensorData.coordinatorsData.at(0).at(0),
               sensorData.coordinatorsData.at(1).at(0));

  std::exclusive_scan(sensorData.coordinatorsData.begin(),
                      sensorData.coordinatorsData.end(),
                      distanceResult.begin(), 1,
                      isEnlargedVec);

               */
  return true;
}

bool Gesture::isShrinked(SensorData& sensorData)
{
  return true;
}

bool Gesture::performAction(void)
{
  system(m_action.c_str());
  return true;
}
