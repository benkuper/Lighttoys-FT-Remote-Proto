#pragma once

#include "CommandProvider.h"
#include "Config.h"

class UDPManager :
  public CommandProvider
{
  public:
    UDPManager() :
      CommandProvider("UDP")
    {

    }

    WiFiUDP udp;
    const int localPort = 8888;
    String remoteHost;
    int remotePort;

    String buffer;

    int opMode;

    void init()
    {
      udp.stop();
      udp.begin(localPort);
      buffer = "";

      remoteHost = Config::instance->getRemoteHost();
      remotePort = Config::instance->getRemoteUDPPort();

      DBG("UDP is init, sending to "+String(remoteHost)+":"+String(remotePort));
    }

    void update()
    {
      int size = udp.parsePacket();
      for (int i = 0; i < size; i++)
      {
        char c = udp.read();
        if (c == '\n')
        {
          parseMessage(buffer);
          buffer = "";
        } else
        {
          buffer += c;
        }
      }
    }

    void parseMessage(String message)
    {
      digitalWrite(13, HIGH);
      delay(2);
      digitalWrite(13, LOW);

      if(message.startsWith("yo"))
      {
        remoteHost = udp.remoteIP().toString();
        Config::instance->setRemoteHost(remoteHost);
        
        int port = message.substring(3).toInt();
        if(port > 0)
        {
          remotePort = port;
          Config::instance->setRemoteUDPPort(remotePort);
        }

        DBG("Got yo, now sending to "+String(remoteHost)+":"+String(remotePort));

        String ip = (WiFi.getMode() == WIFI_STA ? WiFi.localIP() : WiFi.softAPIP()).toString();
        sendMessage("wassup "+ip);
        return;
      }
      
      CommandData d;
      d.type = MESSAGE_RECEIVED;
      d.value1.stringValue = (char *)message.c_str();
      sendCommand(d);
    }

    void sendRawMessage(String message)
    {
      if(message.length() == 0) return;
      udp.beginPacket(remoteHost.c_str(), remotePort);
      udp.printf(message.c_str());
      udp.endPacket();
    }

    void sendMessage(String message)
    {
      sendRawMessage(message+"\n");
    }
};
