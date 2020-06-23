/*=============================
/* Gesture Club
/*
/*
/*===========================*/
#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include "gestureIf.h"
#include "typeDef.h"

using namespace std;

class GestureClub
{
  public:
    GestureClub();
    ~GestureClub();

    void registerMembers(std::vector<GestureData>& gestureData);
    std::vector<std::shared_ptr<GestureIf>> inviteMembers(SensorData& sensorData);
  private:
    std::map<std::uint32_t, std::vector<std::shared_ptr<GestureIf>>> m_members;
};

