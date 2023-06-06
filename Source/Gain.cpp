#include "Gain.h"

effects::Gain::Gain()
    : m_Wetness{ 1.f }
{
}

effects::Gain::~Gain()
{
}

void effects::Gain::processBlock(const juce::AudioSourceChannelInfo& info, float scalar)
{
    juce::ScopedNoDenormals noDenormals;
    int channels = info.buffer->getNumChannels();

    for (int channel = 0; channel < channels; ++channel)
    {
        auto data = info.buffer->getWritePointer(channel);

        for (int i{}; i < info.buffer->getNumSamples(); ++i)
        {
            float sample{ data[i] };
            data[i] = std::clamp(scalar, 0.f, 1.f) * sample * m_Wetness;
        }
    }
}

void effects::Gain::SetWetness(float value)
{
    m_Wetness = std::clamp(value, 0.f, 1.f);
}
