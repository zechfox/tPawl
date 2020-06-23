/*=============================
/* LOG 
/*
/*
/*===========================*/
#include "typeDef.h"
#pragma once
class GestureIf
{
  public:
    virtual bool invite(SensorData& sensorData) = 0;
    virtual bool performAction(void) = 0;
    virtual void dump() = 0;

};
