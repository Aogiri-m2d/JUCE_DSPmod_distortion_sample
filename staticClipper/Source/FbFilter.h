#pragma once

#include <vector>
#include "JuceHeader.h"

//RefarenceCountedObjectPtr(RCOP)を継承しているProcessorStateを継承[11]
template <typename FloatType>
struct LpfState: public dsp::ProcessorState {
    LpfState()
        :coefficients(new dsp::IIR::Coefficients<FloatType>())
    {};

    //ProcessorDuplicatorは，state classをnewして，そのPtrを保持する[12]
    using Ptr = ReferenceCountedObjectPtr<LpfState<FloatType>>;

    void set(double fs, FloatType cutoff, FloatType feedback) {
        *coefficients = *dsp::IIR::Coefficients<FloatType>::makeFirstOrderLowPass(fs, cutoff);
        this->feedback = feedback;
    }

    //自作LPF processorのパラメータ[13]
    FloatType feedback;
    typename dsp::IIR::Coefficients<FloatType>::Ptr coefficients;
};

template <typename FloatType>
class FbLpf {
public:
    //コンストラクタでLpfStateのポインタを受け取り，内部に保持する[14]
    FbLpf(LpfState<FloatType>* ptr)
        :paramPtr(ptr), feedbackSig()
    {
        //4つの1次系LPFを用意．これらの性状を左右するCoeffientsはLpfStateで管理している[15]
        for (int i = 0; i < 4; i++) { filters.push_back(dsp::IIR::Filter<FloatType>(ptr->coefficients)); }
    };

    //prepare, resetは各1次系LPFについて作用させる[16]
    void prepare(const dsp::ProcessSpec& spec) noexcept {
        for (auto& filter : filters) { filter.prepare(spec); }
    };

    void reset() {
        for (auto& filter : filters) { filter.reset(); }
    };

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {
        auto&& inputBlock = context.getInputBlock();
        auto&& outputBlock = context.getOutputBlock();

        jassert(inputBlock.getNumChannels() == 1);
        jassert(outputBlock.getNumChannels() == 1);
        
        auto numSamples = inputBlock.getNumSamples();
        auto* src = inputBlock.getChannelPointer(0);
        auto* dst = outputBlock.getChannelPointer(0);
        auto feedbackAmt = paramPtr->feedback;

        //1 sampleずつ4つの1次系LPFで連続的に処理し，その出力を1遅延させて入力へフィードバック[17]
        for (auto i = 0; i < numSamples; i++) {
            auto data = src[i] - feedbackSig;

            for (auto& filter : filters) {
                data = filter.processSample(data);
            }

            feedbackSig = data * feedbackAmt;
            dst[i] = data;
        }       

        for (auto& filter : filters) { filter.snapToZero(); }
    };

private:
    typename LpfState<FloatType>::Ptr paramPtr;
    std::vector<dsp::IIR::Filter<FloatType>> filters;

    FloatType feedbackSig;
};