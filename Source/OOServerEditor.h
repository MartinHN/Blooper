/*
  ==============================================================================

    OOServerEditor.h
    Created: 11 Feb 2016 11:12:59am
    Author:  Martin Hermant

  ==============================================================================
*/



// TODO


#ifndef OOSERVEREDITOR_H_INCLUDED
#define OOSERVEREDITOR_H_INCLUDED
#include "JuceHeader.h"

#include "OOServer.h"
#include "OOTrack.h"

class OOServerEditor : public Component,SliderListener{
    
    public :
    
    OOServerEditor(){
        
        transportComponent = new TransportComponent();
        addAndMakeVisible(transportComponent);
        oscComponent = new OSCComponent();
        addAndMakeVisible(oscComponent);
        
        globalDelay = new Slider(Slider::SliderStyle::LinearHorizontal,Slider::TextBoxAbove);
        
        globalDelay->setRange(0, 300);
        addAndMakeVisible(globalDelay);
        
        globalDelay->addListener(this);
        
        setSize(200,200);

    };
    
    ~OOServerEditor(){
        deleteAllChildren();
        
    }
    
    
    
     void paint(Graphics & g) override{
         g.setColour(Colours::lightgrey);
         g.fillAll();


     }
    
    void resized()override{
        Rectangle<int> area = getLocalBounds();
        int width = area.getWidth()/3;
        transportComponent->setBounds(area.removeFromLeft(width));
        oscComponent->setBounds(area.removeFromLeft(width));
        globalDelay->setBounds(area.removeFromLeft(width));
    }
    
    
    void sliderValueChanged (Slider* slider) override{
        for(auto & t :OOServer::getInstance()->tracks){
            t->setRecordDelay(slider->getValue());
        }
    }
    Slider * globalDelay;
    
    class OSCComponent : public Component{
    public:
        OSCComponent(){}
        
        void paint(Graphics & g) override {
            g.setColour(Colours::black);
            g.drawFittedText(String(OOServer::getInstance()->oscServer.clients.size()),getLocalBounds(), Justification::centred, 1);
            
        }
        
    };
    OSCComponent *  oscComponent;
    
    
    class TransportComponent : public Component,public Timer{
        public :
        TransportComponent(){
            startTimer(100);
            setSize(100,100);
        }
        void  timerCallback() override{
            int nextCurrentTime = OOServer::getInstance()->looperState.getCurrentQuarter();
            if(nextCurrentTime != currentTime){
                currentTime = nextCurrentTime;
                if(isVisible())repaint();
            }
        };
        void paint(Graphics & g) override{

            Rectangle<int> area = getLocalBounds();
            g.setColour(Colours::red);
            g.drawRect(area);
            g.setColour(Colours::black);
            int numStacked = 2;
            g.drawFittedText(String(currentTime), area.removeFromTop(area.getHeight()/numStacked), Justification::left, 1);
            g.drawFittedText (String(OOServer::getInstance()->looperState.bpm), area.removeFromTop(area.getHeight()/numStacked), Justification::left, 1);

        }
        
        int currentTime;
    };
    
    
    TransportComponent  * transportComponent;
    
    
};



#endif  // OOSERVEREDITOR_H_INCLUDED
