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
  m_members.clear();
  std::vector<GestureData> buildInGestures;
  GestureData pressGesture;
  pressGesture.touchPointNumber = 1;
  pressGesture.evidence = Evidence::PRESS;
  pressGesture.action = "echo \"1 finger pressed\"";
  buildInGestures.push_back(pressGesture);

  GestureData twoFingerEnlarge;
  twoFingerEnlarge.touchPointNumber = 2;
  twoFingerEnlarge.evidence = Evidence::ENLARGE;
  twoFingerEnlarge.action = "echo \"2 finger enlarged\"";
  buildInGestures.push_back(twoFingerEnlarge);

  GestureData twoFingerShrink;
  twoFingerShrink.touchPointNumber = 2;
  twoFingerShrink.evidence = Evidence::SHRINK;
  twoFingerShrink.action = "echo \"2 finger shrinked\"";
  buildInGestures.push_back(twoFingerShrink);

  registerMembers(buildInGestures);
}

GestureClub::~GestureClub()
{
  m_members.clear();
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
  sensorData.coordinatorsData.clear();
  return invitedGestures;
}

