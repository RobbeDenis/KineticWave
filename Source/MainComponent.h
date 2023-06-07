#pragma once

#include <JuceHeader.h>
#include "KSkeletonTracker.h"
#include "Gain.h"
#include "THD.h"
#include "Phaser.h"

class MainComponent final : public juce::AudioAppComponent, private juce::Timer
{
    enum TransportState
    {
        Loading,
        Stopped,
        Playing
    };

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
    void initLoadUI();
    void initKinectTrackingUI();
    void initKinect();
    void initGainSettingsUI();
    void initPlayrateSettingsUI();
    void initTHDSettingsUI();
    void initPhaserSettingsUI();

    void renderKinectTracking();
    void enableUpdate(bool enable);
    void setUpdateInterval(int intervalMs);
    void timerCallback() override;
    void setPlayrate(float rate);
    void setMinPR(float rate);
    void setMaxPR(float rate);

    void loadFile(bool second);
    void changeState(TransportState newState);

    float map(float value, float min, float max, float toMin, float toMax);
    void smooth(float &input, float target);

    // Settings UI
    int m_SettingsSpacing;
    int m_SettingsY;
    float m_MinTreshHold;
    float m_MaxTreshHold;

    // Playrate
    juce::ToggleButton m_EnableTreshold;
    juce::Label m_Treshold;
    juce::Slider m_PRMaxSlider;
    juce::Label m_PRMaxLabel;
    juce::Slider m_PRMinSlider;
    juce::Label m_PRMinLabel;
    float m_PRDry;
    float m_Playrate;
    float m_CurrentMinPR;
    float m_CurrentMaxPR;
    float m_MinPR;
    float m_MaxPR;
    bool m_UseTreshold;

    // Phaser
    effects::Phaser m_Phaser;
    juce::Slider m_PhaserRate;
    juce::Label m_PhaserRateLabel;
    juce::Slider m_Depth;
    juce::Label m_DepthLabel;

    // THD
    effects::THD m_THD;
    juce::ToggleButton m_EnableTresholdTHD;
    juce::Label m_TresholdTHD;
    juce::Slider m_THDAmount;
    juce::Label m_THDLabel;
    bool m_UseTresholdTHD;

    // Gain
    effects::Gain m_Gain;
    juce::Slider m_GainDry;
    juce::Slider m_GainVolume;
    juce::Label m_GainDryLabel;
    juce::Label m_GainVolumeLabel;

    // Sample
    juce::AudioFormatManager m_FormatManager;
    std::unique_ptr <juce::ResamplingAudioSource> m_pResamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> m_pReaderSource;

    // Loading sample
    std::unique_ptr<juce::FileChooser> m_Chooser;
    juce::TextButton m_OpenButton;
    juce::TextButton m_OpenButton2;
    juce::TextButton m_PlayButton;
    juce::TextButton m_StopButton;
    TransportState m_State;

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

    // Other
    float m_SmoothingFactor;
    int m_IChannels;
    int m_OChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
