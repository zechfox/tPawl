/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#include <iostream>
#include "configParser.h"
 
using namespace std;

ConfigParser::ConfigParser(void)
{
  m_confData.accPath = ACCELEROMETER_PATH;
  m_confData.accDevice = ACCELEROMETER_DEVICE;
  m_confData.accRawDataX = ACCELEROMETER_X_RAW_DATA;
  m_confData.accRawDataY = ACCELEROMETER_Y_RAW_DATA;
  m_confData.monitorName = MONITOR_NAME;
  m_confData.accRawDataFactor = ACCELEROMETER_RAW_DATA_FACTOR;
  m_confData.accThreshold = ACCELEROMETER_THRESHOLD;
}

ConfigParser::~ConfigParser(void)
{

}

ConfigurationData& ConfigParser::parseConfig()
{
  return m_confData;
}

