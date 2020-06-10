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
  configParserPtr.reset();

  std::shared_ptr<SensorData> sensorDataPtr = std::make_shared<SensorData>();
  auto orientation = Orientation::NORMAL;
  while (true)
  {
    if (sensorDataHandlerPtr->fillSensorData(sensorDataPtr))
    {
      if (orientation != sensorDataPtr->orientation)
      {
        sensorDataHandlerPtr->rotateScreen(sensorDataPtr->orientation);
        orientation = sensorDataPtr->orientation;
      }
      cout << "Finger Number:" << sensorDataPtr->fingerNumber << endl;
      sensorDataPtr->fingerNumber = 0;
    }
    // TODO:exception 
    else
    {

    }
  }
  sensorDataPtr.reset();
  // 1. get sensor data: accelerometers, touch panel.
  // 2. gestureClub invite gestures by data.
  // 3. all invited gestures takes action.

  return 0;
}
