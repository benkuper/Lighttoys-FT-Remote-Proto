#pragma once
#include "Preferences.h"

class Config
{
public:
  static Config * instance;

  enum WifiBLEMode { WIFI_ONLY = 0, BLE_ONLY = 1, BOTH = 2 };
  Preferences preferences;
  
  Config()
  {
    instance = this;
  }

  ~Config(){}

  void init()
  {
    preferences.begin("bridge", false);
  }

    String getDeviceName() { return preferences.getString("deviceName", String((int)ESP.getEfuseMac()).substring(0,10)); }
  void setDeviceName(String deviceName) { preferences.putString("deviceName", deviceName.substring(0,10)); }

  bool getWifiMode() { int mode = preferences.getInt("wifiBLEMode",2); return mode == WIFI_ONLY || mode == BOTH; } //0 is wifi only, 1 is BLE only, 2 is both
  bool getBLEMode() { int mode = preferences.getInt("wifiBLEMode",2); return mode == BLE_ONLY || mode == BOTH; }
  void setWifiBLEMode(int mode) { preferences.putInt("wifiBLEMode", mode); }
 
  String getWifiSSID() { return preferences.getString("ssid","notset"); }
  void setWifiSSID(String ssid) { preferences.putString("ssid",ssid); }
  
  String getWifiPassword() { return preferences.getString("pass","notset"); }
  void setWifiPassword(String pass) { preferences.putString("pass", pass); }

  String getRemoteHost() { return preferences.getString("remoteHost","127.0.0.1"); }
  void setRemoteHost(String value) { preferences.putString("remoteHost", value); }

  int getRemoteUDPPort() { return preferences.getInt("remoteUDPPort",8889); }
  void setRemoteUDPPort(int value) { preferences.putInt("remoteUDPPort", value); }

  int getRemoteOSCPort() { return preferences.getInt("remoteOSCPort", 9001); }
  void setRemoteOSCPort(int value) { preferences.putInt("remoteOSCPort", value); }


  String getStaticIP() { return preferences.getString("ip", ""); }
  void setStaticIP(String ip) { preferences.putString("ip", ip); }
  
  String getGateway() { return preferences.getString("gateway", ""); }
  void setGateway(String gateway) { preferences.putString("gateway", gateway); }

  String getSubnetMask() { return preferences.getString("subnet", "255.255.255.0"); }
  void setSubnetMask(String subnet) { preferences.putString("subnet", subnet); }
};

Config * Config::instance = nullptr;
