#include "THD.h"

effects::THD::THD()
    : m_DistortionAmount{ 0.f }
    , m_MaxDistortion{ 3.f }
    , m_CurrentDistortionAmount{ 0.f }
{
}

effects::THD::~THD()
{
}

void effects::THD::processBlock(const juce::AudioSourceChannelInfo& info)
{
    juce::ScopedNoDenormals noDenormals;
    int channels = info.buffer->getNumChannels();

    for (int channel = 0; channel < channels; ++channel)
    {
        auto data = info.buffer->getWritePointer(channel);

        for (int sampleIdx{}; sampleIdx < info.buffer->getNumSamples(); ++sampleIdx)
        {
            float sample{ data[sampleIdx] };
            float distortedSample{ 0.f };
            const int numHarmonics{ 10 };
            std::vector<float> harmonics;
            harmonics.resize(numHarmonics);

            for (int i{}; i < numHarmonics; ++i)
            {
                float harmonic = std::sin((i + 1) * sample);
                harmonics[i] = harmonic;
            }

            float totalHarmonics = 0.0f;
            for (int i{}; i < numHarmonics; ++i)
            {
                totalHarmonics += std::abs(harmonics[i]);
            }

            for (int i{}; i < numHarmonics; ++i)
            {
                harmonics[i] /= totalHarmonics;
            }

            for (int i{}; i < numHarmonics; ++i)
            {
                harmonics[i] = std::tanh(harmonics[i] * m_DistortionAmount);

                distortedSample += harmonics[i];
            }

            const float scaledSample{ sample * std::clamp(1.f - (m_DistortionAmount / m_MaxDistortion), 0.f, 1.f) };
            data[sampleIdx] = distortedSample + scaledSample;
        }
    }
}

void effects::THD::SetAmount(float amount)
{
    m_DistortionAmount = std::clamp(amount, 0.f, m_MaxDistortion);
}

void effects::THD::SetMax(float max)
{
    m_CurrentDistortionAmount = std::clamp(max, 0.05f, m_MaxDistortion);
}
