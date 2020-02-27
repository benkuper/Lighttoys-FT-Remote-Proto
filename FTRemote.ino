#include "Config.h"
Config conf;

// ******************************************   DEFINES

#define USE_SERIAL 1
#if USE_SERIAL
#define SERIAL_DEBUG 1
#define USE_BLE 1
#endif

#define USE_WIFI 1
#if USE_WIFI
#define USE_OSC 1
#define USE_UDP 1
#endif

#define USE_FT 1
#define USE_SERVER 1

#define USE_BUTTONS 0
#define USE_LEDS 0
#define USE_FILES 0
#define USE_PLAYER 0

// ******************************************   INCLUDES

#if USE_SERIAL
#include "SerialManager.h"
SerialManager serialManager;

#if USE_BLE
#include "BLEManager.h"
BLEManager bleManager;
#endif //BLE
#endif //SERIAL

#if USE_FT
#include "FTRemoteManager.h"
FTRemoteManager ftManager;
#endif

#if USE_WIFI
#include "WifiManager.h"
WifiManager wifiManager;
#endif //WIFI

#if USE_OSC
#include "OSCManager.h"
OSCManager oscManager;
#endif //OSC

#if USE_UDP
#include "UDPManager.h"
UDPManager udpManager;
#endif //OSC

#if USE_SERVER
#include "RWebServer.h"
RWebServer webServer;
#endif // WEBSERVER

#if USE_BUTTONS
#include "ButtonManager.h"
ButtonManager btManager;
#endif

#if USE_LEDS
#include "LedManager.h"
LedManager ledManager;
#endif

#if USE_FILES
#include "FileManager.h"
FileManager fileManager;
#endif

#if USE_PLAYER
#include "Player.h"
Player player;
#endif

// ******************************************   CALLBACK


// *********************  WIFI

#if USE_WIFI
void wifiConnectionUpdate()
{
  DBG("Wifi connection update " + String(wifiManager.isConnected));

  if (wifiManager.isConnected)
  {
    digitalWrite(13, LOW);
    
#if USE_OSC
    DBG("Setup OSC now");
    oscManager.init();
#endif //OSC

#if USE_UDP
    DBG("Setup UDP now");
    udpManager.init();
#endif

#if USE_SERVER
   webServer.init();
#endif

  }
}
#endif //WIFI

void commandCallback(String providerId, CommandProvider::CommandData data)
{
  DBG("Got Command from " + providerId + " : " + data.type);
  switch (data.type)
  {
    case CommandProvider::CommandType::MESSAGE_RECEIVED:
      {
#if USE_FT
        if (providerId != "FT") ftManager.sendMessage(data.value1.stringValue);
#endif

        if (providerId == "FT")
        {
#if USE_SERIAL
          serialManager.sendRawMessage(data.value1.stringValue);
#endif
#if USE_UDP
          udpManager.sendRawMessage(data.value1.stringValue);
#endif
        }
      }
      break;

#if USE_FT
    case CommandProvider::CommandType::START_SYNC: ftManager.sendPair(true); break;
    case CommandProvider::CommandType::ADD_SYNC: ftManager.sendPair(false); break;
    case CommandProvider::CommandType::STOP_SYNC: ftManager.sendFinishPairing(); break;
    case CommandProvider::CommandType::FULL_COLOR: ftManager.sendFullColor(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::COLOR_AB: ftManager.sendABColor(data.value1.intValue, data.value2.intValue, data.value3.intValue); break;
    case CommandProvider::CommandType::COLOR_MODE: ftManager.sendColorMode(data.value1.intValue, data.value2.intValue); break;
    case CommandProvider::CommandType::BRIGHTNESS: ftManager.sendBrightness(data.value1.intValue, data.value2.floatValue); break;
    case CommandProvider::CommandType::PING: ftManager.sendPing(data.value1.intValue); break;
    case CommandProvider::CommandType::BLACKOUT: ftManager.sendBlackout(); break;
    case CommandProvider::CommandType::START_SHOW: ftManager.sendStartShow(data.value1.intValue, data.value2.floatValue, data.value3.floatValue, data.value4.intValue); break;
    case CommandProvider::CommandType::STOP_SHOW: ftManager.sendStopShow(data.value1.intValue); break;
#endif


    default: DBG("Command not handled"); break;
  }

}



// ******************************************   SETUP AND LOOP

void setup()
{
  //Need to activate mosfet
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  conf.init();

#if USE_SERIAL
  serialManager.init();
  serialManager.setCommandCallback(&commandCallback);
  //serialManager.setPatternCallback(&patternCallback);
#endif //SERIAL


#if USE_FT
  ftManager.init();
  ftManager.setCommandCallback(&commandCallback);
#endif

#if USE_BLE && USE_SERIAL
  bleManager.init();
#endif //BLE

#if USE_LEDS
  ledManager.init();
#endif

#if USE_WIFI
  wifiManager.init();
  wifiManager.setCallbackConnectionUpdate(wifiConnectionUpdate);

#if USE_LEDS
  updateConnectionLeds();
#endif
#endif //WIFI

#if USE_OSC
  //wait for wifi event to init
  oscManager.setCommandCallback(&commandCallback);
#endif //OSC

#if USE_UDP
  //wait for wifi event to init
  udpManager.setCommandCallback(&commandCallback);
#endif //UDP4

#if USE_SERVER
 //wait for wifi event to init
 webServer.setCommandCallback(&commandCallback);
#endif

#if USE_BUTTONS
  btManager.init();
  btManager.setEventCallbacks(handlePress, handleShortPress, handleLongPress, handleVeryLongPress, handleMultiPress);
#endif


#if USE_FILES
  fileManager.init();
#endif

#if USE_PLAYER
  player.init();
#endif

  DBG("Bridge is initialized");
}


void loop()
{
#if USE_BUTTONS
  btManager.update();
#endif

#if USE_SERIAL
  serialManager.update();
#if USE_BLE
  bleManager.update();
#endif
#endif

#if USE_WIFI
  wifiManager.update();
#endif

#if USE_OSC
  oscManager.update();
#endif

#if USE_UDP
  udpManager.update();
#endif

#if USE_SERVER
  webServer.update();
#endif

#if USE_FT
  ftManager.update();
#endif

#if USE_PLAYER
  player.update();
#endif

}
