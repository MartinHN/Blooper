//
//  OOTrack.hpp
//  BloopOne
//
//  Created by Martin Hermant on 10/02/2016.
//
//

#ifndef OOTrack_hpp
#define OOTrack_hpp

#include "OOBaseAudioProcessor.h"
#include "BipBuffer.h"

/*
 
 this class represent a looperTrack plugin
 multiple track can be added in a host environement and will be synced
 
 
 TODO : see if we can use     AudioProcessorValueTreeState valueTree;
 */




class OOTrack : public OOBaseAudioProcessor ,public AudioProcessorListener{
    public :
    OOTrack();
    virtual ~OOTrack();
    virtual const String getName() const override { return "LooperTrack";};
    
    //==============================================================================
    
    void init(double sampleRate, double blockSize);
 
    int getRecordedLengthInSample();
    AudioProcessorEditor* createEditor()override;
    
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)override;
    float  getLoopLength();
    float getNormalizedPosition();
    
    WeakReference<OOTrack>::Master masterReference;
    
    float getRMS();
    
    void setRecordDelay(float ms);
private:
    
    
    AudioParameterBool *  shouldRecord;
    AudioParameterBool * shouldPlay;
    AudioParameterBool * shouldClear;
    AudioParameterFloat * volume;
    float lastVolume;
    AudioParameterChoice * state;
    
    
    
    AudioParameterBool *  isRecording;
    Atomic<int> recordNeedle;
    int recordingDelay;
    int quantizedRecordEnd,quantizedRecordStart;
    
    AudioParameterBool * isPlaying;
    int playNeedle;
    int quantizedPlayStart,quantizedPlayEnd;
    void updatePendingLooperState(int64 curTime);
    //for global Id see OOServer
    AudioParameterInt *  groupID, * trackNum;
    
    
    AudioSampleBuffer monoLoopSample;
    
    // keeps track of few bits of audio
    // to readjust the loop when controllers are delayed
    BipBuffer streamBipBuffer;
    AudioParameterInt * streamBipBufferDelay;
    
    
    float rmsAlpha = 0.1;
    float rmsOut;
    

    
    class TrackState : public ChangeBroadcaster,public AudioProcessorListener{
        public :
        TrackState(OOTrack * t) : track(t){
            t->addListener(this);
        }
        enum State{
        stopped = 0,
        shouldPlay,
        isPlaying,
        shouldRecord,
        isRecording,
        overdub
        };
        
        State state = stopped;
        OOTrack * track;
        
        void audioProcessorParameterChanged (AudioProcessor* processor,
                                             int parameterIndex,
                                             float newValue) override{

            
            if(processor->getParameters()[parameterIndex] == track->shouldRecord){
                if(*track->shouldRecord){
                    state = shouldRecord;
                    sendChangeMessage();
                }
            }
            else if(processor->getParameters()[parameterIndex] == track->isRecording){
                if(*track->isRecording){
                    state = isRecording;
                    sendChangeMessage();}
            }
            else if(processor->getParameters()[parameterIndex] == track->shouldPlay){
                if(*track->shouldPlay){
                    state = shouldPlay;
                    sendChangeMessage();
                    
                }
            }
            else if(processor->getParameters()[parameterIndex] == track->isPlaying){
                state = *track->isPlaying?isPlaying : stopped;
                sendChangeMessage();
                
                
            }
        
        }
        
        void audioProcessorChanged (AudioProcessor* processor) override{}
    };
    
    
    TrackState* trackState ;
    
    void audioProcessorParameterChanged (AudioProcessor* processor,
                                                 int parameterIndex,
                                                 float newValue) override;
    
     void audioProcessorChanged (AudioProcessor* processor) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OOTrack)
    friend class OOTrackEditor;
    friend class OOServer;
    friend class OSCServer;


};

#endif /* OOTrack_hpp */
