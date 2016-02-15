//
//  OOTransport.hpp
//  BloopOne
//
//  Created by Martin Hermant on 10/02/2016.
//
//

#ifndef OOTransport_hpp
#define OOTransport_hpp

#include "JuceHeader.h"
#include "OOBaseAudioProcessor.h"


#include "OOBaseClient.h"

#include "OSCServer.h"



/*
 This class is a singleton that synchronize states between all OOObjects
 -> alows synchronisation between instance of OOplugins in host environement
 -> 
 
 */

class OOTrack;
class OOServerEditor;

class OOServer : public AudioProcessorListener
{
public:
    OOServer();
    ~OOServer();
    juce_DeclareSingleton(OOServer, false);


    
    
    OSCServer oscServer;
    void registerTrack( WeakReference<OOTrack> track);
    void unregisterTrack( WeakReference<OOTrack> track);

     void audioProcessorParameterChanged (AudioProcessor* processor,
                                                 int parameterIndex,
                                                 float newValue) override;

     void audioProcessorChanged (AudioProcessor* processor) override{};
    OOTrack * getTrack(int trackId,int tnum);
    void recordTrack(int trackId,int tnum,bool t);
    void playTrack(int trackId,int tnum,bool t);
    void setVolume(int trackId,int tnum,float v);

    void updateGlobalTime(OOTrack * , int samples);
    void setBPMFromTrack(OOTrack *);
   static  bool isMasterBPMTrack(OOTrack *);
    
    Component * createEditor();
    OOServerEditor * ooServerEditor;
    
// lighter class than AudioPlayHead
    class LooperState {
        public :
        
        double bpm = 86;
        
        int timeSigNumerator = 4;
        int timeSigDenominator = 4;
        /** The current play position, in samples from the start of the edit. */
        int64 timeInSamples = 0;
        int sampleRate = 44100;

        int getCurrentQuarter(){
           return 1+ (int)(timeInSamples*1.0/sampleRate * bpm/60.0)% (timeSigNumerator);
        }
        int getSamplesPerMeasure(){
            return (timeSigNumerator*60.0/bpm)*sampleRate;
        }
        
        int getSampleQuantizedToMeasure(int samples){
            int samplesPerMeasure =getSamplesPerMeasure();
            return  ceil(samples*1.0/samplesPerMeasure)* samplesPerMeasure;
        }
    };
    

    LooperState looperState;
//private:
   Array<WeakReference<OOTrack> > tracks;
    

};
#endif /* OOTransport_hpp */
