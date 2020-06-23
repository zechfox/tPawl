/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#include <iostream>
#include <fstream>
#include <map>

#include "log.h"
#include "configParser.h"
#include "typeDef.h"
 
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
  m_confData.logFile = LOG_FILE;
  readConfigFile(confFilePath);

  std::string accFactorPath;
  accFactorPath.append(m_confData.accPath);
  accFactorPath.append(m_confData.accDevice);
  accFactorPath.append(m_confData.accScale);

  std::ifstream accFactorStream(accFactorPath);
  if (accFactorStream.is_open())
  {
    accFactorStream.clear();
    accFactorStream.seekg(0);
    accFactorStream >> m_confData.accRawDataFactor;
    accFactorStream.close();
  }
}

ConfigParser::~ConfigParser(void)
{

}

bool ConfigParser::readConfigFile(const std::string& confFilePath)
{
  std::ifstream fileStream(confFilePath.c_str());
  if (fileStream.is_open())
  {
    std::string line;
    std::vector<std::string> configGroup;
    std::string groupName;
    while(getline(fileStream, line)){
      // Remove comment Lines
      std::size_t found = line.find_first_of('#');
      std::string configData = line.substr(0, found);

      // Remove ^M from configData
      configData.erase(std::remove(configData.begin(), configData.end(), '\r'), configData.end());
      if(configData.empty())
         continue;

      std::size_t groupNameBegin = configData.find_first_of('[');
      std::size_t groupNameEnd = configData.find_first_of(']');
      if (groupNameBegin != std::string::npos && groupNameEnd != std::string::npos)
      {
        std::string newGroupName = configData.substr(groupNameBegin + 1, groupNameEnd - 1);
        if (groupName != newGroupName)
        {
          LOG("New Group: " << newGroupName);
          dumpGroupConfig(groupName, configGroup);
          groupName = newGroupName;
          configGroup.clear();
        }
        continue;
      }
      configGroup.push_back(configData);
    }
    dumpGroupConfig(groupName, configGroup);
  }
  else
  {
    LOG("Open Config File Failed.");
  }

  return true;
}

void ConfigParser::dumpGroupConfig(const std::string& groupName, const std::vector<std::string> configGroup)
{
  LOG("Dump Group: " << groupName << "size: " << configGroup.size());
  if (0 == groupName.compare("General"))
  {
    parseGeneralConfig(configGroup);
  }
  else if (std::string::npos != groupName.find("Gesture"))
  {
    parseGestureConfig(configGroup);
  }
  else
  {
    LOG("Not Interested Group.");
  }
}

// TODO: need more better way
void ConfigParser::parseGeneralConfig(const std::vector<std::string> configData)
{
  auto parser = [&] (const std::string& config) {
    auto configPair = getConfigPair(config);
    
    if (0 == configPair.first.compare("accPath"))
    {
      m_confData.accPath = configPair.second;
    }
    else if (0 == configPair.first.compare("accTreshold"))
    {
      std::string::size_type sz;
      m_confData.accThreshold = std::stof(configPair.second, &sz);
    }
    else if (0 == configPair.first.compare("accDevice"))
    {
      m_confData.accDevice = configPair.second;
    }
    else if (0 == configPair.first.compare("accRawDataX"))
    {
      m_confData.accRawDataX = configPair.second;
    }
    else if (0 == configPair.first.compare("accRawDataY"))
    {
      m_confData.accRawDataY = configPair.second;
    }
    else if (0 == configPair.first.compare("monitorName"))
    {
      m_confData.monitorName = configPair.second;
    }
    else if (0 == configPair.first.compare("accScale"))
    {
      m_confData.accScale = configPair.second;
    }
    else if (0 == configPair.first.compare("tsDevPath"))
    {
      m_confData.tsDevPath = configPair.second;
    }
    else if (0 == configPair.first.compare("tsDevName"))
    {
      m_confData.tsDevName = configPair.second;
    }
    else if (0 == configPair.first.compare("logFile"))
    {
      m_confData.logFile = configPair.second;
    }
    else
    {
      

    }
  };

  std::for_each(configData.begin(), configData.end(), parser);
}

void ConfigParser::parseGestureConfig(const std::vector<std::string> configData)
{
  GestureData gesture;
  gesture.name = "Gesture";
  std::string::size_type sz;

  auto parser = [&] (const std::string& config) {
    auto configPair = getConfigPair(config);
    if (0 == configPair.first.compare("touchPointNumber"))
    {
      gesture.touchPointNumber = std::stoi(configPair.second, &sz);
    }
    else if (0 == configPair.first.compare("evidence"))
    {
      gesture.evidence = convertToEvidence(configPair.second);
    }
    else if (0 == configPair.first.compare("action"))
    {
      gesture.action = configPair.second;
    }
    else if (0 == configPair.first.compare("name"))
    {
      gesture.name = configPair.second;
    }
    else
    {

    }
  };

  std::for_each(configData.begin(), configData.end(), parser);
  m_confData.gestures.push_back(gesture);
}

Evidence ConfigParser::convertToEvidence(const std::string& str)
{
  Evidence evidence = Evidence::NOT_AVAILIABLE;
  
  if (0 == str.compare("MOVE_UP"))
  {
    evidence = Evidence::MOVE_UP;
  }
  else if (0 == str.compare("MOVE_DOWN"))
  {
    evidence = Evidence::MOVE_DOWN;
  }
  else if (0 == str.compare("MOVE_LEFT"))
  {
    evidence = Evidence::MOVE_LEFT;
  }
  else if (0 == str.compare("MOVE_RIGHT"))
  {
    evidence = Evidence::MOVE_RIGHT;
  }
  else if (0 == str.compare("MOVE_TOP"))
  {
    evidence = Evidence::MOVE_TOP;
  }
  else if (0 == str.compare("MOVE_BOTTOM"))
  {
    evidence = Evidence::MOVE_BOTTOM;
  }
  else if (0 == str.compare("MOVE_LEFT_SIDE"))
  {
    evidence = Evidence::MOVE_LEFT_SIDE;
  }
  else if (0 == str.compare("MOVE_RIGHT_SIDE"))
  {
    evidence = Evidence::MOVE_RIGHT_SIDE;
  }
  else if (0 == str.compare("ENLARGE"))
  {
    evidence = Evidence::ENLARGE;
  }
  else if (0 == str.compare("SHRINK"))
  {
    evidence = Evidence::SHRINK;
  }
  else
  {

  }
  return evidence;
}

std::pair<std::string, std::string> ConfigParser::getConfigPair(const std::string& config)
{
  std::string delimeter = "=";
  std::uint32_t initPos = 0;
  std::string tag, value;
  std::uint32_t length = config.find(delimeter);

  if (length != string::npos)
  {
    tag = config.substr(initPos, length);
    value = config.substr(length+1);
  }
  // Trim white spaces
  tag = reduce(tag, " ", " \t");
  value = reduce(value, " ", " \t");
  return std::make_pair(tag, value);
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

