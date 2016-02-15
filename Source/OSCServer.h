/*
 ==============================================================================
 
 OSCServer.h
 Created: 11 Feb 2016 12:46:06pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef OSCSERVER_H_INCLUDED
#define OSCSERVER_H_INCLUDED

#include "JuceHeader.h"
#include "OSCClient.h"


class OOServer;
class OSCServer : public
OSCReceiver,
OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    OSCServer(OOServer * _owner):owner(_owner){
        OscPort = 20000;
        connectOSC();
        
        addListener(this);
    };
    int OscPort;
    void connectOSC();
    bool isConnected;
    
    // becareful, its realTime
    void oscMessageReceived (const OSCMessage& message);
    OOServer *  owner;
    bool getValueFromMessage(const OSCMessage& message,float & val);
    bool createClientIfNeeded(const OSCMessage &m);
    
    OwnedArray<OSCClient> clients;
};


#endif  // OSCSERVER_H_INCLUDED
