/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class StaticClipperVstAudioProcessorEditor  
    :public AudioProcessorEditor, 
    private Slider::Listener,
    private Timer
{
public:
    StaticClipperVstAudioProcessorEditor (StaticClipperVstAudioProcessor&);
    ~StaticClipperVstAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    using Parameters = StaticClipperVstAudioProcessor::Parameters;

    StaticClipperVstAudioProcessor& processor;

    String parameterName[Parameters::TotalNumParams];
    Label parameterLabels[Parameters::TotalNumParams];
    Slider parameterSliders[Parameters::TotalNumParams];

    Label redutionChannelLabel[2];
    Slider reductionSlider[2];

    void sliderValueChanged(Slider*);

    void timerCallback();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticClipperVstAudioProcessorEditor)
};
