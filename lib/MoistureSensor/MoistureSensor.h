#ifndef __MoistureSENSOR_H
#define __MoistureSENSOR_H

#include <Arduino.h>
#include "IOBase.h"
#include "../../src/version.h"
//#include "version.h"


class MoistureSensor : public IOBase
{
private:
  //bool state;
  int pin;
  bool newLevelFlag;
  u_int currentLevel;
  u_int previousLevel = 500;
  u_int readIntervalMillis = CMSM_READ_INTERVAL; //min interval between reading sensor in ms
  u_int lastReadMillis = -CMSM_READ_INTERVAL;

  //hysteresis and state related properties
  //u_int thresholdLevel = 600;
  int lowerThresholdLevel = 1200;
  int upperThresholdLevel = 2100;
  //bool trigger = false;

  //int currentLevel;
  //int lowerHys = targetLevel - lowerLevel;
  //int upperHys = targetLevel + upperLevel;

public:
  MoistureSensor(uint8_t pin);
  u_int getLevel();
  u_int clearNewLevelFlag();
  bool hasNewLevel();
  void setThresholdLevel();
  int getMoistureSensor();
  bool getState();
};

#endif