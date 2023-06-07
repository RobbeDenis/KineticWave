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

		void SetVolume(float value);
		void SetDry(float value);
		float GetDry() const;
		float GetMin() const;
		float GetMax() const;
		float GetVolume() const;

	private:
		float m_MasterVolume;
		float m_Wet;
		float m_Dry;
		float m_Max;
		float m_Min;
	};
}