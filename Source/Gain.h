#pragma once
#include <JuceHeader.h>

namespace effects
{
	class Gain
	{
	public:
		Gain();
		~Gain();

		void processBlock(const juce::AudioSourceChannelInfo& info, float scalar);
		void SetWetness(float value);

	private:
		float m_Wetness;
	};
}