#pragma once

#include <vector>
#include "JuceHeader.h"

template <typename FloatType>
class Average {
public:
    Average() = default;

    void prepare(const dsp::ProcessSpec& spec) noexcept {
        samplerate = spec.sampleRate;
        averages = std::vector<FloatType>(spec.numChannels, FloatType());
        sumSignal = std::vector<FloatType>(spec.numChannels, FloatType());

        reset();
    };

    void reset() noexcept {
        elapse = 0;

        for (auto i = 0; i < averages.size(); i++) {
            averages[i] = FloatType();
            sumSignal[i] = FloatType();
        }
    };

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {
        auto&& inBlock = context.getInputBlock();

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();

        for (auto channel = 0; channel < numChannels; channel++) {
            auto* src = inBlock.getChannelPointer(channel);

            for (auto i = 0; i < len; i++) { sumSignal[channel] += abs(src[i]); }
        }

        elapse += static_cast<int>(len);

        if (elapse >= updatePeriod) {
            for (auto i = 0; i < averages.size(); i++) {
                averages[i] = sumSignal[i] / static_cast<FloatType>(elapse);
                sumSignal[i] = FloatType();
            }

            elapse = 0;
        }
    };

    void set(FloatType durationSec) {
        auto newPeriod = static_cast<int>(samplerate * durationSec);

        if (updatePeriod != newPeriod) {
            updatePeriod = newPeriod;
            reset();
        }
    };

    FloatType get(int channel) {
        jassert(channel >= 0 && channel < averages.size());

        return averages[channel];
    };

private:
    double samplerate;
    int updatePeriod;
    int elapse;
    std::vector<FloatType> averages;
    std::vector<FloatType> sumSignal;
};