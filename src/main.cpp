#include <iostream>
#include <memory>

#include "configParser.h"
#include "gestureClub.h"
#include "sensorDataHandler.h"

using namespace std;

int main()
{
  cout << "Hello, world" << endl;
  std::shared_ptr<ConfigParser> configParser_p = std::make_shared<ConfigParser>();
  ConfigurationData confData = configParser_p->parseConfig();
  auto gestureClub_p = std::make_shared<GestureClub>();
  auto sensorDataHandler_p = std::make_shared<SensorDataHandler>(confData.monitorName,
                                                                 confData.accRawDataFactor,
                                                                 confData.accThreshold);
  auto accelerometer_p = std::make_shared<Accelerometer>(confData.accPath,
                                                         confData.accDevice,
                                                         confData.accRawDataX,
                                                         confData.accRawDataY);
  sensorDataHandler_p->registerAccelerometer(accelerometer_p);

  sensorDataHandler_p->getSensorData();

  configParser_p.reset();
  // 1. get sensor data: accelerometers, touch panel.
  // 2. gestureClub invite gestures by data.
  // 3. all invited gestures takes action.

  return 0;
}
