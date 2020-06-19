/*=============================
/* Gesture
/*
/*
/*===========================*/
#include <iostream>
#include <algorithm>
#include <execution>
#include <numeric>
#include <cmath>
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
  if (m_evidenceChecker)
  {
    return std::invoke(m_evidenceChecker, this, sensorData);
  }
  return false;
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
      checkerPtr = &Gesture::isEnlarged;
      break;
    case Evidence::SHRINK:
      checkerPtr = &Gesture::isShrinked;
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
  std::int32_t dirValue = (evidence + orientation) & 0x3; // mod 4
  LOG("movementChecker is invoked, evidence: " << evidence << " orientation: " << orientation << " dir: " << dirValue);

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

bool Gesture::isPoint(std::vector<CoordinatorData> coordinatorsData)
{
  auto first = coordinatorsData.front();
  auto last = coordinatorsData.back();
/*
  for (auto coordinatorData : coordinatorsData)
  {
    LOG("CoordinatorData X: " << coordinatorData.x << " Y: " << coordinatorData.y);
  }
  */
  if (abs(first.x - last.x) < 20
      && (abs(first.y - last.y) < 20))
  {
    LOG("Point!");
    return true;
  }
  return false;
}

bool Gesture::isHorizontalMove(std::vector<CoordinatorData> coordinatorsData)
{
  auto first = coordinatorsData.front();
  auto last = coordinatorsData.back();

  if (abs(first.x - last.x) > abs(first.y - last.y))
  {
    return true;
  }
  LOG("NOT Horizontal Movement.");
  return false;
}

bool Gesture::isVerticalMove(std::vector<CoordinatorData> coordinatorsData)
{
  auto first = coordinatorsData.front();
  auto last = coordinatorsData.back();

  if (abs(first.x - last.x) < abs(first.y - last.y))
  {
    return true;
  }
  LOG("NOT Vertical Movement.");
  return false;
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
  auto isGreater = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    if (isPoint(coordinatorsData.second)
        || isHorizontalMove(coordinatorsData.second))
    {
      return false;
    }
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
  auto isSmaller = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    if (isPoint(coordinatorsData.second)
        || isHorizontalMove(coordinatorsData.second))
    {
      return false;
    }
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
  auto isSmaller = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    if (isPoint(coordinatorsData.second)
        || isVerticalMove(coordinatorsData.second))
    {
      return false;
    }
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
  auto isGreater = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData) {
    if (isPoint(coordinatorsData.second)
        || isVerticalMove(coordinatorsData.second))
    {
      return false;
    }
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
  std::int32_t smallDistance = INT32_MAX;
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 3;

  auto pointChecker = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> data) {
    return isPoint(data.second);
  };
  bool isAnyPoint = std::any_of(sensorData.coordinatorsData.begin(),
                                sensorData.coordinatorsData.end(),
                                pointChecker);
  if (isAnyPoint)
  {
    return false;
  }
    
  auto isShrink = [&] (CoordinatorData &first, CoordinatorData &second) {
    std::int32_t distance = std::abs(first.x - second.x) + std::abs(first.y - second.y);
    if (distance <= smallDistance)
    {
      smallDistance = distance;
    }
    else
    {
      sensetive--;
      smallDistance = distance;
    }
    return sensetive > 0;
  };

  auto isShrinkedVec = [&] (std::pair<std::uint32_t, std::vector<CoordinatorData>> first,
                            std::pair<std::uint32_t, std::vector<CoordinatorData>> second) {
    smallDistance = INT32_MAX;
    if (first.second.size() < second.second.size())
    {
      sensetive = first.second.size() >> 2;
      return std::equal(first.second.begin(), first.second.end(), second.second.begin(), isShrink);
    }
    else
    {
      sensetive = second.second.size() >> 2;
      return std::equal(second.second.begin(), second.second.end(), first.second.begin(), isShrink);
    }

  };

  // find coordinatorsData that shrinked.
  auto iter = std::adjacent_find(sensorData.coordinatorsData.begin(),
                                 sensorData.coordinatorsData.end(),
                                 isShrinkedVec);

  return (iter == sensorData.coordinatorsData.end());
}

bool Gesture::isShrinked(SensorData& sensorData)
{
  std::int32_t largeDistance = INT32_MIN;
  // TODO: configure sesertive by config file.
  std::int32_t sensetive = 3;
  auto pointChecker = [=, this] (std::pair<std::uint32_t, std::vector<CoordinatorData>> data) {
    return isPoint(data.second);
  };
  bool isAnyPoint = std::any_of(sensorData.coordinatorsData.begin(),
                                sensorData.coordinatorsData.end(),
                                pointChecker);
  if (isAnyPoint)
  {
    return false;
  }
  auto isEnlarge = [&] (CoordinatorData &first, CoordinatorData &second) {
    std::int32_t distance = std::abs(first.x - second.x) + std::abs(first.y - second.y);

    if (distance >= largeDistance)
    {
      largeDistance = distance;
    }
    else
    {
      sensetive--;
      largeDistance = distance;
    }
    return sensetive > 0;
  };

  auto isEnlargedVec = [&] (std::pair<std::uint32_t, std::vector<CoordinatorData>> first,
                            std::pair<std::uint32_t, std::vector<CoordinatorData>> second) {
    largeDistance = INT32_MIN;
    if (first.second.size() < second.second.size())
    {
      // it's difficult to idendify enlarge.
      // tolerent more than shrink.
      sensetive = first.second.size() >> 1;
      return std::equal(first.second.begin(), first.second.end(), second.second.begin(), isEnlarge);
    }
    else
    {
      sensetive = first.second.size() >> 1;
      return std::equal(second.second.begin(), second.second.end(), first.second.begin(), isEnlarge);
    }
  };
  // find coordinatorsData that enlarged.
  auto iter = std::adjacent_find(sensorData.coordinatorsData.begin(),
                                 sensorData.coordinatorsData.end(),
                                 isEnlargedVec);

  return (iter == sensorData.coordinatorsData.end());
}

bool Gesture::performAction(void)
{
  system(m_action.c_str());
  return true;
}
