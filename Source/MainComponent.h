#pragma once

#include <JuceHeader.h>
#include "KSkeletonTracker.h"

class MainComponent final : public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void initInputChannels();
    void initKinectInfoUI();
    void initKinect();
    void renderKinectTracking();
    void enableUpdate(bool enable);
    void setUpdateInterval(int intervalMs);
    void timerCallback() override;

    float map(float value, float min, float max, float toMin, float toMax);

    // Kinect
    std::unique_ptr<KSkeletonTracker> m_pTracker;
    Vector4 m_LeftWrist;
    Vector4 m_RightWrist;
    float m_UpdateIntervalMs;
    bool m_EnableUpdate;

    // Kinect info UI
    Vector4 m_InfoPos;
    juce::Label m_ConnectedLabel;
    juce::Label m_InfoLX;
    juce::Label m_InfoLY;
    juce::Label m_InfoRX;
    juce::Label m_InfoRY;

    // Kinect tracking UI
    Vector4 m_SquarePos;
    juce::Label m_LeftHand;
    juce::Label m_RightHand;
    int m_SquareSpacing;
    int m_SquareWidth;
    int m_CircleRadius;
    int m_FontSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
