/*=============================
/* Sensor Data Handler
/*
/*
/*===========================*/
#include <iostream>
#include <sstream>
#include "sensorDataHandler.h"
 
using namespace std;

SensorDataHandler::SensorDataHandler()
{
  std::stringstream ss;
  ss << ROTATE_COMMAND;
  m_rotateCommand[Orientation::NORMAL] = ss.str();

}

SensorDataHandler::~SensorDataHandler()
{

}

std::shared_ptr<SensorData> SensorDataHandler::getSensorData() const
{
  auto sensorData_p = std::make_shared<SensorData>();
  sensorData_p->orientation = getOrientation();

  return sensorData_p;
}

Orientation SensorDataHandler::getOrientation() const
{
  CoordinatorData data = m_accelerometer_p->read();
  Orientation orientation = Orientation::NORMAL;
  double accelerometerDataX = data.x * ACCELEROMETER_RAW_DATA_FACTOR;
  double accelerometerDataY = data.y * ACCELEROMETER_RAW_DATA_FACTOR;

  if (accelerometerDataX < ACCELEROMETER_THRESHOLD && accelerometerDataY < -ACCELEROMETER_THRESHOLD)
  {
    orientation = Orientation::NORMAL; /* 0 is normal orientation */
  }
  else if (accelerometerDataX > ACCELEROMETER_THRESHOLD && accelerometerDataY < ACCELEROMETER_THRESHOLD)
  {
    orientation = Orientation::RIGHT_UP; /* 1 is rotated right*/
  }
  else if (accelerometerDataX < ACCELEROMETER_THRESHOLD && accelerometerDataY > ACCELEROMETER_THRESHOLD )
  {
    orientation = Orientation::BOTTOM_UP; /* 2 is upside down*/
  }
  else if (accelerometerDataX < -ACCELEROMETER_THRESHOLD && accelerometerDataY < ACCELEROMETER_THRESHOLD)
  {
    orientation = Orientation::LEFT_UP; /* 3 is rotated left*/
  }

  return orientation;
}

void SensorDataHandler::rotateScreen(Orientation orientation) const
{

}

