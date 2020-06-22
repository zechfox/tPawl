/*=============================
/* Sensor Data Handler
/*
/*
/*===========================*/
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "log.h"
#include "sensorDataHandler.h"
 
using namespace std;

SensorDataHandler::SensorDataHandler(std::string& monitorName,
                                     float accRawDataFactor,
                                     float accThreshold,
                                     std::string& touchScreenDevName)
{
  std::ostringstream normalCommand, leftCommand, rightCommand, invertCommand;
  normalCommand << ROTATE_COMMAND << monitorName << " --rotate " << "normal";
  leftCommand << ROTATE_COMMAND << monitorName << " --rotate " << "left";
  rightCommand << ROTATE_COMMAND << monitorName << " --rotate " << "right";
  invertCommand << ROTATE_COMMAND << monitorName << " --rotate " << "inverted";

  m_rotateCommand[Orientation::NORMAL] = normalCommand.str();
  m_rotateCommand[Orientation::LEFT] = leftCommand.str();
  m_rotateCommand[Orientation::RIGHT] = rightCommand.str();
  m_rotateCommand[Orientation::INVERT] = invertCommand.str();

  std::ostringstream mapCommand;
  mapCommand << MAP_COMMAND << "\"pointer:" << touchScreenDevName << "\" " << monitorName;
  m_mapInputOutputCommand = mapCommand.str();
  m_accRawDataFactor = accRawDataFactor;
  m_accThreshold = accThreshold;

  std::string tsDevPath = getTouchScreenDevicePath(touchScreenDevName);
  LOG("TouchScreen Path:" << tsDevPath);
  
  pollfd tmpFd;
  tmpFd.fd = open(tsDevPath.c_str(), O_RDONLY | O_NONBLOCK);
  if (tmpFd.fd > 0)
  {
    tmpFd.events = POLLIN;
    tmpFd.revents = 0;
    m_sensorFds.push_back(tmpFd);
  }
  else
  {
    LOG("Failed to open touch screen device.");
  }
  m_slotSpace.currentSlotNumber = 0;
  m_slotSpace.positionX = -1;
  m_slotSpace.positionY = -1;
  m_slotSpace.activatedSlots.clear();
}

SensorDataHandler::~SensorDataHandler()
{

}
// TODO: more elegant way.
std::string SensorDataHandler::getTouchScreenDevicePath(std::string& touchScreenDevName)
{
  std::ifstream procDevices;
  std::string line;
  std::ostringstream touchScreenDevPath;
  std::string eventName;
  bool findTouchScreen = false;
  procDevices.open("/proc/bus/input/devices", ios::in);
  while (std::getline(procDevices, line) && eventName.empty())
  {
    std::istringstream ss(line);
    std::string word;
    while (std::getline(ss, word, '='))
    {
      
      if (findTouchScreen)
      {
        std::istringstream itemValue(word);
        std::string value;
        while (std::getline(itemValue, value, ' '))
        {
          if (std::string::npos != value.find("event"))
          {
            eventName = value;
            break;
          }
        }
      }
      else if (std::string::npos != word.find(touchScreenDevName))
      {
        findTouchScreen = true;
        break;
      }
    }
  }

  procDevices.close();
  touchScreenDevPath << "/dev/input/" << eventName ;
  return touchScreenDevPath.str();


}

bool SensorDataHandler::fillSensorData(SensorData& sensorData)
{
  int ret = -1;
  bool isCollectDone = false;
  if (m_sensorFds.size() > 0)
  {
    // TODO: got 2000 by config: ROTATION_INTERVAL
    ret = poll(&m_sensorFds[0], 1, 2000);
  }
  
  input_event inputEventData;
  if (ret > 0)
  {
    if (m_sensorFds[0].revents)
    {
      ssize_t size = read(m_sensorFds[0].fd, &inputEventData, sizeof(inputEventData));
      if (size >= 0)
      {
        isCollectDone = collectEventData(inputEventData, sensorData);
      }
    }
  }
  // timeout
  // It's suppose that screen should not be rotated by user.
  else if (0 == ret)
  {
    sensorData.orientation = getOrientation();
    isCollectDone = true;
  }
  // TODO: error
  else
  {

  }

  return isCollectDone;
}

bool SensorDataHandler::collectEventData(input_event& inputEventData, SensorData& sensorData)
{
  // only care about ABS event.
  if (EV_ABS != inputEventData.type)
  {
    return false;
  }

  if (ABS_MT_SLOT == inputEventData.code)
  {
    if (m_slotSpace.currentSlotNumber != inputEventData.value)
    {
      m_slotSpace.currentSlotNumber = inputEventData.value;
    }
  }

  if (ABS_MT_POSITION_X == inputEventData.code)
  {
    m_slotSpace.positionX = inputEventData.value;
  }

  if (ABS_MT_POSITION_Y == inputEventData.code)
  {
    m_slotSpace.positionY = inputEventData.value;
  }

  // save data
  if (m_slotSpace.positionX != -1
      && m_slotSpace.positionY != -1)
  {
    CoordinatorData data{m_slotSpace.positionX, m_slotSpace.positionY};
    sensorData.coordinatorsData[m_slotSpace.currentSlotNumber].push_back(data);
    sensorData.fingerNumber = m_slotSpace.activatedSlots.size();
    m_slotSpace.positionX = -1;
    m_slotSpace.positionY = -1;
  }

  // 1 finger gesture
  if ((1 == sensorData.fingerNumber
       || 2 == sensorData.fingerNumber)
      && sensorData.coordinatorsData[0].size() > 15)
  {
    // currentSlotNumber always 0,
    // postionX and positionY was clear in above
    // code.
    return true;
  }

  if (ABS_MT_TRACKING_ID == inputEventData.code)
  {
    // 1 finger left screen
    if (-1 == inputEventData.value)
    {
      m_slotSpace.activatedSlots.erase(m_slotSpace.currentSlotNumber);
      // all finger left screen
      if (m_slotSpace.activatedSlots.empty())
      {
        sensorData.fingerNumber = sensorData.coordinatorsData.size();
        m_slotSpace.currentSlotNumber = 0;
        m_slotSpace.positionX = -1;
        m_slotSpace.positionY = -1;
        return true;
      }
    }
    else
    {
      m_slotSpace.activatedSlots[m_slotSpace.currentSlotNumber] = inputEventData.value;
      sensorData.fingerNumber = m_slotSpace.activatedSlots.size();
    }
  }

  return false;
}

Orientation SensorDataHandler::getOrientation() const
{
  CoordinatorData data = m_accelerometer_p->read();
  Orientation orientation = Orientation::NORMAL;
  double accelerometerDataX = data.x * m_accRawDataFactor;
  double accelerometerDataY = data.y * m_accRawDataFactor;

  if (accelerometerDataY < -m_accThreshold)
  {
    orientation = Orientation::INVERT;
  }
  else if (accelerometerDataX > m_accThreshold)
  {
    orientation = Orientation::LEFT;
  }
  else if (accelerometerDataY > m_accThreshold)
  {
    orientation = Orientation::NORMAL;
  }
  else if (accelerometerDataX < -m_accThreshold)
  {
    orientation = Orientation::RIGHT;
  }

  return orientation;
}

void SensorDataHandler::rotateScreen(Orientation orientation) const
{
  std::string command = m_rotateCommand.at(orientation);
  system(command.c_str());

  // xinput --map-to-output "pointer:silead_ts" DSI-1
  command = m_mapInputOutputCommand;
  LOG(command);
  system(command.c_str());
}

void SensorDataHandler::registerAccelerometer(std::shared_ptr<Accelerometer> accelerometerPtr)
{
  m_accelerometer_p = accelerometerPtr;
}
