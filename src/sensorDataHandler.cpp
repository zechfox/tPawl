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

void SensorDataHandler::processSensorData()
{
  while (true)
  {
    int ret = -1;
    if (m_sensorFds.size() > 0)
    {
      ret = poll(&m_sensorFds[0], 1, 2000);
    }
    input_event inputEventData;
    auto orientation = getOrientation();
    rotateScreen(orientation);
    if (ret > 0)
    {
      if (m_sensorFds[0].revents)
      {
        ssize_t size = read(m_sensorFds[0].fd, &inputEventData, sizeof(inputEventData));
        if (size >= 0)
        {
          analyzeGesture(inputEventData);

        }
      }
    }
    // error or timeout
    else
    {

    }
  }

}

void SensorDataHandler::analyzeGesture(struct input_event& inputEventData)
{

}


std::shared_ptr<SensorData> SensorDataHandler::getSensorData() const
{
  auto sensorData_p = std::make_shared<SensorData>();
  sensorData_p->orientation = getOrientation();

  auto orientation = getOrientation();
  rotateScreen(orientation);

  return sensorData_p;
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
