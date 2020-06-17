/*=============================
/* Sensor Data Handler
/*
/*
/*===========================*/
#include <iostream>
#include <vector>
#include <memory>
#include <math.h>
#include <poll.h>
#include <fstream>
#include <map>

#include "log.h"
#pragma once 
using namespace std;

#define ROTATE_COMMAND "xrandr --output "
#define MAP_COMMAND "xinput --map-to-output "
struct CoordinatorData
{
  std::int32_t x;
  std::int32_t y;
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
      std::int32_t xRawData = 0;
      std::int32_t yRawData = 0;
      if (m_accRawDataX)
      {
        m_accRawDataX.clear();
        m_accRawDataX.seekg(0);
        m_accRawDataX >> xRawData;
      }
      else
      {
        LOG("Open Accelerometer X Failed." << m_accPathX);
      }

      if (m_accRawDataY)
      {
        m_accRawDataY.clear();
        m_accRawDataY.seekg(0);
        m_accRawDataY >> yRawData;
      }
      else
      {
        LOG("Open Accelerometer Y Failed." << m_accPathY);
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

class SensorDataHandler
{
  public:
    SensorDataHandler(std::string& monitorName,
                      float accRawDataFactor,
                      float accThreshold,
                      std::string& touchScreenDevName);
    ~SensorDataHandler();

    bool fillSensorData(SensorData& sensorData);
    void registerTouchPanelDevice(std::shared_ptr<TouchPanelDevice> touchPanelPtr);
    void registerAccelerometer(std::shared_ptr<Accelerometer> accelerometerPtr);
    void rotateScreen(Orientation orientation) const;
  private:
    Orientation getOrientation() const;
    bool collectEventData(struct input_event& inputEventData, SensorData& sensorDataPtr);
    std::string getTouchScreenDevicePath(std::string& touchScreenName);
    std::shared_ptr<TouchPanelDevice> m_touchPanel_p;
    std::shared_ptr<Accelerometer> m_accelerometer_p;
    std::map<Orientation, std::string> m_rotateCommand;
    std::string m_mapInputOutputCommand;
    SlotSpace m_slotSpace;
    float m_accRawDataFactor;
    float m_accThreshold;
    std::vector<pollfd> m_sensorFds;
};

