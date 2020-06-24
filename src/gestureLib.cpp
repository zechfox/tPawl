/*=============================
/* Gesture Lib
/*
/*
/*===========================*/
#include <iostream>
#include <algorithm>
#include <execution>
#include <numeric>
#include <cmath>
#include <stdlib.h>

#include "log.h"
#include "typeDef.h"
#include "gestureLib.h"

namespace GestureLib {
  std::int32_t convertOrientation(Orientation orientation)
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

  std::int32_t convertMovementEvidence(Evidence evidence)
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

  std::pair<PressType, CoordinatorData> isPressed(SensorData& sensorData)
  {
    bool ret = false;
    PressType pressType = PressType::NOT_AVAILABLE;
    LOG("isPressed");
    CoordinatorData pressPoint{0, 0};
    auto& pointData = sensorData.coordinatorsData[0];
    auto pointDataSize = pointData.size();

    auto sumCoordinatorData = [&](CoordinatorData coordinatorData) {
      pressPoint.x += coordinatorData.x;
      pressPoint.y += coordinatorData.y;
    };

    if (isPoint(pointData))
    {
      std::for_each(pointData.begin(),
                    pointData.end(),
                    sumCoordinatorData);
      pressPoint.x /= pointDataSize;
      pressPoint.y /= pointDataSize;
      if (pointDataSize < 5)
      {
        pressType = PressType::SHORT_PRESS;
        LOG("Short Pressed. center X: " << pressPoint.x << " center Y: " << pressPoint.y);
      }
      else if (pointDataSize < 15)
      {
        pressType = PressType::PRESS;
        LOG("Pressed. center X: " << pressPoint.x << " center Y: " << pressPoint.y);
      }
      else
      {
        pressType = PressType::LONG_PRESS;
        LOG("Long Pressed center X: " << pressPoint.x << " center Y: " << pressPoint.y);
      }
    }

    return std::make_pair(pressType, pressPoint);
  }

  bool isPoint(std::vector<CoordinatorData> coordinatorsData)
  {
    if (0 == coordinatorsData.size())
      return true;
    auto first = coordinatorsData.front();
    auto last = coordinatorsData.back();

    if (abs(first.x - last.x) < 20
        && (abs(first.y - last.y) < 20))
    {
      LOG("Point!");
      return true;
    }
    return false;
  }

  bool isHorizontalMove(std::vector<CoordinatorData> coordinatorsData)
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

  bool isVerticalMove(std::vector<CoordinatorData> coordinatorsData)
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
  bool isMoveDown(SensorData& sensorData)
  {
    // Down direction only care about y.
    // first y data
    std::int32_t greater = INT32_MIN; 
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

  bool isMoveUp(SensorData& sensorData)
  {
    // Up direction only care about y.
    // first y data
    std::int32_t smaller = INT32_MAX; 
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

  bool isMoveLeft(SensorData& sensorData)
  {
    // Left direction only care about x.
    // first x data
    std::int32_t smaller = INT32_MAX; 
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

  bool isMoveRight(SensorData& sensorData)
  {
    // Right direction only care about x.
    // first x data
    std::int32_t greater = INT32_MIN; 
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

  bool isShrinked(SensorData& sensorData)
  {
    // center point
    // distance from center point decreased.
    //
    std::int32_t smallDistance = INT32_MAX;
    // TODO: configure sesertive by config file.
    std::int32_t sensetive = 3;
    CoordinatorData centerPoint{0, 0};
    auto sumFirstElement = [&](std::pair<std::uint32_t, std::vector<CoordinatorData>> data) {
      centerPoint.x += data.second.front().x;
      centerPoint.y += data.second.front().y;
    };
    std::for_each(sensorData.coordinatorsData.begin(),
                  sensorData.coordinatorsData.end(),
                  sumFirstElement);
    centerPoint.x /= sensorData.fingerNumber;
    centerPoint.y /= sensorData.fingerNumber;
    LOG("CenterPoint: " << centerPoint.x << " : " << centerPoint.y);

    auto isShrink = [&] (CoordinatorData &point) {
      std::int32_t distance = std::abs(point.x - centerPoint.x) + std::abs(point.y - centerPoint.y);
      LOG("Distance " << distance);
      // 10 as filter
      if ((distance) <= smallDistance)
      {
        smallDistance = distance;
      }
      else
      {
        sensetive--;
        smallDistance = distance;
      }
      return sensetive >= 0;
    };

    auto isPointTraceShrinked = [&] (std::pair<std::uint32_t, std::vector<CoordinatorData>> pointTrace) {
      smallDistance = INT32_MAX;
      LOG("IsShrinked: " << pointTrace.first);
      sensetive = pointTrace.second.size() >> 2;
      return !isPoint(pointTrace.second) && std::all_of(pointTrace.second.begin(), pointTrace.second.end(), isShrink);
    };

    return std::any_of(sensorData.coordinatorsData.begin(),
                       sensorData.coordinatorsData.end(),
                       isPointTraceShrinked);
  }

  bool isEnlarged(SensorData& sensorData)
  {
    std::int32_t distance = 0;
    std::int32_t threshold = 100 * (sensorData.fingerNumber - 1);

    auto sumDistance = [&] (std::vector<CoordinatorData> &firstPointTrace, std::vector<CoordinatorData> &secondPointTrace) {
      std::int32_t startDistance = std::abs(firstPointTrace.front().x - secondPointTrace.front().x)
          + std::abs(firstPointTrace.front().y - secondPointTrace.front().y);
      std::int32_t endDistance = std::abs(firstPointTrace.back().x - secondPointTrace.back().x)
          + std::abs(firstPointTrace.back().y - secondPointTrace.back().y);
      distance += endDistance - startDistance;
      LOG("Start Distance: " << startDistance << " End Distance: " << endDistance << " Sum: " << distance);
    };

    auto isPointTraceEnlarged = [&] (std::pair<std::uint32_t, std::vector<CoordinatorData>> first,
                                     std::pair<std::uint32_t, std::vector<CoordinatorData>> second) {
      LOG("IsEnlarged: " << first.first << " <-> " << second.first);
      std::int32_t startDistance = std::abs(first.second.front().x - second.second.front().x)
          + std::abs(first.second.front().y - second.second.front().y);
      std::int32_t endDistance = std::abs(first.second.back().x - second.second.back().x)
          + std::abs(first.second.back().y - second.second.back().y);
      distance += endDistance - startDistance;
      LOG("Start Distance: " << startDistance << " End Distance: " << endDistance << " Sum: " << distance);
      return distance > threshold;
    };
    
    auto iter = std::adjacent_find(sensorData.coordinatorsData.begin(),
                                   sensorData.coordinatorsData.end(),
                                   isPointTraceEnlarged);
    return iter != sensorData.coordinatorsData.end();
  }
}
