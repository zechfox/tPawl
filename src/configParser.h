/*=============================
/* configuratoin Parser
/*
/*
/*===========================*/
#pragma once

#include <iostream>
#include <vector>
#include <map>
 
#include "typeDef.h"

using namespace std;

class ConfigParser
{
  public:
    ConfigParser(const std::string& confFilePath);
    ~ConfigParser();

    ConfigurationData& parseConfig();
  private:
    bool readConfigFile(const std::string& confFilePath);
    std::string trim(const std::string& str, const std::string& whitespace) const;
    std::string reduce(const std::string& str,
                       const std::string& fill,
                       const std::string& whitespace) const;
    void parseGeneralConfig(const std::vector<std::string> configData);
    void parseGestureConfig(const std::vector<std::string> configData);
    std::pair<std::string, std::string> getConfigPair(const std::string& config);

    void dumpGroupConfig(const std::string& groupName, const std::vector<std::string> configGroup);
    Evidence convertToEvidence(const std::string& str);
    ConfigurationData m_confData;
    std::map<std::string, std::string> m_confSettingMap;

};
