#include <iostream>
#include <fstream>
#include <memory>

#include <signal.h>

#include "log.h"
#include "configParser.h"
#include "gestureClub.h"
#include "sensorDataHandler.h"

using namespace std;
bool g_isQuit = false;
void signal_handle(int signal_num)
{
  g_isQuit = true;
}

int main()
{
  std::shared_ptr<ConfigParser> configParserPtr = std::make_shared<ConfigParser>();
  ConfigurationData confData = configParserPtr->parseConfig();

  std::streambuf *newStdOut, *backupStdOut;
  std::ofstream logFileStream;
  logFileStream.open(confData.logFile.c_str(), std::ofstream::trunc);
  backupStdOut = std::cout.rdbuf();
  newStdOut = logFileStream.rdbuf();
  std::cout.rdbuf(newStdOut);

  LOG("Touch Pawl Start!");
  signal(SIGINT, signal_handle);

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
  while (!g_isQuit)
  {
    if (sensorDataHandlerPtr->fillSensorData(sensorData))
    {
      if (orientation != sensorData.orientation)
      {
        sensorDataHandlerPtr->rotateScreen(sensorData.orientation);
        orientation = sensorData.orientation;
      }
      LOG("Finger Number:" << sensorData.fingerNumber);
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


  LOG("Bye!");
  std::cout.rdbuf(backupStdOut);
  logFileStream.close();

  // 1. get sensor data: accelerometers, touch panel.
  // 2. gestureClub invite gestures by data.
  // 3. all invited gestures takes action.

  return 0;
}
