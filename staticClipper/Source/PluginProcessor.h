/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Average.h"
#include "FbFilter.h"


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
    //Parameter�̗p��[1]
    enum Parameters {
        Drive = 0,
        OutVol = 1,
        LsfGain = 2,
        LpfFreq = 3,
        LpfFeedback = 4,
        TotalNumParams = 5
    };

    AudioParameterFloat* parameters[Parameters::TotalNumParams];

    //�c�ݗp�̊֐�[2]
    static float clipping(float in) {
        auto threshold = std::tanhf(in);
        auto out = in;

        if (abs(in) > abs(threshold)) { out = threshold; }

        return out;
    };

    Atomic<float> leftReductionPercent, rightReductionPercent;

private:
    //Filter���}���`�`�����l���d�l�ɂ���[10]
    using StLsf = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    using StLpf = dsp::ProcessorDuplicator<FbLpf<float>, LpfState<float>>;

    //�M���������s��DSP module�N���X�̃C���X�^���X��p��[3]
    dsp::WaveShaper<float> clipper;
    dsp::Gain<float> drive, outVol;
    StLsf lsf;
    StLpf lpf;

    //[3]�Ő錾�����C���X�^���X�̏������ɕK�v�ȏ���ێ�����\����[4]
    dsp::ProcessSpec spec;

    //==============================================================================
    Average<float> preAverage;
    Average<float> postAverage;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticClipperVstAudioProcessor)
};
