#pragma once

class CommandProvider
{
public:
    CommandProvider(String id) :
      providerId(id)
    {
      setCommandCallback(&CommandProvider::onCommandDefaultCallback);   
    }
    
    virtual ~CommandProvider()
    {
    }

    String providerId;
    enum CommandType { 
            
      MESSAGE_RECEIVED, // DIRECT MODE
      
      //API MODE
      START_SYNC, ADD_SYNC, STOP_SYNC,
      FULL_COLOR, COLOR_AB, COLOR_MODE, BRIGHTNESS, PING, BLACKOUT,
      START_SHOW, STOP_SHOW
      };
    
    union var
    {
      int intValue;
      float floatValue;
      char * stringValue;
    };
      
    struct CommandData
    {
      CommandType type;
      var value1;
      var value2;
      var value3;
      var value4;
    };

    void sendCommand(CommandData data) { onCommand(providerId, data); } 
    void sendCommand(CommandType type) { CommandData d; d.type = type; onCommand(providerId, d); } 

    typedef void(*CommandEvent)(String providerId, CommandData command);
    void (*onCommand) (String providerId, CommandData command);
    void setCommandCallback (CommandEvent func) { onCommand = func; }
    static void onCommandDefaultCallback(String providerId, CommandData command) {}
};
