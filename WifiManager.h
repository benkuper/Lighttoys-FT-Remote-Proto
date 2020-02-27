#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

#include "Config.h"
#include "SerialManager.h"

#define CONNECT_TIMEOUT 5000
#define CONNECT_TRYTIME 500

class WifiManager
{
public:
  WifiManager(){  
      setCallbackConnectionUpdate(&WifiManager::connectionUpdateDefaultCallback);
  }
  
  ~WifiManager(){}

  bool isLocal = false;
  bool isConnected = false;
  bool isConnecting = false;
  
  long timeAtStartConnect;
  long timeAtLastConnect;

  bool isActivated;
  
  void init()
  {
    isActivated = Config::instance->getWifiMode();

    if(isConnected)
    {
      DBG("Disconnecting first...");
      WiFi.disconnect();
      delay(100);
    }
    
    if(!isActivated)
    {
      DBG("Wifi is not activated, not initializing");
      return;
    }

    String ssid = Config::instance->getWifiSSID();
    String pass = Config::instance->getWifiPassword();
    String ip = Config::instance->getStaticIP();
    String gateway = Config::instance->getGateway();
    String subnet = Config::instance->getSubnetMask();
    
    DBG("Connecting to Wifi "+ssid+" with password "+pass+"...");

    WiFi.mode(WIFI_STA);
   
    if(ip != "" && gateway != "" && subnet != "")
    {
      IPAddress addr;
      IPAddress gatAddr;
      IPAddress subnetAddr;
      if(addr.fromString(ip) && gatAddr.fromString(gateway) && subnetAddr.fromString(subnet))
      {
        WiFi.config(addr, gatAddr, subnetAddr);
      }else
      {
        DBG("Error setting static IP "+ip+", gateway "+gateway+", subnet "+subnet);
      }
    }
    
    WiFi.begin(ssid.c_str(), pass.c_str());
    WiFi.setSleep(false);

    
    timeAtStartConnect = millis();
    timeAtLastConnect = millis();
    
    isLocal = false;
    isConnecting = true;
    setConnected(false);
    
    digitalWrite(13, HIGH);
  }

  void update()
  {
    if(!isActivated) return;
    if(isLocal || isConnected) return;

    if(millis() > timeAtLastConnect + CONNECT_TRYTIME)
    {      
      if(WiFi.status() == WL_CONNECTED)
      {  
         digitalWrite(13, LOW);

         DBG("WiFi Connected, local IP : "+WiFi.localIP().toString());

        isLocal = true;
        setConnected(true);
    
         return;
      }
      timeAtLastConnect = millis();
    }
        
    if(millis() > timeAtStartConnect + CONNECT_TIMEOUT)
    {
      DBG("Could not connect to "+Config::instance->getWifiSSID());
      setConnected(false);
      for(int i=0;i<5;i++)
      {
        digitalWrite(13, HIGH);
        delay(50);
        digitalWrite(13, LOW);
        delay(50);
      }
      
      setupLocal();
    }
  }

  void setupLocal()
  {
    String softAPName = "Lighttoys RemoteRemote "+Config::instance->getDeviceName();
    String softAPPass = "pyroterra";
    WiFi.softAP(softAPName.c_str(), softAPPass.c_str());
    Serial.println("Local IP : "+WiFi.softAPIP().toString());

    isLocal = true;
    isConnecting = false;
    setConnected(true);
    
    DBG("AP WiFi is init with name "+softAPName+" , pass : "+softAPPass);
  }

  void setConnected(bool value)
  {
    isConnected = value;
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
