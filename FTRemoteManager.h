#pragma once

#include "CommandProvider.h"
#include "Config.h"

#include <HardwareSerial.h>



class FTRemoteManager  :
  public CommandProvider
{
  public:
    FTRemoteManager() :
      CommandProvider("FT"),
      ftSerial(1)
    {
    }

    char buffer[256];
    int bufferIndex = 0;

    HardwareSerial ftSerial;

    void init()
    {
      ftSerial.begin(115200, SERIAL_8N1, 36,4); // ADAFRUIT FEATHER 32 : 16, 17);

      memset(buffer, 0, 256);
    }

    void update()
    {
      while (ftSerial.available())
      {
        byte c = ftSerial.read();
        //DBG("Got char : "+String(c));
        if (c == 255 || c == '\n')
        {
          parseMessage(buffer);
          memset(buffer, 0, 255);
          bufferIndex = 0;
        } else
        {
          if (bufferIndex < 255) buffer[bufferIndex] = c;
          bufferIndex++;
        }
      }
    }

    void parseMessage(String message)
    {
      DBG("Got from FT " + message);
      CommandData d;
      d.type = MESSAGE_RECEIVED;
      d.value1.stringValue = (char *)message.c_str();
      sendCommand(d);
    }

    /* HELPERS COMMANDS */

    void sendPing(int propID)
    {
      int targetMask = getMaskForPropID(propID);
      sendMessage("gping " + String(targetMask));
    }

    void sendBlackout()
    {
      int targetMask = getMaskForPropID(-1);
      sendMessage("leach " + String(targetMask) + ",0,0,0,0,0,0");
    }

    void sendPair(bool newGroup)
    {
      sendMessage("gadd " + String(newGroup ? "1" : "0"));
    }

    void sendFinishPairing()
    {
      sendMessage("gstop");
    }

    void sendStartShow(int showID, float startTime,  float startDelay, int propID)
    {
      int targetMask = getMaskForPropID(propID);
      sendMessage("sstart " + String(targetMask) + "," + String(showID - 1) + ", " +
                  (int)(startDelay * 1000) + ", " + (int)(startTime * 1000));
    }

    void sendStopShow(int propID)
    {
      int targetMask = getMaskForPropID(propID);
      sendMessage("sstop " + String(targetMask));
    }

    void sendFullColor(int color, int propID)
    {
      sendABColor(color, color, propID);
    }

    void sendABColor(int color1, int color2, int propID)
    {
      int targetMask = getMaskForPropID(propID);
      int r1 = (color1 >> 16) & 0xFF;
      int g1 = (color1 >> 8) & 0xFF;
      int b1 = color1 & 0xFF;
      int r2 = (color2 >> 16) & 0xFF;
      int g2 = (color2 >> 8) & 0xFF;
      int b2 = color2 & 0xFF;

      sendMessage("leach " + String(targetMask) + "," + String(r1) + "," + String(g1) + "," + String(b1) + "," + String(r2) + "," + String(g2) + "," + String(b2));
    }

    void sendColorMode(int colorMode, int propID)
    {
      int targetMask = getMaskForPropID(propID);
      sendMessage("lmode " + String(targetMask) + "," + String(colorMode));
    }

    void sendBrightness(float brightness, int propID)
    {
      int targetMask = getMaskForPropID(propID);
      sendMessage("lbright " + String(targetMask) + "," + String((int)(brightness*255)));
    }

    void sendMessage(String message)
    {
      DBG("Send to FT : " + message);
      ftSerial.print(message + "\r\n");
    }

    int getMaskForPropID(int propID)
    {
      if (propID < 0) return 1099511627775;
      else return 1 << propID;
    }
};
