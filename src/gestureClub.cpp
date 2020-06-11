/*=============================
/* Gesture Club
/*
/*
/*===========================*/
#include <iostream>
#include <algorithm>
#include "gestureClub.h"
#include "gesture.h" 
using namespace std;

GestureClub::GestureClub()
{

}

GestureClub::~GestureClub()
{

}

void GestureClub::registerMembers(std::vector<GestureData>& gestureData)
{
  for (auto gestures : gestureData)
  {
    m_members[gestures.touchPointNumber].push_back(std::make_shared<Gesture>(gestures));
  }
}

std::vector<std::shared_ptr<Gesture>> GestureClub::inviteMembers(SensorData& sensorData)
{
  std::vector<std::shared_ptr<Gesture>> invitedGestures;
  std::copy_if(m_members[sensorData.fingerNumber].begin(),
               m_members[sensorData.fingerNumber].end(),
               std::back_inserter(invitedGestures),
               [&](std::shared_ptr<Gesture> gesture){
                   return gesture->invite(sensorData);
               });
  
  return invitedGestures;
}

