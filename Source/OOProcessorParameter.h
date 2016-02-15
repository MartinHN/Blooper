/*
  ==============================================================================

    OOParameter.h
    Created: 10 Feb 2016 7:36:58pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef OOPARAMETER_H_INCLUDED
#define OOPARAMETER_H_INCLUDED

#include "JuceHeader.h"


/*
 Old class not use ATM
 */
class OOProcessorParameter : public AudioProcessorParameter,public Value {
public:
    OOProcessorParameter() :hasLabel(false){
        Value::setValue(getDefaultValue());
    }
    
    OOProcessorParameter(String _name):name(_name),hasLabel(false){Value::setValue(getDefaultValue());}
    OOProcessorParameter(String _name,String _label):name(_name),label(_label),hasLabel(true){Value::setValue(getDefaultValue());}
    virtual ~OOProcessorParameter() {}

    
    float getValue() const override { return Value::getValue(); }
    void setValue(float newValue) override {Value::setValue(newValue);}
    
    float getDefaultValue() const override {return 0.5f; }
    String getName(int maximumStringLength) const override {return name;};
    String getLabel() const override {return hasLabel?label:name;};
    float getValueForText(const String& text) const override {return Value::getValue();}
    
    
    String name;
    String label;
    bool hasLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OOProcessorParameter);
    
};



#endif  // OOPARAMETER_H_INCLUDED
