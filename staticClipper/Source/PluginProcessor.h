/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FbFilter.h"
#include "Monitoring.h"


//==============================================================================
/**
*/
class StaticClipperVstAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    StaticClipperVstAudioProcessor();
    ~StaticClipperVstAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
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

    //==============================================================================
    enum Parameters {
        Drive = 0,
        OutVol = 1,
        LsfGain = 2,
        LpfFreq = 3,
        LpfFeedback = 4,
        TotalNumParams = 5
    };

    AudioParameterFloat* parameters[Parameters::TotalNumParams];

    //==============================================================================
    static float clipping(float in) {
        auto threshold = std::tanhf(in);
        auto out = in;

        if (abs(in) > abs(threshold)) { out = threshold; }

        return out;
    };

    Atomic<float> leftReductiondB, rightReductiondB;

private:
    using StLsf = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    using StLpf = dsp::ProcessorDuplicator<FbFilter<float>, FbFiltParam<float>>;

    dsp::WaveShaper<float> clipper;
    dsp::Gain<float> drive, outVol;
    StLsf lsf;
    StLpf lpf;

    dsp::ProcessSpec spec;

    ReductionRec<float> reductionRec;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticClipperVstAudioProcessor)
};
