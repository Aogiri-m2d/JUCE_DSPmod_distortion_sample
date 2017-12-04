#pragma once

#include <vector>
#include "JuceHeader.h"

//RefarenceCountedObjectPtr(RCOP)���p�����Ă���ProcessorState���p��[11]
template <typename FloatType>
struct LpfState: public dsp::ProcessorState {
    LpfState()
        :coefficients(new dsp::IIR::Coefficients<FloatType>())
    {};

    //ProcessorDuplicator�́Cstate class��new���āC����Ptr��ێ�����[12]
    using Ptr = ReferenceCountedObjectPtr<LpfState<FloatType>>;

    void set(double fs, FloatType cutoff, FloatType feedback) {
        *coefficients = *dsp::IIR::Coefficients<FloatType>::makeFirstOrderLowPass(fs, cutoff);
        this->feedback = feedback;
    }

    //����LPF processor�̃p�����[�^[13]
    FloatType feedback;
    typename dsp::IIR::Coefficients<FloatType>::Ptr coefficients;
};

template <typename FloatType>
class FbLpf {
public:
    //�R���X�g���N�^��LpfState�̃|�C���^���󂯎��C�����ɕێ�����[14]
    FbLpf(LpfState<FloatType>* ptr)
        :paramPtr(ptr), feedbackSig()
    {
        //4��1���nLPF��p�ӁD�����̐�������E����Coeffients��LpfState�ŊǗ����Ă���[15]
        for (int i = 0; i < 4; i++) { filters.push_back(dsp::IIR::Filter<FloatType>(ptr->coefficients)); }
    };

    //prepare, reset�͊e1���nLPF�ɂ��č�p������[16]
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

        //1 sample����4��1���nLPF�ŘA���I�ɏ������C���̏o�͂�1�x�������ē��͂փt�B�[�h�o�b�N[17]
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