/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#include <iostream>
 
using namespace std;

#define ACCELEROMETER_PATH "/sys/bus/iio/devices/"
#define ACCELEROMETER_DEVICE "iio:device0/"
#define ACCELEROMETER_X_RAW_DATA "in_accel_x_raw"
#define ACCELEROMETER_Y_RAW_DATA "in_accel_y_raw"
#define ACCELEROMETER_SCALE "in_accel_scale"
#define ACCELEROMETER_RAW_DATA_FACTOR (0.000009806)
#define ACCELEROMETER_THRESHOLD (7.0)

#define TOUCHSCREEN_DEVICE_PATH "/dev/input/xxxx"
#define MONITOR_NAME "DSI-1"

struct ConfigurationData
{
  std::string accPath;
  std::string accDevice;
  std::string accRawDataX;
  std::string accRawDataY;
  std::string monitorName;
  std::string accScale;
  std::string tsDevPath;
  float accRawDataFactor;
  float accThreshold;
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
