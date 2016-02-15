/*
  ==============================================================================

    OOComponents.h
    Created: 11 Feb 2016 11:21:25am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef OOCOMPONENTS_H_INCLUDED
#define OOCOMPONENTS_H_INCLUDED

#include "JuceHeader.h"
#include "OOBaseClient.h"

class OOComponentSlider : public OOBaseClient,public Slider,public SliderListener{
public:
    OOComponentSlider(AudioProcessor * proc,AudioParameterFloat * p) : Slider(p->name),OOBaseClient(proc,p),processor(proc),floatParam(p){
        
        setRange(p->range.getRange().getStart(), p->range.getRange().getEnd());
        Slider::setValue(*p,dontSendNotification);
        Slider::setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        Slider::setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 40, 18);
        setSize(40,200);

        Slider::addListener(this);
    }
    
    virtual ~OOComponentSlider(){
        Slider::removeListener(this);
        
    };

    
    // any ui modifications
    void sliderValueChanged (Slider* slider) override{
        *floatParam = slider->getValue();
//        OOBaseClient::setValue();

    };

    
    // refresh UI on message thread
    void handleAsyncUpdate()override{
        Slider::setValue(*floatParam,dontSendNotification);

    }
    
    
    AudioParameterFloat * floatParam;
    AudioProcessor * processor;

};


class OOComponentButton : public OOBaseClient,public TextButton,public ButtonListener{
public:
    OOComponentButton(AudioProcessor * proc,AudioParameterBool * p,bool isToggle = true) : TextButton(p->name),OOBaseClient(proc,p),processor(proc),boolParam(p){
        
        Button::setToggleState(*p,dontSendNotification);
        Button::setClickingTogglesState(isToggle);

        setSize(40,40);
        
        Button::addListener(this);
    }
    
    virtual ~OOComponentButton(){
        Button::removeListener(this);
        
    };
    
    

     void buttonClicked (Button* b) override {
        *boolParam = b->getToggleState();
    };
    
//    /** Called when the button's state changes. */
//    virtual void buttonStateChanged (Button*)  {}
//    
    // refresh UI on message thread
    void handleAsyncUpdate()override{
        Button::setToggleState(*boolParam,dontSendNotification);
        
    }
    
    
    AudioParameterBool * boolParam;
    AudioProcessor * processor;
    
};
#endif  // OOCOMPONENTS_H_INCLUDED
