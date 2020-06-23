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

  bool isEnlarged(SensorData& sensorData)
  {
    std::int32_t smallDistance = INT32_MAX;
    // TODO: configure sesertive by config file.
    std::int32_t sensetive = 3;

    auto pointChecker = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> data) {
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

  bool isShrinked(SensorData& sensorData)
  {
    std::int32_t largeDistance = INT32_MIN;
    // TODO: configure sesertive by config file.
    std::int32_t sensetive = 3;
    auto pointChecker = [=] (std::pair<std::uint32_t, std::vector<CoordinatorData>> data) {
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
}
