/*=============================
/* Type definition Header
/*
/*
/*===========================*/
#pragma once
#include <map>
#include <vector>
#include <string>
using namespace std;
/*=====================
//  Macro
//=====================*/

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

#define ROTATE_COMMAND "xrandr --output "
#define LOG_FILE "/var/log/tPawl.log"

#define MAP_COMMAND "xinput --map-to-output "

/*=====================
// enum class
//=====================*/

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
  PRESS,
  NOT_AVAILIABLE
};

enum class Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class PressType
{
  SHORT_PRESS,
  LONG_PRESS,
  PRESS,
  NOT_AVAILABLE
};

enum class Orientation
{
  NORMAL,
  LEFT,
  RIGHT,
  INVERT,
  NOT_AVAILABLE
};

enum class BuildInAction
{
  PRESS,
  SHORT_PRESS,
  LONG_PRESS,
  ONE_FINGER_UP,
  ONE_FINGER_DOWN,
  ONE_FINGER_LEFT,
  ONE_FINGER_RIGHT,
  TWO_FINGER_ENLARGED,
  TWO_FINGER_SHRINKED,
  NOT_AVAILABLE
};

enum class BuildInKey
{
  MOUSE_MOVE,
  LEFT_CLICK,
  RIGHT_CLICK,
  WHEEL_UP,
  WHEEL_DOWN,
  NOT_AVAILABLE
};

enum class KeyState
{
  NONE,
  PRESS,
  RELEASE
};
/*=====================
// Data struct
//=====================*/
//
struct CoordinatorData
{
  std::int32_t x;
  std::int32_t y;
};

struct SensorData
{
  std::uint32_t fingerNumber;
  bool isFingerLeft;
  Orientation orientation;
  std::map<std::uint32_t, std::vector<CoordinatorData>> coordinatorsData;
};

struct SlotSpace
{
  std::int32_t currentSlotNumber;
  std::int32_t positionX;
  std::int32_t positionY;
  // slotNumber -> trackId
  std::map<std::int32_t, std::int32_t> activatedSlots;
};

struct GestureData
{
  std::uint32_t touchPointNumber;
  Evidence evidence;
  std::string action;
  std::string name;
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
  std::string logFile;
  float accRawDataFactor;
  float accThreshold;
  std::vector<GestureData> gestures;
};




