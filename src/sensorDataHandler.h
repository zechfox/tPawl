/*=============================
/* Sensor Data Handler
/*
/*
/*===========================*/
#include <iostream>
#include <vector>
#include <memory>
#include <math.h>
#include <fstream>
#include <map>
 
using namespace std;

#define ACCELEROMETER_PATH "/sys/bus/iio/devices/"
#define ACCELEROMETER_DEVICE "iio:device0/"
#define ACCELEROMETER_X_RAW_DATA "in_accel_x_raw"
#define ACCELEROMETER_Y_RAW_DATA "in_accel_y_raw"
#define ACCELEROMETER_RAW_DATA_FACTOR (0.000009806)
#define ACCELEROMETER_THRESHOLD (7.0)

#define MONITOR_NAME "DSI-1"
#define ROTATE_COMMAND "xrandr --output {} --rotate {}"

struct CoordinatorData
{
  std::uint32_t x;
  std::uint32_t y;
};

enum class Orientation
{
  NORMAL,
  LEFT_UP,
  RIGHT_UP,
  BOTTOM_UP
};

class TouchPanelDevice
{
  public:
    TouchPanelDevice();
    ~TouchPanelDevice();

};

class Accelerometer
{
  public:
    Accelerometer()
    {
      m_accPathX.append(ACCELEROMETER_PATH);
      m_accPathX.append(ACCELEROMETER_DEVICE);
      m_accPathX.append(ACCELEROMETER_X_RAW_DATA);

      m_accPathY.append(ACCELEROMETER_PATH);
      m_accPathY.append(ACCELEROMETER_DEVICE);
      m_accPathY.append(ACCELEROMETER_X_RAW_DATA);

      m_accRawDataX.open(m_accPathX, ios::in);

      m_accRawDataY.open(m_accPathY, ios::in);
    }
    ~Accelerometer()
    {
      m_accRawDataX.close();
      m_accRawDataY.close();
    }

    CoordinatorData read()
    {
      std::uint32_t xRawData = 0;
      std::uint32_t yRawData = 0;
      if (m_accRawDataX)
      {
        m_accRawDataX.clear();
        m_accRawDataX.seekg(0);
        m_accRawDataX >> xRawData;
      }
      else
      {
        cout << "Open Accelerometer X Failed." << endl;
      }

      if (m_accRawDataY)
      {
        m_accRawDataY.clear();
        m_accRawDataY.seekg(0);
        m_accRawDataY >> yRawData;
      }
      else
      {
        cout << "Open Accelerometer Y Failed." << endl;
      }

      CoordinatorData data{xRawData, yRawData};
      return data;
    }
  private:
    std::string m_accPathX;
    std::string m_accPathY;
    std::ifstream m_accRawDataX;
    std::ifstream m_accRawDataY;

};

struct SensorData
{
  std::uint32_t fingerNumber;
  Orientation orientation;
  std::vector<CoordinatorData> coordinatorsData;
};

class SensorDataHandler
{
  public:
    SensorDataHandler();
    ~SensorDataHandler();

    std::shared_ptr<SensorData> getSensorData() const;
    void registerTouchPanelDevice(std::shared_ptr<TouchPanelDevice> touchPanel_p);
    void registerAccelerometer(std::shared_ptr<Accelerometer> accelerometer_p);

  private:
    Orientation getOrientation() const;
    void rotateScreen(Orientation orientation) const;
    std::shared_ptr<TouchPanelDevice> m_touchPanel_p;
    std::shared_ptr<Accelerometer> m_accelerometer_p;
    std::map<Orientation, std::string> m_rotateCommand;
};

