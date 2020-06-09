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
#include "sensorDataHandler.h"
 
using namespace std;

SensorDataHandler::SensorDataHandler(std::string& monitorName,
                                     float accRawDataFactor,
                                     float accThreshold,
                                     std::string& touchScreenDevPath)
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

  m_accRawDataFactor = accRawDataFactor;
  m_accThreshold = accThreshold;

  pollfd tmpFd;
  tmpFd.fd = open(touchScreenDevPath.c_str(), O_RDONLY | O_NONBLOCK);
  if (tmpFd.fd > 0)
  {
    tmpFd.events = POLLIN;
    tmpFd.revents = 0;
    m_sensorFds.push_back(tmpFd);
  }
}

SensorDataHandler::~SensorDataHandler()
{

}

bool SensorDataHandler::fillSensorData(std::shared_ptr<SensorData> sensorDataPtr)
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
        isCollectDone = collectEventData(inputEventData, sensorDataPtr);
      }
    }
  }
  // timeout
  // It's suppose that screen should not be rotated by user.
  else if (0 == ret)
  {
    sensorDataPtr->orientation = getOrientation();
    isCollectDone = true;
  }
  // TODO: error
  else
  {

  }

  return isCollectDone;
}

bool SensorDataHandler::collectEventData(input_event& inputEventData, std::shared_ptr<SensorData> sensorDataPtr)
{
  // only care about ABS event.
  if (EV_ABS != inputEventData.type)
  {
    return false;
  }

  if (ABS_MT_SLOT == inputEventData.code)
  {
    std::uint32_t fingerIndex = inputEventData.value;
  }

  if (ABS_MT_TRACKING_ID == inputEventData.code)
  {
    if (-1 == inputEventData.value)
    {

    }
    else
    {
      sensorDataPtr->fingerNumber++;
    }

  }

  return true;
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
}

void SensorDataHandler::registerAccelerometer(std::shared_ptr<Accelerometer> accelerometerPtr)
{
  m_accelerometer_p = accelerometerPtr;
}
