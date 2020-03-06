#pragma once
#include <ETH.h>

#include "Config.h"
#include "SerialManager.h"

#define ETH_CONNECT_TIMEOUT 5000

class EthernetManager
{
public:
    bool isConnecting = false;
    bool isConnected = false;
    bool connectionError = false;
    
    long timeAtConnect;
    
    void init()
    {
      setCallbackConnectionUpdate(&EthernetManager::connectionUpdateDefaultCallback);

      isConnecting =true;
      timeAtConnect = millis();
      
      WiFi.onEvent(std::bind(&EthernetManager::ethernetEvent, this, std::placeholders::_1));
      ETH.begin();

      String ip = Config::instance->getStaticIP();
      String gateway = Config::instance->getGateway();
      String subnet = Config::instance->getSubnetMask();
      
      if(ip != "" && gateway != "" && subnet != "")
      {
        IPAddress addr;
        IPAddress gatAddr;
        IPAddress subnetAddr;
        if (addr.fromString(ip) && gatAddr.fromString(gateway) && subnetAddr.fromString(subnet))
        {
          ETH.config(addr, gatAddr, subnetAddr);
        } else
        {
          DBG("Error setting Ethernet static IP " + ip + ", gateway " + gateway + ", subnet " + subnet);
        }
      }
    }
    
    void update()
    {
      if(isConnecting && millis() > timeAtConnect + ETH_CONNECT_TIMEOUT)
      {
        DBG("Ethernet could not connect.");
        setConnected(false);
        connectionError = true;
        for (int i = 0; i < 5; i++)
        {
          digitalWrite(13, HIGH);
          delay(50);
          digitalWrite(13, LOW);
          delay(50);
        }

      }
    }
    
    void ethernetEvent(WiFiEvent_t event)
    {
      if(connectionError) return;
      
      if(event == SYSTEM_EVENT_ETH_START || isConnecting || isConnected)
      {
        switch (event) {
          case SYSTEM_EVENT_ETH_START:
            ETH.setHostname("ftremote");
            WiFi.setHostname("ftremote");
            break;
          case SYSTEM_EVENT_ETH_CONNECTED:
            break;
          case SYSTEM_EVENT_ETH_GOT_IP:
            /*Serial.print("ETH MAC: ");
            Serial.print(ETH.macAddress());
            Serial.print(", IPv4: ");
            Serial.print(ETH.localIP());
            if (ETH.fullDuplex()) {
              Serial.print(", FULL_DUPLEX");
            }
            Serial.print(", ");
            Serial.print(ETH.linkSpeed());
            Serial.println("Mbps");
            */

            DBG("Ethernet Connected, local IP : " + ETH.localIP().toString());
            setConnected(true);
            break;
          case SYSTEM_EVENT_ETH_DISCONNECTED:
            setConnected(false);
            break;
          case SYSTEM_EVENT_ETH_STOP:
            setConnected(false);
            break;
            
          default:
            break;
        }
      }
    }

    void setConnected(bool value)
    {
      isConnected = value;
      isConnecting = false;
      onConnectionUpdate();
    }


    typedef void(*onConnectionUpdateEvent)();
    void (*onConnectionUpdate) ();

    void setCallbackConnectionUpdate (onConnectionUpdateEvent func) {
      onConnectionUpdate = func;
    }

    static void connectionUpdateDefaultCallback()
    {
      //nothing
    }
};
