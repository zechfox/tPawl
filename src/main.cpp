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
  auto gestureClub_p = std::make_shared<GestureClub>();
  auto sensorDataHandler_p = std::make_shared<SensorDataHandler>();

  // 1. get sensor data: accelerometers, touch panel.
  // 2. gestureClub invite gestures by data.
  // 3. all invited gestures takes action.

  return 0;
}
