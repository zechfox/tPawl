/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#include <iostream>
#include <fstream>

#include "configParser.h"
 
using namespace std;

ConfigParser::ConfigParser(void)
{
  m_confData.accPath = ACCELEROMETER_PATH;
  m_confData.accDevice = ACCELEROMETER_DEVICE;
  m_confData.accRawDataX = ACCELEROMETER_X_RAW_DATA;
  m_confData.accRawDataY = ACCELEROMETER_Y_RAW_DATA;
  m_confData.accScale = ACCELEROMETER_SCALE;
  m_confData.monitorName = MONITOR_NAME;
  m_confData.accThreshold = ACCELEROMETER_THRESHOLD;
  m_confData.tsDevPath = TOUCHSCREEN_DEVICE_PATH;
  m_confData.tsDevName = TOUCHSCREEN_DEVICE_NAME;

  std::ifstream accFactorStream;
  std::string accFactorPath;
  accFactorPath.append(m_confData.accPath);
  accFactorPath.append(m_confData.accDevice);
  accFactorPath.append(m_confData.accScale);
  accFactorStream.open(accFactorPath, ios::in);
  accFactorStream.clear();
  accFactorStream.seekg(0);

  accFactorStream >> m_confData.accRawDataFactor;
  accFactorStream.close();

  GestureData gesture1;
  gesture1.touchPointNumber = 2;
  gesture1.evidence = Evidence::MOVE_UP;
  gesture1.action = "echo \"2 finger action\"";
  m_confData.gestures.push_back(gesture1);

  m_confData.logFile = LOG_FILE;

}

ConfigParser::~ConfigParser(void)
{

}

ConfigurationData& ConfigParser::parseConfig()
{
  return m_confData;
}

