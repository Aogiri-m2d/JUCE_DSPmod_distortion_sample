/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
StaticClipperVstAudioProcessorEditor::StaticClipperVstAudioProcessorEditor (StaticClipperVstAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (420, 250);

    parameterName[Parameters::Drive] = String("Drive");
    parameterName[Parameters::OutVol] = String("OutVol");
    parameterName[Parameters::LsfGain] = String("LSF Gain");
    parameterName[Parameters::LpfFreq] = String("LPF Freq");
    parameterName[Parameters::LpfFeedback] = String("LPF Feedback");

    parameterSliders[Parameters::Drive].setTextValueSuffix(" dB");
    parameterSliders[Parameters::OutVol].setTextValueSuffix(" dB");
    parameterSliders[Parameters::LsfGain].setTextValueSuffix(" dB");
    parameterSliders[Parameters::LpfFreq].setTextValueSuffix(" Hz");


    for (int i = 0; i < Parameters::TotalNumParams; i++) {
        parameterLabels[i].setText(parameterName[i], dontSendNotification);
        parameterLabels[i].setFont(Font(13.f, Font::FontStyleFlags::plain));
        parameterLabels[i].setJustificationType(Justification::centred);

        parameterSliders[i].setSliderStyle(Slider::SliderStyle::Rotary);
        parameterSliders[i].setRange(processor.parameters[i]->range.start, processor.parameters[i]->range.end);
        parameterSliders[i].setValue(processor.parameters[i]->get(), dontSendNotification);
        parameterSliders[i].addListener(this);

        addAndMakeVisible(&parameterLabels[i]);
        addAndMakeVisible(&parameterSliders[i]);
    }

    //============================================================================================
    redutionChannelLabel[0].setText("Left GR", dontSendNotification);
    redutionChannelLabel[1].setText("Right GR", dontSendNotification);
    for (int i = 0; i < 2; i++) {
        reductionSlider[i].setSliderStyle(Slider::SliderStyle::LinearBar);
        reductionSlider[i].setRange(0, 20);
        reductionSlider[i].setTextValueSuffix(" dB");
    }

    for (int i = 0; i < 2; i++) {
        addAndMakeVisible(&redutionChannelLabel[i]);
        addAndMakeVisible(&reductionSlider[i]);
    }

    //==============================================================================================
    startTimer(30);
}

StaticClipperVstAudioProcessorEditor::~StaticClipperVstAudioProcessorEditor()
{
}

//==============================================================================
void StaticClipperVstAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    //g.setColour (Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void StaticClipperVstAudioProcessorEditor::resized()
{
    int initX = 10;
    int initY = 10;
    int labelWidth = 80;
    int labelHeight = 20;
    int sliderWidth = labelWidth;
    int sliderHeight = sliderWidth + labelHeight;

    int x = initX, y = initY;

    parameterLabels[Parameters::Drive].setBounds(x, y, labelWidth, labelHeight);
    parameterLabels[Parameters::LsfGain].setBounds(x += labelWidth, y, labelWidth, labelHeight);
    parameterLabels[Parameters::LpfFreq].setBounds(x += labelWidth, y, labelWidth, labelHeight);
    parameterLabels[Parameters::LpfFeedback].setBounds(x += labelWidth, y, labelWidth, labelHeight);
    parameterLabels[Parameters::OutVol].setBounds(x += labelWidth, y, labelWidth, labelHeight);

    x = initX; y += labelHeight;

    for (int i = 0; i < Parameters::TotalNumParams; i++) {
        parameterSliders[i].setTextBoxStyle(Slider::TextBoxBelow, true, labelWidth * 0.9f, labelHeight);
    }

    parameterSliders[Parameters::Drive].setBounds(x, y, sliderWidth, sliderHeight);
    parameterSliders[Parameters::LsfGain].setBounds(x += sliderWidth, y, sliderWidth, sliderHeight);
    parameterSliders[Parameters::LpfFreq].setBounds(x += sliderWidth, y, sliderWidth, sliderHeight);
    parameterSliders[Parameters::LpfFeedback].setBounds(x += sliderWidth, y, sliderWidth, sliderHeight);
    parameterSliders[Parameters::OutVol].setBounds(x += sliderWidth, y, sliderWidth, sliderHeight);

    x = initX; y += sliderHeight;

    //=======================================================================================================
    y += 20;

    for (int i = 0; i < 2; i++) {
        redutionChannelLabel[i].setBounds(x, y, labelWidth, labelHeight);
        reductionSlider[i].setBounds(x + labelWidth, y, getWidth() - labelWidth - 20, labelHeight);
        y += labelHeight;
    }
}

void StaticClipperVstAudioProcessorEditor::sliderValueChanged(Slider* s) {
    for (int i = 0; i < Parameters::TotalNumParams; i++) {
        if (s == &parameterSliders[i]) {
            float rawVal = (float)parameterSliders[i].getValue();
            float newVal = processor.parameters[i]->range.convertTo0to1(rawVal);
            processor.parameters[i]->setValueNotifyingHost(newVal);
            break;
        }
    }
}

void StaticClipperVstAudioProcessorEditor::timerCallback() {
    for (int i = 0; i < Parameters::TotalNumParams; i++) {
        float newVal = processor.parameters[i]->get();
        parameterSliders[i].setValue(newVal, dontSendNotification);
    }

    reductionSlider[0].setValue(-processor.leftReductiondB.get(), dontSendNotification);
    reductionSlider[1].setValue(-processor.rightReductiondB.get(), dontSendNotification);
}