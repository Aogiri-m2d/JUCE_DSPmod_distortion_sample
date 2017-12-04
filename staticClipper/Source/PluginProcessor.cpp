/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
//�R���X�g���N�^[5]
StaticClipperVstAudioProcessor::StaticClipperVstAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters{ 
        new AudioParameterFloat("DRV", "Drive", 0.f, 30.f, 12.f),
        new AudioParameterFloat("OV", "OutVol", -36.f, 6.f, -12.f),
        new AudioParameterFloat("LSG", "Pre-LSF Gain [dB]", -12.f, 12.f, 0.f),
        new AudioParameterFloat("LPC", "Post-LPF Cutoff [Hz]", 200.f, 20000.f, 15000.f),
        new AudioParameterFloat("LPFD", "Post-LPF Feedback", 0.f, 1.f, 0.f)
    },
    clipper(),
    drive(),
    outVol(),
    lsf(),
    lpf()
{ 
    for (int i = 0; i < Parameters::TotalNumParams; i++) {
        addParameter(parameters[i]);
    }
}

StaticClipperVstAudioProcessor::~StaticClipperVstAudioProcessor()
{
}

//==============================================================================
const String StaticClipperVstAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StaticClipperVstAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StaticClipperVstAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StaticClipperVstAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StaticClipperVstAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StaticClipperVstAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StaticClipperVstAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StaticClipperVstAudioProcessor::setCurrentProgram (int index)
{
}

const String StaticClipperVstAudioProcessor::getProgramName (int index)
{
    return {};
}

void StaticClipperVstAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
//processor�̏�����[6]
void StaticClipperVstAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    spec.maximumBlockSize = samplesPerBlock;

    clipper.prepare(spec);
    drive.prepare(spec);
    outVol.prepare(spec);
    lsf.prepare(spec);
    lpf.prepare(spec);
    preAverage.prepare(spec);
    postAverage.prepare(spec);
}

void StaticClipperVstAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StaticClipperVstAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StaticClipperVstAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannel = getTotalNumInputChannels();
    auto totalNumOutputChannel = getTotalNumOutputChannels();
    auto bufferSize = buffer.getNumSamples();

    for (int i = totalNumInputChannel; i < totalNumOutputChannel; i++) {
        buffer.clear(i, 0, bufferSize);
    }

    //�eprocessor�̃p�����[�^�i�U�镑���j��ݒ�[7]
    clipper.functionToUse = clipping;
    drive.setGainDecibels(parameters[Parameters::Drive]->get());
    outVol.setGainDecibels(parameters[Parameters::OutVol]->get());
    *lsf.state = *dsp::IIR::Coefficients<float>::makeLowShelf(spec.sampleRate, 300.f, 0.5f, Decibels::decibelsToGain(parameters[Parameters::LsfGain]->get()));
    lpf.state->set(spec.sampleRate, parameters[Parameters::LpfFreq]->get(), parameters[Parameters::LpfFeedback]->get());
    preAverage.set(0.05f);
    postAverage.set(0.05f);

    //���͐M���o�b�t�@�����b�v[8]
    dsp::AudioBlock<float> audioBlock(buffer);
    dsp::ProcessContextReplacing<float> context(audioBlock);

    //processor�ɂ��M������[9]
    lsf.process(context);
    drive.process(context);
    preAverage.process(context);
    clipper.process(context);
    postAverage.process(context);
    lpf.process(context);
    outVol.process(context);

    //==========================================================================
    auto leftReduction = (preAverage.get(0) - postAverage.get(0)) / preAverage.get(0);
    auto rightReduction = (preAverage.get(1) - postAverage.get(1)) / preAverage.get(1);

    leftReductionPercent.set(leftReduction * 100.f);
    rightReductionPercent.set(rightReduction * 100.f);
}

//==============================================================================
bool StaticClipperVstAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StaticClipperVstAudioProcessor::createEditor()
{
    return new StaticClipperVstAudioProcessorEditor (*this);
}

//==============================================================================
void StaticClipperVstAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StaticClipperVstAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StaticClipperVstAudioProcessor();
}
