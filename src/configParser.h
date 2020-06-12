/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#pragma once

#include <iostream>
#include <vector>
 
using namespace std;

#define ACCELEROMETER_PATH "/sys/bus/iio/devices/"
#define ACCELEROMETER_DEVICE "iio:device0/"
#define ACCELEROMETER_X_RAW_DATA "in_accel_x_raw"
#define ACCELEROMETER_Y_RAW_DATA "in_accel_y_raw"
#define ACCELEROMETER_SCALE "in_accel_scale"
#define ACCELEROMETER_RAW_DATA_FACTOR (0.000009806)
#define ACCELEROMETER_THRESHOLD (7.0)

#define TOUCHSCREEN_DEVICE_PATH "/dev/input/event20"
#define TOUCHSCREEN_DEVICE_NAME "silead_ts"
#define MONITOR_NAME "DSI-1"

enum class Evidence
{
  // modify Gesture::assignCheck if add item
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_TOP,
  MOVE_BOTTOM,
  MOVE_LEFT_SIDE,
  MOVE_RIGHT_SIDE,
  ENLARGE,
  SHRINK,
  NOT_AVALIABLE
};

struct GestureData
{
  std::uint32_t touchPointNumber;
  Evidence evidence;
  std::string action;
};

struct ConfigurationData
{
  std::string accPath;
  std::string accDevice;
  std::string accRawDataX;
  std::string accRawDataY;
  std::string monitorName;
  std::string accScale;
  std::string tsDevPath;
  std::string tsDevName;
  float accRawDataFactor;
  float accThreshold;
  std::vector<GestureData> gestures;
};

class ConfigParser
{
  public:
    ConfigParser();
    ~ConfigParser();

    ConfigurationData& parseConfig();
  private:
    ConfigurationData m_confData;

};
