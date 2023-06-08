#include "Phaser.h"
#include <random>

effects::Phaser::Phaser()
	: m_Rate{ 3.f }
    , m_MaxRate{ 10.f }
	, m_Depth{ 0.5f }
	, m_Phase{ 0.f }
	, m_SampleRate{ 44100.f }
    , m_MinDelay{ 0.3f }
    , m_MaxDelay{ 1.f }
    , m_Voices{ 3 }
    , m_Delays{ }
    , m_CurrentMaxRate{ 0.f }
{
    m_CurrentMaxRate = m_Rate;
}

effects::Phaser::~Phaser()
{
}

void effects::Phaser::initVoices()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> delayDist(m_MinDelay, m_MaxDelay);

    m_Delays.resize(m_Voices);
    m_DelayedVoices.resize(m_Voices);

    for (int voice = 0; voice < m_Voices; ++voice)
    {
        m_Delays[voice] = delayDist(gen) * m_SampleRate;

        for (auto& delayQueue : m_DelayedVoices)
        {
            while (delayQueue.size() > m_Delays[voice])
                delayQueue.pop();

            while (delayQueue.size() < m_Delays[voice])
                delayQueue.push(0.);
        }
    }
}

void effects::Phaser::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	m_SampleRate = sampleRate;
}

void effects::Phaser::processBlock(const juce::AudioSourceChannelInfo& info)
{
    juce::ScopedNoDenormals noDenormals;
    int channels = info.buffer->getNumChannels();
    const float pi{ juce::MathConstants<float>::pi };

    for (int channel = 0; channel < channels; ++channel)
    {
        auto data = info.buffer->getWritePointer(channel);

        for (int i = 0; i < info.buffer->getNumSamples(); ++i)
        {
            const float sample{ data[i] };
            float extraVoices{};

            float baseVoice = sample * std::sin(m_Phase);
            baseVoice *= m_Depth;

            for (int voice{}; voice < m_Voices; ++voice)
            {
                m_DelayedVoices[voice].push(sample);

                float voiceSample{ m_DelayedVoices[voice].front() };
                voiceSample = sample * std::sin(m_Phase + m_Delays[voice]);
                voiceSample *= m_Depth;
                extraVoices += voiceSample;

                m_DelayedVoices[voice].pop();
            }

            const float phaseIncrement = 2.0f * pi * m_Rate / m_SampleRate;
            m_Phase += phaseIncrement;
            m_Phase = fmod(m_Phase, 2.0f * pi);

            data[i] = baseVoice + extraVoices;
        }
    }
}

void effects::Phaser::SetDepth(float depth)
{
    m_Depth = std::clamp(depth, 0.f, 1.f);
}

void effects::Phaser::SetRate(float rate)
{
    m_Rate = std::clamp(rate, 0.f, m_MaxRate);
}

void effects::Phaser::SetMax(float max)
{
    m_CurrentMaxRate = std::clamp(max, 0.f, m_MaxRate);
}
