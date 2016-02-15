/*
 ==============================================================================
 
 OSCClient.h
 Created: 11 Feb 2016 2:43:09pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef OSCCLIENT_H_INCLUDED
#define OSCCLIENT_H_INCLUDED

#include "JuceHeader.h"
#include "OOBaseClient.h"

class OSCClient : public OSCSender{
public:
    OSCClient(String _host,int _port,bool _wantsHisOwnFeedback=false):host(_host),port(_port),wantsHisOwnFeedback(_wantsHisOwnFeedback){
        if(!connect(host,port)){
            Logger::outputDebugString("can't connect to "+ host + "on port "+ String(port));
        }
        else
            Logger::outputDebugString("new client connected to "+ host + "on port "+ String(port));
    }
    
    
    
    void bindParameter( AudioProcessor * proc ,AudioProcessorParameter * param,String ad){
        OSCBind *b = new OSCBind(this,proc,param,ad,wantsHisOwnFeedback);
        boundedParams.add(b);
        b->triggerAsyncUpdate();
    }
    
    
    ~OSCClient(){
        
    }
    
    String host;
    int port;
    
    
    class OSCBind:  public OOBaseClient{
    public:
        OSCBind(OSCSender * _owner,
                AudioProcessor * proc ,
                AudioProcessorParameter * param ,
                String ad,
                bool feedBack
                ) :
        OOBaseClient(proc,param,feedBack),address(ad),sender(_owner)
        {

        }
        void handleAsyncUpdate() override{
            sender->send(address, OOBaseClient::getValue());
        }
        
        
        OSCAddressPattern address;
        OSCSender * sender;
    };
    
    
    OwnedArray<OSCBind> boundedParams;
private:
    bool wantsHisOwnFeedback;

    
};



#endif  // OSCCLIENT_H_INCLUDED
