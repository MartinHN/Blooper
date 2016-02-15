//
//  OOTransport.cpp
//  BloopOne
//
//  Created by Martin : on 10/02/2016.
//
//

#include "OOServer.h"


#include "OOTrack.h"

juce_ImplementSingleton(OOServer);


#include "OOServerEditor.h"

OOServer::OOServer():oscServer(this){
    ooServerEditor = nullptr;
}

OOServer::~OOServer(){
    delete ooServerEditor;
}

void OOServer::audioProcessorParameterChanged (AudioProcessor* processor,
                                               int parameterIndex,
                                               float newValue) {
    OOTrack * t = dynamic_cast<OOTrack*>(processor);
    if(t){
        //record in sync
        if(parameterIndex == t->isRecording->getParameterIndex()){
            // stop recording
            if(!*t->isRecording){
                // get BPM from master
                if( isMasterBPMTrack(t)){
                    setBPMFromTrack(t);
                }
            }
        }
        if(parameterIndex == t->shouldRecord->getParameterIndex()){
            // stop recording
            if(!*t->shouldRecord){
                // get BPM from master
                if( isMasterBPMTrack(t)){
//                    setBPMFromTrack(t);
                }
                
                //quantize recordLength
                else {
                    
                    t->quantizedRecordEnd = looperState.getSampleQuantizedToMeasure(looperState.timeInSamples);
                }
            }
            
            // start recording
            
            // quantize slave
            else if(!isMasterBPMTrack(t)){
                t-> quantizedRecordStart =  looperState.getSampleQuantizedToMeasure(looperState.timeInSamples);
            }
            
            // start new Master recording
            else {
                looperState.timeInSamples = 0;
            }
        }
        
        //play in Sync
        
        if(parameterIndex == t->shouldPlay->getParameterIndex() && !isMasterBPMTrack(t)){
            if(!*t->shouldPlay){
                t->quantizedPlayEnd = looperState.getSampleQuantizedToMeasure(looperState.timeInSamples);
                t->quantizedPlayStart = -1;
            }
            else{
                t->quantizedPlayEnd = -1;
                t-> quantizedPlayStart = looperState.getSampleQuantizedToMeasure(looperState.timeInSamples);
            }
        }
        
    }
    
}

//TODO clever Mechanism ?
bool OOServer::isMasterBPMTrack(OOTrack *t){
    return *t->trackNum == 0 && *t->groupID ==0;
}



Component * OOServer::createEditor(){
    return new OOServerEditor();
}


void OOServer::updateGlobalTime(OOTrack *t , int samples){
    if(isMasterBPMTrack(t)){
        if(looperState.sampleRate != t->getSampleRate()){
            looperState.sampleRate = t->getSampleRate();
            // TODO notify any sampleRate change here
        };
        if(*t->isPlaying || *t->isRecording){
            if((t->recordNeedle.get() == 0 )){
                looperState.timeInSamples = 0;
            }
            looperState.timeInSamples+=samples;
            
        }
        else{
            if(looperState.timeInSamples>0){
                looperState.timeInSamples = 0;
            }
        }
    }
}

//Todo better handling ? for now really cheap...
void OOServer::setBPMFromTrack(OOTrack *t){
    double time = t->getRecordedLengthInSample() / t->getSampleRate();
    double beatTime = time* 1.0/looperState.timeSigNumerator;
    
    // over 150 bpm
    if(beatTime < .40){beatTime*=2;}
    // under 60 bpm
    else if(beatTime > 1){beatTime/=2;}
    
    looperState.bpm = 60/(beatTime);
}



OOTrack * OOServer::getTrack(int trackId,int trackNum){
    if(trackNum>= tracks.size()){jassert(false);return nullptr;}
    return dynamic_cast<OOTrack*>(tracks[trackNum].get());;
}

void OOServer::recordTrack(int trackID,int trackNum, bool state){
    OOTrack * t  = getTrack(trackID, trackNum);
    if(t==nullptr){jassert(false);return;}
    *t->shouldRecord = state;
    
}
void OOServer::playTrack(int trackID,int trackNum, bool state){
    OOTrack * t  = getTrack(trackID, trackNum);
    if(t==nullptr){jassert(false);return;}
    *t->shouldPlay = state;
    
}
void OOServer::setVolume(int trackID,int trackNum, float v){
    OOTrack * t  = getTrack(trackID, trackNum);
    if(t==nullptr){jassert(false);return;}
    *t->volume  = v;
    
}




void OOServer::registerTrack( WeakReference<OOTrack> track){

    *track->groupID = (int)tracks.size()/8;
    *track->trackNum = tracks.size()%8;
    
    track->addListener(this);
    tracks.add(track);
    
}

void OOServer::unregisterTrack( WeakReference<OOTrack>  track){
    tracks.removeFirstMatchingValue(track);
    track->removeListener(this);
    // delete if no more Tracks to sync
    if(tracks.size() == 0)
        deleteInstance();
    
    
    // weird bug in juce host it deletes OOTracks after creating new ones so index getCorrupted
    for(int i = 0 ; i < tracks.size() ; i++){
        *tracks[i]->groupID = (int)i/8;
        *tracks[i]->trackNum = i%8;
    }
    
}



