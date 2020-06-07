/*=============================
/* Sensor Data Handler
/*
/*
/*===========================*/
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "sensorDataHandler.h"
 
using namespace std;

SensorDataHandler::SensorDataHandler(std::string& monitorName,
                                     float accRawDataFactor,
                                     float accThreshold)
{
  std::ostringstream normalCommand, leftCommand, rightCommand, invertCommand;
  normalCommand << ROTATE_COMMAND << monitorName << " --rotate " << "normal";
  leftCommand << ROTATE_COMMAND << monitorName << " --rotate " << "left";
  rightCommand << ROTATE_COMMAND << monitorName << " --rotate " << "right";
  invertCommand << ROTATE_COMMAND << monitorName << " --rotate " << "invert";

  m_rotateCommand[Orientation::NORMAL] = normalCommand.str();
  m_rotateCommand[Orientation::LEFT] = leftCommand.str();
  m_rotateCommand[Orientation::RIGHT] = rightCommand.str();
  m_rotateCommand[Orientation::INVERT] = invertCommand.str();

  m_accRawDataFactor = accRawDataFactor;
  m_accThreshold = accThreshold;
}

SensorDataHandler::~SensorDataHandler()
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

  if (accelerometerDataX < m_accThreshold && accelerometerDataY < -m_accThreshold)
  {
    orientation = Orientation::NORMAL; /* 0 is normal orientation */
  }
  else if (accelerometerDataX > m_accThreshold && accelerometerDataY < m_accThreshold)
  {
    orientation = Orientation::RIGHT; /* 1 is rotated right*/
  }
  else if (accelerometerDataX < m_accThreshold && accelerometerDataY > m_accThreshold)
  {
    orientation = Orientation::INVERT; /* 2 is upside down*/
  }
  else if (accelerometerDataX < -m_accThreshold && accelerometerDataY < m_accThreshold)
  {
    orientation = Orientation::LEFT; /* 3 is rotated left*/
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
