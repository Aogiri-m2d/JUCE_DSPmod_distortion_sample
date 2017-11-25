#pragma once

#include <vector>
#include "JuceHeader.h"

template <typename FloatType>
struct FbFiltParam: public dsp::ProcessorState {
    FbFiltParam()
        :coefficients(new dsp::IIR::Coefficients<FloatType>())
    {};

    using Ptr = ReferenceCountedObjectPtr<FbFiltParam<FloatType>>;

    void set(double fs, FloatType cutoff, FloatType feedback) {
        *coefficients = *(dsp::IIR::Coefficients<FloatType>::makeFirstOrderLowPass(fs, cutoff));
        this->feedback = feedback;
    }

    FloatType feedback;
    typename dsp::IIR::Coefficients<FloatType>::Ptr coefficients;
};

template <typename FloatType>
class FbFilter {
public:
    FbFilter(FbFiltParam<FloatType>* ptr)
        :paramPtr(ptr), feedbackSig()
    {
        for (int i = 0; i < 4; i++) { filters.push_back(dsp::IIR::Filter<FloatType>(ptr->coefficients)); }
    };

    void prepare(const dsp::ProcessSpec& spec) noexcept {
        for (auto& filt : filters) { filt.prepare(spec); }
    };

    void reset() {
        for (auto& filt : filters) { filt.reset(); }
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
    typename FbFiltParam<FloatType>::Ptr paramPtr;
    std::vector<dsp::IIR::Filter<FloatType>> filters;

    FloatType feedbackSig;
};