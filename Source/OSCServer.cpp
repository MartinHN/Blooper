/*
 ==============================================================================
 
 OSCServer.cpp
 Created: 11 Feb 2016 12:46:06pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#include "OSCServer.h"
#include "OOServer.h"
#include "OOTrack.h"

void OSCServer::connectOSC(){
    if(!(isConnected=connect(OscPort))){
        Logger::outputDebugString("can't connect to port " + String(OscPort));
    };
    
}

bool OSCServer::createClientIfNeeded(const OSCMessage &m){
    //    m.
    
    
    // TODO what clever protocol should we implement
    if(m.getAddressPattern().matches(OSCAddress("/Hi"))){
        String host = m[0].getString();
        if(host == "localhost"){
            host = "127.0.0.1";
        }
        int port = m[1].getInt32();
        for(auto & c:clients){
            if(c->host==host && c->port == port)
                return false;
        }
        OSCClient * cli = new OSCClient(host,port,true);
        for(auto & t:owner->tracks){
            for(auto & p :t.get()->getParameters()){
                cli->bindParameter(t, p, "/Tracks/"+String(*t->groupID) + "/" +String(*t->trackNum) + "/" + p->getName(50) );
            }
        }
        clients.add(cli);
        return true;
    }
    
    else if(m.getAddressPattern().matches(OSCAddress("/Bye"))){
        String host = m[0].getString();
        if(host == "localhost"){
            host = "127.0.0.1";
        }
        int port = m[1].getInt32();
        for(auto & c:clients){
            if(c->host==host && c->port == port){
                clients.removeObject(c);
                break;
            }
        }
        
    }
    
    
    return false;
}


bool OSCServer::getValueFromMessage(const OSCMessage& message,float & val){
//    allow empty messages for boolean actions
    if(message.size() == 0){
        val = 1;
        return true;
    }
    if(message[0].isInt32()){
        val = message[0].getInt32();
        return true;
    }
    else if(message[0].isFloat32()){
        val = message[0].getFloat32();
        return true;
    }
    else if(message[0].isBlob()){
        //not supported
        jassert(false);
    }
    return false;
}
void OSCServer::oscMessageReceived (const OSCMessage& message){
    // we're realtime here, defer if needed
    
    if(createClientIfNeeded(message))
        return;
    
    
    if(OSCAddressPattern("/All/*").matches(message.getAddressPattern().toString())){
        StringArray res;
        res.addTokens(message.getAddressPattern().toString(),"/","");
        res.removeRange(1, 1);
        String current = res.joinIntoString("/");

        for(auto &c:clients){
            for(auto & b:c->boundedParams){
                StringArray res2;
                res2.addTokens(b->address.toString(),"/","");
                // currently /Tracks/i/i/...
                res2.removeRange(1, 3);
                String curS = res2.joinIntoString("/");
                if(curS == current){
                    float val;
                    if(getValueFromMessage(message, val)){
                        b->setValue(val);
                    }
                    
                }
            }
        }
    }
    else{
        
        for(auto &c:clients){
            for(auto & b:c->boundedParams){
                if(message.getAddressPattern() == b->address){
                    float val;
                    if(getValueFromMessage(message, val)){
                        b->setValue(val);
                    }
                    else{
                        DBG("message format not supported : "+String( message[0].getType()));
                    }
                }
            }
            
        }
    }
    
    
    
    
    
    
}