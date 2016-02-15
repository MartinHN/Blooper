/*
  ==============================================================================

    OORMSComponent.h
    Created: 12 Feb 2016 10:21:01am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef OORMSCOMPONENT_H_INCLUDED
#define OORMSCOMPONENT_H_INCLUDED

#include "juceHeader.h"

#include "OOTrack.h"
class OORMSComponent : public Timer,public Component{
public:
    OORMSComponent(OOTrack * t):track(t){
        startTimer(20);
        currentRMS = 0.001;
        setSize(70,70);
        numSteps = 50;
        angularResolution = (2*double_Pi/numSteps);
        
        clearRMS();
    }
    
    
    void clearRMS(){

        RMSValues.resize(numSteps);
        RMSValues.clear();
        lastIdx = 0;
    }
    void paint(Graphics& g) override{
        g.setColour(juce::Colours::green);

        g.fillPath(enveloppePath);
    }
    void timerCallback() override{
        
        
        int nextIdx = track->getNormalizedPosition()*numSteps;
        currentRMS = jmax(jmin(track->getRMS(),0.999f),0.0001f);
        RMSValues.set(nextIdx, RMSValues[nextIdx] *alpha + (1-alpha)* currentRMS*scale);
        if(nextIdx!=lastIdx){
            Rectangle<int> area = getLocalBounds();
            Point<int> center = area.getCentre();
            
            float innerRadius = area.getHeight()/2.0/4.0;
            enveloppePath.clear();
            for(int i = 0 ; i < numSteps ; i++){
            float size =  2*innerRadius + area.getHeight()*(RMSValues[i]);
                float halfWidth = 0.4;
            enveloppePath.addPieSegment( center.x-size/2, center.y-size/2, size,size,
                                        (i-halfWidth)* angularResolution, (i+halfWidth)*angularResolution,
                                        innerRadius/(size/2.0));
            }
            lastIdx = nextIdx;
            
            if(isVisible()){
             repaint();
            }
        }
        
        



    }
    
    
    Array<float> RMSValues;
    float scale = 4;
    float alpha = 0.5;
    int lastIdx ;
    int numSteps;
    float angle;
    float angularResolution;
    Path enveloppePath;
    float currentRMS;
    OOTrack * track;
};

#endif  // OORMSCOMPONENT_H_INCLUDED
