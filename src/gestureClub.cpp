/*=============================
/* Gesture Club
/*
/*
/*===========================*/
#include <iostream>
#include <algorithm>

#include "typeDef.h"
#include "gestureClub.h"
#include "buildInGesture.h"
#include "gesture.h"
#include "gestureIf.h"
#include "log.h"
using namespace std;

GestureClub::GestureClub()
{
  m_members.clear();
  auto buildInGesture = std::make_shared<BuildInGesture>();
  // 1 finger buildin gesture
  m_members[1].push_back(buildInGesture);
  // 2 finger buildin gesture
  m_members[2].push_back(buildInGesture);
}

GestureClub::~GestureClub()
{
  m_members.clear();
}

void GestureClub::dumpGestures()
{
  for (auto members : m_members)
  {
    LOG("Finger: " << members.first << " has member: ");
    for (auto member : members.second)
    {
      member->dump();
    }
  }

}

void GestureClub::registerMembers(std::vector<GestureData>& gestureData)
{
  
  for (auto gestures : gestureData)
  {
    m_members[gestures.touchPointNumber].push_back(std::make_shared<Gesture>(gestures));
  }
}

std::vector<std::shared_ptr<GestureIf>> GestureClub::inviteMembers(SensorData& sensorData)
{
  std::vector<std::shared_ptr<GestureIf>> invitedGestures;
  std::copy_if(m_members[sensorData.fingerNumber].begin(),
               m_members[sensorData.fingerNumber].end(),
               std::back_inserter(invitedGestures),
               [&](std::shared_ptr<GestureIf> gesture){
                   return gesture->invite(sensorData);
               });
  sensorData.coordinatorsData.clear();
  return invitedGestures;
}

