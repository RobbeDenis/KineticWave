#pragma once
#include <JuceHeader.h>

namespace effects
{
	class THD // Not actually Harmonic distortion
	{
	public:
		THD();
		~THD();

		void processBlock(const juce::AudioSourceChannelInfo& info);

		void SetAmount(float amount);
		void SetMax(float max);

		float GetMax() const { return m_CurrentDistortionAmount; }
		float GetTotalMax() const { return m_MaxDistortion; }
		float GetAmount() const { return m_DistortionAmount; }


	private:
		float CalculateDistortedSample(float sample);
		float LPF(float prevSample, float sample);

		float m_DistortionAmount;
		float m_CurrentDistortionAmount;
		float m_MaxDistortion;
	};
}