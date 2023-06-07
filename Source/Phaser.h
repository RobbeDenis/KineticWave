#pragma once
#include <JuceHeader.h>

namespace effects
{
	class Phaser
	{
	public:
		Phaser();
		~Phaser();

		void initVoices();

		void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
		void processBlock(const juce::AudioSourceChannelInfo& bufferToFill);

		void SetDepth(float depth);
		void SetRate(float rate);
		void SetMax(float max);

		float GetMaxRate() const { return m_CurrentMaxRate; }
		float GetTotalMax() const { return m_MaxRate; }
		float GetDepth() const { return m_Depth; }

	private:
		float m_Rate;
		float m_MaxRate;
		float m_CurrentMaxRate;
		float m_Depth;
		float m_Phase;
		float m_SampleRate;
		int m_Voices;
		float m_MinDelay, m_MaxDelay;
		std::vector<int> m_Delays;
		std::vector<std::queue<float>> m_DelayedVoices;
	};
}