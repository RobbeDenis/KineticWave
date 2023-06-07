#include "Gain.h"

effects::Gain::Gain()
    : m_Wet{ 1.f }
    , m_Dry{ 0.f }
    , m_Min{ 0.f }
    , m_Max{ 1.5f }
    , m_MasterVolume{ 1.f }
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
            data[i] = (std::clamp(scalar, 0.f, 1.f) * sample * m_Wet + sample * m_Dry) * m_MasterVolume;
        }
    }
}

void effects::Gain::SetVolume(float value)
{
    m_MasterVolume = std::clamp(value, m_Min, m_Max);
}

void effects::Gain::SetDry(float value)
{
    m_Dry = std::clamp(value, m_Min, m_Max);
    m_Wet = std::clamp(m_Max - m_Dry, m_Min, m_Max);
}

float effects::Gain::GetDry() const
{
    return m_Dry;
}

float effects::Gain::GetMin() const
{
    return m_Min;
}

float effects::Gain::GetMax() const
{
    return m_Max;
}

float effects::Gain::GetVolume() const
{
    return m_MasterVolume;
}
