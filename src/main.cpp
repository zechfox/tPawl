#include <iostream>
#include <memory>

#include "configParser.h"
#include "gestureClub.h"
#include "sensorDataHandler.h"

using namespace std;

int main()
{
  cout << "Touch Pawl Start!" << endl;
  std::shared_ptr<ConfigParser> configParserPtr = std::make_shared<ConfigParser>();
  ConfigurationData confData = configParserPtr->parseConfig();
  auto gestureClubPtr = std::make_shared<GestureClub>();
  auto sensorDataHandlerPtr = std::make_shared<SensorDataHandler>(confData.monitorName,
                                                                  confData.accRawDataFactor,
                                                                  confData.accThreshold,
                                                                  confData.tsDevName);
  auto accelerometerPtr = std::make_shared<Accelerometer>(confData.accPath,
                                                          confData.accDevice,
                                                          confData.accRawDataX,
                                                          confData.accRawDataY);
  sensorDataHandlerPtr->registerAccelerometer(accelerometerPtr);
  gestureClubPtr->registerMembers(confData.gestures);
  configParserPtr.reset();

  SensorData sensorData;
  sensorData.fingerNumber = 0;
  auto orientation = Orientation::NORMAL;
  while (true)
  {
    if (sensorDataHandlerPtr->fillSensorData(sensorData))
    {
      if (orientation != sensorData.orientation)
      {
        sensorDataHandlerPtr->rotateScreen(sensorData.orientation);
        orientation = sensorData.orientation;
      }
      cout << "Finger Number:" << sensorData.fingerNumber << endl;
      if (sensorData.fingerNumber > 0)
      {
        std::vector<std::shared_ptr<Gesture>> invitedGestures = gestureClubPtr->inviteMembers(sensorData);
        for (auto gesture : invitedGestures)
        {
          gesture->performAction();
        }
      }
      sensorData.fingerNumber = 0;
    }
    // TODO:exception 
    else
    {

    }
  }
  // 1. get sensor data: accelerometers, touch panel.
  // 2. gestureClub invite gestures by data.
  // 3. all invited gestures takes action.

  return 0;
}
