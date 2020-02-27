#pragma once

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <ESPmDNS.h>
#include "SerialManager.h"

class OSCManager :
  public CommandProvider
{
  public:
    OSCManager() : CommandProvider("OSC") {}
    ~OSCManager() {}

    WiFiUDP Udp;
    const unsigned int localPort = 9000;        // local port to listen for UDP packets (here's where we send the packets)
    char address[64];

    void init()
    {

      DBG("Init OSC");

      Udp.stop();
      Udp.begin(localPort);

      if (!MDNS.begin("ftremote")) {
        DBG("Error setting up MDNS responder!");
      } else
      {
        MDNS.addService("_osc", "_udp", localPort);
        DBG("_osc._udp. ftremote zeroconf is init.");
      }

      DBG("OSC Initialized");// listening on "+String(buf)+":"+String(localPort));

    }

    void update() {
      int size = Udp.parsePacket();
      if (size > 0) {
        OSCMessage msg;
        while (size--) {
          msg.fill(Udp.read());
        }

        if (!msg.hasError())
        {

          digitalWrite(13, HIGH);
          delay(2);
          digitalWrite(13, LOW);

          if (msg.fullMatch("/sync"))  sendCommand(START_SYNC);
          else if (msg.fullMatch("/stopSync")) sendCommand(STOP_SYNC);
          else if (msg.fullMatch("/addSync")) sendCommand(ADD_SYNC);
          else if (msg.fullMatch("/ping"))
          {
            CommandData d;
            d.type = PING;
            d.value1.intValue = msg.size() > 0 ? msg.getInt(0) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/blackout"))
          {
            CommandData d;
            d.type = BLACKOUT;
            d.value1.intValue = msg.size() > 0 ? msg.getInt(0) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/startShow"))
          {
            if (msg.size() < 1) return;
            CommandData d;
            d.type = START_SHOW;
            d.value1.intValue = msg.getInt(0); //showID
            d.value2.floatValue = msg.size() > 1 ? msg.getFloat(1) : 0; //startTime
            d.value3.floatValue = msg.size() > 2 ? msg.getFloat(2) : 0; //startDelay
            d.value4.intValue = msg.size() > 3 ? msg.getFloat(3) : -1; //propID
            sendCommand(d);
          } else if (msg.fullMatch("/stopShow"))
          {
            CommandData d;
            d.type = STOP_SHOW;
            d.value1.intValue = msg.size() > 0 ? msg.getInt(0) : -1;
            sendCommand(d);

          } else if (msg.fullMatch("/fullColor"))
          {
            if (msg.size() < 3) return;
            CommandData d;
            d.type = FULL_COLOR;

            int color1 = ((int)(msg.getFloat(0) * 255)) << 16 | ((int)(msg.getFloat(1) * 255)) << 8 | ((int)(msg.getFloat(2) * 255));
            d.value1.intValue = color1;
            d.value2.intValue = msg.size() > 3 ? msg.getInt(3) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/colorAB"))
          {
            if (msg.size() < 6) return;
            CommandData d;
            d.type = COLOR_AB;
            int color1 = ((int)(msg.getFloat(0) * 255)) << 16 | ((int)(msg.getFloat(1) * 255)) << 8 | ((int)(msg.getFloat(2) * 255));
            int color2 = ((int)(msg.getFloat(3) * 255)) << 16 | ((int)(msg.getFloat(4) * 255)) << 8 | ((int)(msg.getFloat(5) * 255));
            d.value1.intValue = color1;
            d.value2.intValue = color2;
            d.value3.intValue = msg.size() > 6 ? msg.getInt(3) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/brightness"))
          {
            if(msg.size() < 1) return;
            CommandData d;
            d.type = BRIGHTNESS;
            d.value1.floatValue = msg.getFloat(0);
            d.value2.intValue = msg.size() > 1 ? msg.getInt(1) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/colorMode"))
          {
            if(msg.size() < 1) return;
            CommandData d;
            d.type = COLOR_MODE;
            d.value1.intValue = msg.getInt(0);
            d.value2.intValue = msg.size() > 1 ? msg.getInt(1) : -1;
            sendCommand(d);
          } else if (msg.fullMatch("/direct"))
          {
            if (msg.size() == 0) return;
            CommandData d;
            d.type = MESSAGE_RECEIVED;
            char str[256];
            msg.getString(0, str);
            d.value1.stringValue = str;
            sendCommand(d);
          }
          else {
            char addr[32];
            msg.getAddress(addr, 0);
            DBG("OSC Address not handled : " + String(addr));
          }
        }
      }
    }


    void sendMessage(OSCMessage &m)
    {
      //
    }
};
