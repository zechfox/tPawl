/*=============================
/* Gesture
/*
/*
/*===========================*/

#include "typeDef.h"
#include "buildInGesture.h"
#include "gestureLib.h" 
#include "log.h"

using namespace std;

bool BuildInGesture::invite(SensorData& sensorData)
{

  return false;
}

bool BuildInGesture::performAction(void)
{
  system(m_action.c_str());
  return true;
}
