/*
 ==============================================================================
 
 OOOscClient.h
 Created: 10 Feb 2016 6:00:28pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef OOOSCCLIENT_H_INCLUDED
#define OOOSCCLIENT_H_INCLUDED


/*
 base class for  clients
 
 */

#include "JuceHeader.h"

class OOBaseClient: public AudioProcessorListener,public AsyncUpdater{
public:
    
    OOBaseClient(AudioProcessor * p,AudioProcessorParameter * _param,bool _wantsAsyncUpdate = false,bool _wantsSyncUpdate = false):
    param(_param),isUpdating(false),
    wantsAsyncUpdate(_wantsAsyncUpdate),
    wantsSyncUpdate(_wantsSyncUpdate)
    {
        
        proc =p;
        proc->addListener(this);
    }

    virtual ~OOBaseClient(){
        if(proc)
        proc->removeListener(this);
        
    }
    
    
    void setValue(float v){
        isUpdating = true;
        param->setValueNotifyingHost(v);
        isUpdating = false;
        
        
    }
    
    float getValue(){
        return param->getValue();
    }
    
    virtual void syncUpdate(){};
    void audioProcessorParameterChanged (AudioProcessor* processor,
                                         int parameterIndex,
                                         float newValue)override {
        if( param->getParameterIndex() == parameterIndex){
            if(!isUpdating || wantsSyncUpdate)
                syncUpdate();
            if(!isUpdating || wantsAsyncUpdate)
                triggerAsyncUpdate();
        }
        
        
    };
    void audioProcessorChanged (AudioProcessor* processor) override{
        
    };
    
    
private:
    
    bool wantsAsyncUpdate;
    bool wantsSyncUpdate;
    AudioProcessorParameter * param;
    AudioProcessor* proc;
    bool isUpdating;
};


#endif  // OOOSCCLIENT_H_INCLUDED
