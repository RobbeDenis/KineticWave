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

    void updateEffects();
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
    juce::ToggleButton m_TogglePRS1;
    juce::ToggleButton m_TogglePRS2;
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
    bool m_PRS1;
    bool m_PRS2;

    // Phaser
    effects::Phaser m_Phaser;
    juce::Slider m_PhaserRate;
    juce::Label m_PhaserRateLabel;
    juce::Slider m_Depth;
    juce::Label m_DepthLabel;
    bool m_P1;
    bool m_P2;

    // THD
    effects::THD m_THD;
    juce::ToggleButton m_EnableTresholdTHD;
    juce::ToggleButton m_ToggleD1;
    juce::ToggleButton m_ToggleD2;
    juce::Label m_TresholdTHD;
    juce::Slider m_THDAmount;
    juce::Label m_THDLabel;
    bool m_UseTresholdTHD;
    bool m_D1;
    bool m_D2;

    // Gain
    effects::Gain m_Gain;
    juce::Slider m_GainDry;
    juce::Label m_GainDryLabel;
    juce::Slider m_GainVolume;
    juce::Slider m_GainVolume2;
    juce::Label m_GainVolumeLabel;
    juce::Label m_GainVolumeLabel2;

    // Sample
    juce::AudioFormatManager m_FormatManager;
    std::unique_ptr <juce::ResamplingAudioSource> m_pResamplingSource;
    std::unique_ptr <juce::ResamplingAudioSource> m_pResamplingSource2;
    std::unique_ptr<juce::AudioFormatReaderSource> m_pReaderSource;
    std::unique_ptr<juce::AudioFormatReaderSource> m_pReaderSource2;

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
    float m_NormalizationFactor = 0.5f;
    float m_SampleRate;
    int m_SamplesPerBlock = 0.f;
    float m_SmoothingFactor = 0;
    int m_IChannels;
    int m_OChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
