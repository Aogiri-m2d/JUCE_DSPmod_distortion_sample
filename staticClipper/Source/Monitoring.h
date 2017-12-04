#pragma once

#include "JuceHeader.h"

template <typename FloatType>
class ReductionRec {
public:
    ReductionRec() {};

    void set(FloatType durationSec) {
        duration = int(samplerate * durationSec);
    };

    void prepare(const dsp::ProcessSpec& spec) noexcept {
        samplerate = spec.sampleRate;
        reset();
    };

    void reset() { 
        count = 0; 
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                sum[i][j] = FloatType();
                avg[i][j] = FloatType();
            }
        }

        isPreRecReady = true;
        isPostRecReady = false;
        isFilled = false;
    };

    template <typename ProcessContext>
    void preProcess(const ProcessContext& context) noexcept {
        if (!isPreRecReady && isFilled) { return; }

        auto&& inBlock = context.getInputBlock();

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();

        for (auto channel = 0; channel < numChannels; channel++) {
            auto* src = inBlock.getChannelPointer(channel);

            for (auto i = 0; i < len; i++) {
                sum[0][channel] += abs(src[i]);
            }
        }

        count += len;

        if (count > duration) { 
            isFilled = true;
            avg[0][0] = sum[0][0] / FloatType(duration);
            avg[0][1] = sum[0][1] / FloatType(duration);
        }

        isPostRecReady = true;
        isPreRecReady = false;
    };

    template <typename ProcessContext>
    void postProcess(const ProcessContext& context) noexcept {
        if (!isPostRecReady) { return; }

        auto&& inBlock = context.getInputBlock();

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();

        for (auto channel = 0; channel < numChannels; channel++) {
            auto* src = inBlock.getChannelPointer(channel);

            for (auto i = 0; i < len; i++) {
                sum[1][channel] += abs(src[i]);
            }
        }

        if (isFilled) {
            avg[1][0] = sum[1][0] / FloatType(duration);
            avg[1][1] = sum[1][1] / FloatType(duration);
        }

        isPostRecReady = false;
        isPreRecReady = true;
    };

    std::pair<FloatType, FloatType> getReductiondB() {
        FloatType res[2];

        for (int i = 0; i < 2; i++) {
            if (avg[0][i] == FloatType(0)) { res[i] = FloatType(1); }
            else { res[i] = avg[1][i] / avg[0][i]; }

            res[i] = Decibels::gainToDecibels(res[i]);
        }

        reset();

        return std::pair<FloatType, FloatType> { res[0], res[1] };
    };

    bool canGetAvg() { return isFilled; };

private:
    double samplerate;
    int duration;
    int count;
    bool isPreRecReady = true;
    bool isPostRecReady = false;
    bool isFilled = false;
    FloatType sum[2][2];
    FloatType avg[2][2];
};