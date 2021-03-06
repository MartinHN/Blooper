/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "OOProcessorParameter.h"
#include "OOServer.h"

//==============================================================================
/**
 helper class to define dummys function from AudioProcessor
 just to get the code cleaner in OOTrack
*/
class OOBaseAudioProcessor  : public AudioProcessor,public OSCReceiver::ListenerWithOSCAddress<>
{
public:
    //==============================================================================
    OOBaseAudioProcessor();
    ~OOBaseAudioProcessor();

    
    virtual void oscMessageReceived (const OSCMessage& message) override {};
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    virtual void processBlock (AudioSampleBuffer&, MidiBuffer&) override = 0;

    //==============================================================================
    virtual  AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    


//    WeakReference<OOBaseAudioProcessor>::Master masterReference;


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OOBaseAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
