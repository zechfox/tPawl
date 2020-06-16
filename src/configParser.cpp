/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#include <iostream>
#include <fstream>
#include <map>

#include "configParser.h"
 
using namespace std;

ConfigParser::ConfigParser(const std::string& confFilePath)
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

  readConfigFile(confFilePath);

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

bool ConfigParser::readConfigFile(const std::string& confFilePath)
{
  std::ifstream fileStream(confFilePath.c_str());
  std::string delimeter = "=";
  std::uint32_t initPos = 0;
  if (fileStream.is_open())
  {
    std::string line;
    while(getline(fileStream, line)){
      // Remove comment Lines
      std::size_t found = line.find_first_of('#');
      std::string configData = line.substr(0, found);

      // Remove ^M from configData
      configData.erase(std::remove(configData.begin(), configData.end(), '\r'), configData.end());
      if(configData.empty())
         continue;

      std::string tag, value;
      std::uint32_t length = configData.find(delimeter);
      if (length != string::npos)
      {
         tag = configData.substr(initPos, length);
         value = configData.substr(length+1);
      }
      // Trim white spaces
      tag = reduce(tag, " ", " \t");
      value = reduce(value, " ", " \t");
      if (tag.empty() || value.empty())
         continue;

      // override old one.
      m_confSettingMap[tag] = value;
    }

  }

  return true;
}

std::string ConfigParser::trim(const std::string& str, const std::string& whitespace) const
{
   size_t strBegin = str.find_first_not_of(whitespace);
   if (strBegin == std::string::npos)
      return "";

   size_t strEnd = str.find_last_not_of(whitespace);
   size_t strRange = strEnd - strBegin + 1;

   return str.substr(strBegin, strRange);
}

std::string ConfigParser::reduce(const std::string& str,
                                 const std::string& fill,
                                 const std::string& whitespace) const
{
   // trim first
  std::string result = trim(str, whitespace);

   // replace sub ranges
   size_t beginSpace = result.find_first_of(whitespace);
   while (beginSpace != std::string::npos)
   {
      size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
      size_t range = endSpace - beginSpace;

      result.replace(beginSpace, range, fill);

      size_t newStart = beginSpace + fill.length();
      beginSpace = result.find_first_of(whitespace, newStart);
   }

   return result;
}

ConfigurationData& ConfigParser::parseConfig()
{
  return m_confData;
}

