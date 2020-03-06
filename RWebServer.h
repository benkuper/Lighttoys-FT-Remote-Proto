#pragma once
#include "CommandProvider.h"
#include "Config.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#if USE_ETHERNET
#include <ETH.h>
#endif

class RWebServer :
  public CommandProvider
{
  public:
    RWebServer() : CommandProvider("WebServer"), server(80)
    {
    }

    ~RWebServer() {}

    WebServer server;

    void init()
    {
      server.on("/", std::bind(&RWebServer::handleRoot, this));
      server.on("/saveConfig", std::bind(&RWebServer::handleSaveConfig, this));
      server.onNotFound(std::bind(&RWebServer::handleNotFound, this));
      server.begin();
      DBG("HTTP server started");
    }

    void update()
    {
      server.handleClient();
    }


    // **************************************** WEB SERVER FUNCTIONS

    void handleRoot() {
      String form = getForm();
      String content = wrapContent(form);
      server.send(200, "text/html", content);
    }

    void handleSaveConfig() {
      String message = "Save Config";
      for (uint8_t i = 0; i < server.args(); i++) {
        message += " > " + server.argName(i) + ": " + server.arg(i) + "\n";

        if(server.argName(i) == "name") Config::instance->setDeviceName(server.arg(i));
        else if(server.argName(i) == "ssid") Config::instance->setWifiSSID(server.arg(i));
        else if(server.argName(i) == "pass") Config::instance->setWifiPassword(server.arg(i));
        else if(server.argName(i) == "ip") Config::instance->setStaticIP(server.arg(i));
        else if(server.argName(i) == "gateway") Config::instance->setGateway(server.arg(i));
        else if(server.argName(i) == "subnet") Config::instance->setSubnetMask(server.arg(i));
      }

      DBG(message);
      server.send(200, "text/html", getConfirmation());

      delay(1000);
      ESP.restart();
    }


    void handleNotFound() {
      String message = "File Not Found\n\n";
      message += "URI: ";
      message += server.uri();
      message += "\nMethod: ";
      message += (server.method() == HTTP_GET) ? "GET" : "POST";
      message += "\nArguments: ";
      message += server.args();
      message += "\n";

      for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      }

      server.send(404, "text/plain", message);

    }

    String wrapContent(String content)
    {
      String ip = (WiFi.getMode() == WIFI_STA ? WiFi.localIP() : WiFi.softAPIP()).toString();
#if USE_ETHERNET
    String ethIP  = ETH.localIP().toString();
#endif
      String result = "<html><head><title>FT Remote Configuration</title>";
      result += "<style>.inline{ display:inline-block; width:100px; }</style>";
      result += "</head>";
      result += "<body id='main_body' style='margin: 0;background-color: #222;font-family: Arial;font-size: 14px;width: 100%;height: 100%;padding: 0; text-align:center'>";
      result += "<img src='"+String(getLogoData())+"' style='margin:30px 0 0' /><br>";
      result += "<div id='form_container' style='background-color: #333; color: #eee; border: solid 1px #444;width: 60%;left: 20%;position: absolute;margin-top: 20px;text-align: center;padding: 10px 0 20px; line-height:2em'>";
      result += "<h1><a>FT Remote Configuration</a></h1>";
      if(ip != "0.0.0.0") result += "<h2>Current WiFi IP : " + ip + "</h2>";
#if USE_ETHERNET
      if(ethIP != "0.0.0.0") result += "<h2>Current Ethernet IP : " + ethIP + "</h2>";
#endif
      result +=  content + "</body></html>";

      return result;
    }

    String getForm()
    {
      String result = "<form id='config-form' method='post' action='/saveConfig'>";
      result +=   "<label>Device Name (FTRemote ***) </label><br>";
      result +=   "<input id='name' name='name' type='text' value='" + Config::instance->getDeviceName() + "'><br><br>";
      result +=   "<label style='text-align:center;'>WiFi settings</label><br>";
      result +=   "<span><span class='inline'>SSID</span><input id='ssid' name='ssid' value='" + Config::instance->getWifiSSID() + "'></span><br>";
      result +=   "<span><span class='inline'>Password</span><input id='pass' name='pass' value='" + Config::instance->getWifiPassword() + "'></span> <br><br>";
      result +=   "<label>Static IP Configuration (leave empty for automatic DHCP)</label><br>";
      result +=   "<span><span class='inline'>IP</span><input id='ip' name='ip'  value='" + Config::instance->getStaticIP() + "'></span><br>";
      result +=   "<span><span class='inline'>Gateway</span><input id='gateway' name='gateway' value='" + Config::instance->getGateway() + "'></span><br>";
      result +=   "<span><span class='inline'>Subnet mask</span><input id='subnet' name='subnet' value='" + Config::instance->getSubnetMask() + "'></span><br>";
      result +=   "<br /><input type='submit' value='Save'/>";
      result +=   "</form></div>";

      return result;
    }

    String getConfirmation()
    {
      String result = "<html><header><meta http-equiv='refresh' content='5;url=/' /></header>";
      result += "<body>The configuration has been saved ! You will be automatically redirected to the home page.</body>";
      return result;
    }
    

    // NICE STUFF
    String getLogoData()
    {
      return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAAAdCAYAAADvqyMCAAAACXBIWXMAAAsTAAALEwEAmpwYAAAJUWlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgNS42LWMxNDUgNzkuMTYzNDk5LCAyMDE4LzA4LzEzLTE2OjQwOjIyICAgICAgICAiPiA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0iIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIgeG1sbnM6c3RSZWY9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZVJlZiMiIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8vbnMuYWRvYmUuY29tL3Bob3Rvc2hvcC8xLjAvIiB4bWxuczp0aWZmPSJodHRwOi8vbnMuYWRvYmUuY29tL3RpZmYvMS4wLyIgeG1sbnM6ZXhpZj0iaHR0cDovL25zLmFkb2JlLmNvbS9leGlmLzEuMC8iIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIgeG1wTU06RG9jdW1lbnRJRD0iYWRvYmU6ZG9jaWQ6cGhvdG9zaG9wOjliOTJjZTZkLWRjZWMtMWY0My1hZjA3LTMxMGRkNmI5YzkxNSIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDozMGFkNjZjZC02YThkLWViNGMtYWI3MC1lNTNhY2UwYjk4NDIiIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0iMjUzN0U4NDI4MzI3Mzk0RDAzNDBFMTQ4NzdFRDA4NzgiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIiBwaG90b3Nob3A6Q29sb3JNb2RlPSIzIiBwaG90b3Nob3A6SUNDUHJvZmlsZT0iIiB0aWZmOkltYWdlV2lkdGg9IjEzNzQiIHRpZmY6SW1hZ2VMZW5ndGg9IjIwMCIgdGlmZjpQaG90b21ldHJpY0ludGVycHJldGF0aW9uPSIyIiB0aWZmOlNhbXBsZXNQZXJQaXhlbD0iMyIgdGlmZjpYUmVzb2x1dGlvbj0iOTYvMSIgdGlmZjpZUmVzb2x1dGlvbj0iOTYvMSIgdGlmZjpSZXNvbHV0aW9uVW5pdD0iMiIgZXhpZjpFeGlmVmVyc2lvbj0iMDIyMSIgZXhpZjpDb2xvclNwYWNlPSI2NTUzNSIgZXhpZjpQaXhlbFhEaW1lbnNpb249IjEzNzQiIGV4aWY6UGl4ZWxZRGltZW5zaW9uPSIyMDAiIHhtcDpDcmVhdGVEYXRlPSIyMDIwLTAyLTI2VDE3OjM4OjExKzAxOjAwIiB4bXA6TW9kaWZ5RGF0ZT0iMjAyMC0wMi0yNlQxNzozOTowNCswMTowMCIgeG1wOk1ldGFkYXRhRGF0ZT0iMjAyMC0wMi0yNlQxNzozOTowNCswMTowMCI+IDx4bXBNTTpIaXN0b3J5PiA8cmRmOlNlcT4gPHJkZjpsaSBzdEV2dDphY3Rpb249InNhdmVkIiBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOmI2ZDM2NjI2LWJhYWUtZDQ0OS1hNmZhLWE4MjQ5NTk2N2FlOCIgc3RFdnQ6d2hlbj0iMjAyMC0wMi0yNlQxNzozOTowNCswMTowMCIgc3RFdnQ6c29mdHdhcmVBZ2VudD0iQWRvYmUgUGhvdG9zaG9wIENDIDIwMTkgKFdpbmRvd3MpIiBzdEV2dDpjaGFuZ2VkPSIvIi8+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJjb252ZXJ0ZWQiIHN0RXZ0OnBhcmFtZXRlcnM9ImZyb20gaW1hZ2UvanBlZyB0byBpbWFnZS9wbmciLz4gPHJkZjpsaSBzdEV2dDphY3Rpb249ImRlcml2ZWQiIHN0RXZ0OnBhcmFtZXRlcnM9ImNvbnZlcnRlZCBm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";
    }
};
