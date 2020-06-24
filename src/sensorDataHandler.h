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

#include "typeDef.h"
#include "log.h"
#pragma once 
using namespace std;



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



class SensorDataHandler
{
  public:
    SensorDataHandler(std::string& monitorName,
                      float accRawDataFactor,
                      float accThreshold,
                      std::string& touchScreenDevName);
    ~SensorDataHandler();

    bool fillSensorData(SensorData& sensorData);
    void registerAccelerometer(std::shared_ptr<Accelerometer> accelerometerPtr);
    void rotateScreen(Orientation orientation) const;
    Orientation getOrientation() const;
  private:
    bool collectEventData(struct input_event& inputEventData, SensorData& sensorDataPtr);
    std::string getTouchScreenDevicePath(std::string& touchScreenName);
    void setTouchScreenDeviceProps(std::string property, std::int32_t value);
    std::shared_ptr<Accelerometer> m_accelerometer_p;
    std::map<Orientation, std::string> m_rotateCommand;
    std::string m_mapInputOutputCommand;
    SlotSpace m_slotSpace;
    float m_accRawDataFactor;
    float m_accThreshold;
    std::string m_touchScreenName;
    std::vector<pollfd> m_sensorFds;
};

