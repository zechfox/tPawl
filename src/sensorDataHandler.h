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

#define ROTATE_COMMAND "xrandr --output "
struct CoordinatorData
{
  std::uint32_t x;
  std::uint32_t y;
};

enum class Orientation
{
  NORMAL,
  LEFT,
  RIGHT,
  INVERT 
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
    Accelerometer(std::string& path, std::string& dev, std::string& x, std::string& y)
    {
      m_accPathX.append(path);
      m_accPathX.append(dev);
      m_accPathX.append(x);

      m_accPathY.append(path);
      m_accPathY.append(dev);
      m_accPathY.append(y);

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
    SensorDataHandler(std::string& monitorName,
                      float accRawDataFactor,
                      float accThreshold);
    ~SensorDataHandler();

    std::shared_ptr<SensorData> getSensorData() const;
    void registerTouchPanelDevice(std::shared_ptr<TouchPanelDevice> touchPanelPtr);
    void registerAccelerometer(std::shared_ptr<Accelerometer> accelerometerPtr);

  private:
    Orientation getOrientation() const;
    void rotateScreen(Orientation orientation) const;
    std::shared_ptr<TouchPanelDevice> m_touchPanel_p;
    std::shared_ptr<Accelerometer> m_accelerometer_p;
    std::map<Orientation, std::string> m_rotateCommand;
    float m_accRawDataFactor;
    float m_accThreshold;
};

