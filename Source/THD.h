#pragma once
#include <JuceHeader.h>

namespace effects
{
	class THD // Not actually Harmonic distortion i like this better
	{
	public:
		THD();
		~THD();

		void processBlock(const juce::AudioSourceChannelInfo& info);

		void SetAmount(float amount);
		void SetMax(float max);

		float GetMax() const { return m_CurrentDistortionAmount; }


	private:
		float m_DistortionAmount;
		float m_CurrentDistortionAmount;
		float m_MaxDistortion;
	};
}