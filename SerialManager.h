#pragma once

#include "CommandProvider.h"
#include "Config.h"

#if SERIAL_DEBUG
#define DBG(msg) Serial.println(msg)
#else
#define DBG(msg)
#endif

class SerialManager :
  public CommandProvider
{
  public:
    SerialManager() :
      CommandProvider("Serial")
    {
      instance = this;
    }

    ~SerialManager() {}

    char buffer[256];
    int bufferIndex = 0;

    static SerialManager * instance;

    void init()
    {
      Serial.begin(115200);
      memset(buffer, 0, 256);

    }

    void update()
    {
      while (Serial.available())
      {
        byte c = Serial.read();
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
       CommandData d;
        d.type = MESSAGE_RECEIVED;
        d.value1.stringValue = (char *)message.c_str();
        sendCommand(d);
    }

    int splitString(char * source, String * dest, int maxNum)
    {
      char * pch = strtok (source, ",");
      int i = 0;
      while (pch != NULL && i < maxNum)
      {
        dest[i] = String(pch);
        pch = strtok (NULL, ",");
        i++;
      }

      return i;
    }

    void sendRawMessage(String msg) //without a newline
    {
      Serial.print(msg);
    }

    void sendMessage(String msg)
    {
      Serial.println(msg);
    }

    void sendTrigger(String name)
    {
      Serial.println(name);
    }

    void sendBoolValue(String name, bool value)
    {
      Serial.println(name + " " + (value ? 1 : 0));
    }

    void sendIntValue(String name, int value)
    {
      Serial.println(name + " " + String(value));
    }

    void sendFloatValue(String name, float value)
    {
      Serial.println(name + " " + String(value));
    }
};

SerialManager * SerialManager::instance;
