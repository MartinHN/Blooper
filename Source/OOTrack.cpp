//
//  OOTrack.cpp
//  BloopOne
//
//  Created by Martin Hermant on 10/02/2016.
//
//

#include "OOTrack.h"

#include "OOTrackEditor.h"

#define MAX_LOOP_LENGTH_S 30

OOTrack::OOTrack()
:monoLoopSample(1,44100*MAX_LOOP_LENGTH_S),
// 16000 ~ 300ms and 256*64
streamBipBuffer(16384)

{
    
    
    
    quantizedRecordEnd = -1;
    quantizedRecordStart = -1;
    quantizedPlayStart = -1;
    quantizedPlayEnd = -1;

    
    shouldRecord= new AudioParameterBool("shouldRec","shouldRec",false);
    isRecording= new AudioParameterBool("Rec","Rec",false);
    shouldPlay= new AudioParameterBool("shouldPlay","shouldPlay",false);
    isPlaying= new AudioParameterBool("Play","Play",false);
    
    shouldClear = new AudioParameterBool("shouldClear","shouldClear",false);
    volume= new AudioParameterFloat("Volume","Volume",NormalisableRange<float>(0,1),1);
    
    groupID = new AudioParameterInt("GroupID","GroupID",-1,10,-1);
    trackNum = new AudioParameterInt("trackNum","trackNum",0,10,0);
    streamBipBufferDelay = new AudioParameterInt("RecordDelay","RecordDelay",0,streamBipBuffer.buf.getNumSamples(),0);
    
    addParameter(shouldRecord);
    addParameter(isRecording);
    
    addParameter(shouldPlay);
    addParameter(isPlaying);
    
    
    addParameter(shouldClear);
    addParameter(volume);
    
    addParameter(groupID);
    addParameter(trackNum);
    
    
    addParameter(streamBipBufferDelay);
    
    OOServer::getInstance()->registerTrack(this);
    init(44100, 512); // update from outside
    
    
    trackState = new TrackState(this);
    addListener(this);
    
    lastVolume = 1;
    
}

OOTrack::~OOTrack() {
    OOServer::getInstance()->unregisterTrack(this);
    delete trackState;
    OOTrack::masterReference.clear();
};


void OOTrack::init(double sampleRate, double blockSize)
{
    this->setPlayConfigDetails (1, 1, sampleRate, blockSize);
    
}

AudioProcessorEditor* OOTrack::createEditor()
{
    return new OOTrackEditor (*this);
}


int OOTrack::getRecordedLengthInSample(){
    return recordNeedle.get() ;
}


// looper internal state update
void OOTrack::audioProcessorParameterChanged (AudioProcessor* processor,
                                     int parameterIndex,
                                     float newValue) {
    if(parameterIndex == isRecording->getParameterIndex()){

        if(*isRecording){
            *isPlaying = false;
            *shouldPlay = false;
            quantizedRecordStart = -1;
            
            if(*streamBipBufferDelay>0){
                monoLoopSample.copyFrom(0,0,streamBipBuffer.getLastBlock(*streamBipBufferDelay),*streamBipBufferDelay);
                recordNeedle = *streamBipBufferDelay;
            }
            else{
                *streamBipBufferDelay = 0;
                recordNeedle = 0;
            }

        }
        else{
            
            
            recordNeedle-=*streamBipBufferDelay;
            // 22 ms if 44100
            int fadeNumSaples = 10;
            if(recordNeedle.get()>2*fadeNumSaples){
            monoLoopSample.applyGainRamp(0, 0, fadeNumSaples, 0, 1);
            monoLoopSample.applyGainRamp(0,recordNeedle.get() - fadeNumSaples, fadeNumSaples, 1, 0);
            }
            
            quantizedRecordEnd = -1;
        }
        
        

    }
    
    else if(parameterIndex == isPlaying->getParameterIndex()){
        if(*isPlaying){
            *isRecording = false;
            *shouldRecord = false;
            quantizedPlayStart = -1;
            playNeedle = 0;
        }
        else{
            quantizedPlayEnd = -1;
        }

    }
    
    else if(parameterIndex == shouldClear->getParameterIndex()){
        if(*shouldClear){
            recordNeedle = 0;
            playNeedle = 0;
            quantizedPlayEnd = -1;
            quantizedPlayStart = -1;
            quantizedRecordEnd = -1;
            quantizedRecordStart = -1;
            *shouldClear = false;

        }
    }
    
    else if(parameterIndex == isRecording->getParameterIndex()){
        if(*isRecording){
            *shouldClear = true;
        }
    }
};



void OOTrack::audioProcessorChanged (AudioProcessor* processor) {};

float  OOTrack::getLoopLength(){
//    jassert(!*shouldRecord);
    return recordNeedle.get() / getSampleRate();
    
}
float OOTrack::getNormalizedPosition(){
    if(*isPlaying){
        return playNeedle*1.0 / recordNeedle.get();
    }
    else if(*isRecording){
        return getLoopLength() *1.0/ MAX_LOOP_LENGTH_S;
    }
    else return 0;
}

void OOTrack::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    
    updatePendingLooperState(OOServer::getInstance()->looperState.timeInSamples);
    OOServer::getInstance()->updateGlobalTime(this,buffer.getNumSamples());
    // RECORDING
    if (*isRecording)
    {
        if(recordNeedle.get() + buffer.getNumSamples()> getSampleRate() * MAX_LOOP_LENGTH_S){
            *shouldRecord = false;
            *isRecording = false;
        };
        monoLoopSample.copyFrom(0, recordNeedle.get(), buffer, 0, 0, buffer.getNumSamples());
        recordNeedle += buffer.getNumSamples();
        rmsOut = (1-rmsAlpha)*rmsOut+ rmsAlpha*buffer.getRMSLevel(0, 0, buffer.getNumSamples());
        
        
    }
    else{
        streamBipBuffer.writeBlock(buffer);
    }
    // PLAYING
    // allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
    // we may need it if we start to use a different clock  than looperState in OOServer that has a granularity of blockSize
    // or if we dynamicly change blockSize
    if (*isPlaying && recordNeedle.get()>0 && monoLoopSample.getNumSamples())
    {
        if ( (playNeedle + buffer.getNumSamples()) > recordNeedle.get())
        {
            
            //assert false for now see above
//            jassert(false);
            int firstSegmentLength = recordNeedle.get() - playNeedle;
            int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, firstSegmentLength);
            buffer.copyFrom(0, 0, monoLoopSample, 0, 0, secondSegmentLength);
            playNeedle = secondSegmentLength;
            
        }else{
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, buffer.getNumSamples());
            playNeedle += buffer.getNumSamples();
            playNeedle %= recordNeedle.get();
        }
            buffer.applyGainRamp(0, 0, buffer.getNumSamples(), lastVolume,*volume);
            lastVolume = *volume;
        
        
        
            rmsOut = (1-rmsAlpha)*rmsOut+ rmsAlpha*buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    }
    else{
        // silence output buffer
        buffer.applyGain(0, 0, buffer.getNumSamples(), 0);
    }

}


float OOTrack::getRMS(){
    return rmsOut;
}

void OOTrack::setRecordDelay(float ms){
    if(!*isRecording)
        *streamBipBufferDelay = ms * getSampleRate() * 0.001;
    else{
        jassert(false);
    }
    
}

void OOTrack::updatePendingLooperState(int64 curTime){

    if(quantizedRecordStart>0){
        if(curTime>quantizedRecordStart){
            *streamBipBufferDelay = 0;
            *isRecording = true;

        }
        
    }
    else if( quantizedRecordEnd>0){
        if(curTime>quantizedRecordEnd){
            *streamBipBufferDelay = 0;
            *isRecording = false;
        }
    }
    
    else if(*isRecording!=*shouldRecord){
        *isRecording = (bool)*shouldRecord;
    }
    
    
    if(quantizedPlayStart>0){
        if(curTime>quantizedPlayStart){
            *isPlaying = true;
        }
    }
    else if( quantizedPlayEnd>0){
        if(curTime>quantizedPlayEnd){
            *isPlaying = false;
        }
    }
    else if(*isPlaying!= *shouldPlay){
        *isPlaying = (bool)*shouldPlay;
    }
    
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OOTrack();
}
