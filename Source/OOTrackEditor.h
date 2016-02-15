/*
 ==============================================================================
 
 OOTrackEditor.h
 Created: 10 Feb 2016 7:59:40pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef OOTRACKEDITOR_H_INCLUDED
#define OOTRACKEDITOR_H_INCLUDED

#include "OOBaseProcessorEditor.h"
#include "OOTrack.h"
#include "OOComponents.h"
#include "OORMSComponent.h"


class OOTrackEditor : public AudioProcessorEditor,AudioProcessorListener{
    public :
    
    OOTrackEditor(OOTrack& t) : AudioProcessorEditor(&t),track(&t){
        t.addListener(this);
        curState =  "unknown";
        
        //Instanciation
        
        
        
        volumeSlider = new OOComponentSlider(&t,t.volume);
        addAndMakeVisible(volumeSlider);
        
        recButton = new OOComponentButton(&t,t.shouldRecord);
        addAndMakeVisible(recButton);
        
        playButton = new OOComponentButton(&t,t.shouldPlay);
        addAndMakeVisible(playButton);
        
        rmsComponent = new OORMSComponent(&t);
        addAndMakeVisible(rmsComponent);
        
        serverComponent = OOServer::getInstance()->createEditor();
        addAndMakeVisible(serverComponent);
        

        trackStateComponent = new TrackStateComponent(t);
        addAndMakeVisible(trackStateComponent);
        
        //Layout
        

        
        setSize(300,400);

        
        
        
    }
    
    ~OOTrackEditor() {
        
        // not deleted when using juce audio host demo ?
        deleteAllChildren();
        processor.removeListener(this);
        
    }
    
    
    
    virtual void paint (Graphics& g) override{
        g.fillAll (Colours::darkgrey);

    };
    virtual void resized() override{
        Rectangle<int> area = getLocalBounds();
        Rectangle<int> header = area.removeFromTop(50);
        serverComponent->setBounds(header.removeFromLeft(header.getWidth()*.5));
        trackStateComponent->setBounds(header);
        
        Rectangle<int> mainFrame = area.removeFromTop(300);
        rmsComponent->setBounds(mainFrame);
        rmsComponent->toBack();
        volumeSlider -> setBounds(mainFrame.reduced(mainFrame.getHeight()*.40));

        
        playButton->setBounds(area.removeFromLeft(area.getWidth()*.5));
        recButton->setBounds(area);
        
//        serverComponent->setBounds("0,0,parent.width/2,top+50");
//        trackStateComponent->setBounds("parent.width/2,0,parent.width,top+50");
//        
//        playButton->setBounds("parent.right -100 , 50, left + 100, top + 100");
//        recButton->setBounds("parent.right - 100, 150, left + 100, top + 100");
//        rmsComponent->setBounds("0, 250, left + 70, top + 70");
//        volumeSlider -> setBounds("110 , 50 , left + 40 ,parent.height - 2* top ");
    };
    
    String curState;
    OOComponentSlider * volumeSlider;
    OOComponentButton * recButton,*playButton;
    
    OORMSComponent * rmsComponent;
    Component * serverComponent;

    
    

    
    
    class TrackStateComponent:public Component,ChangeListener{
        public :
        TrackStateComponent(OOTrack &t):track(&t){
            t.trackState->addChangeListener(this);
            labelTrackNum = new Label("trackNum",String(*t.trackNum));
            addAndMakeVisible(labelTrackNum);
            labelGroupID = new Label("trackID",String(*t.groupID));
            addAndMakeVisible(labelGroupID);
            
            labelGroupID->setBounds("0, 0, left + parent.width/2, top + 50");
            labelTrackNum->setBounds("parent.width/2, 0, left + parent.width/2, top + 50");
        }
        ~TrackStateComponent(){
            deleteAllChildren();
            track->trackState->removeChangeListener(this);
        }
        
        
        Label * labelTrackNum;
        Label * labelGroupID;
        
        
        OOTrack * track;

        void paint(Graphics & g) override{
            switch(track->trackState->state){
                case OOTrack::TrackState::stopped:
                    g.setColour(Colours::lightgrey);break;
                case OOTrack::TrackState::shouldPlay:
                    g.setColour(Colours::lightgreen);break;
                case OOTrack::TrackState::isPlaying:
                    g.setColour(Colours::green);break;
                case OOTrack::TrackState::shouldRecord:
                    g.setColour(Colours::hotpink);break;
                case OOTrack::TrackState::isRecording:
                    g.setColour(Colours::red);break;
                case OOTrack::TrackState::overdub:
                    g.setColour(Colours::orange);break;
                default:
                    g.setColour(Colours::turquoise);break;
                    
            }
            g.fillAll();
        }
        
        
        void  changeListenerCallback (ChangeBroadcaster* source)override{
            
//            if(processor->getParameters()[parameterIndex] == track->groupID){
//                labelGroupID->setText(String(*track->groupID),dontSendNotification);
//            }
//            else if(processor->getParameters()[parameterIndex] == track->trackNum){
//                labelTrackNum->setText(String(*track->trackNum),dontSendNotification);
//            }
            
        
            if(isVisible())repaint();
        };
        
    };
    
    TrackStateComponent *  trackStateComponent;

    void audioProcessorParameterChanged (AudioProcessor* processor,
                                         int parameterIndex,
                                         float newValue) override{}
    void audioProcessorChanged (AudioProcessor* processor) override{};

    
    OOTrack* track;
    
    //    WeakReference<OOTrack> owner;
    
};


#endif  // OOTRACKEDITOR_H_INCLUDED
