#include "MainComponent.h"

MainComponent::MainComponent()
    : m_LeftWrist{ 0, 0, 0, 0 }
    , m_RightWrist{ 0, 0, 0, 0 }
    , m_UpdateIntervalMs{ 20 }
    , m_EnableUpdate{ false }
    , m_InfoPos{ 10, 0 }
    , m_SquareWidth{ 270 }
    , m_SquareSpacing{ 39 }
    , m_SquarePos{ 200, 20, 0, 0 }
    , m_CircleRadius{ 30 }
    , m_FontSize{ 24 }
    , m_SmoothingFactor{ 0.2f }
    , m_SettingsSpacing{ 20 }
    , m_SettingsY{ 310 }
    , m_Playrate{ 1.f }
    , m_MinPR{ 0.2f }
    , m_MaxPR{ 3.f }
    , m_CurrentMinPR{ 1.f }
    , m_CurrentMaxPR{ 1.f }
    , m_UseTreshold{ true }
    , m_MinTreshHold{ 0.3f }
    , m_MaxTreshHold{ 0.7f }
    , m_UseTresholdTHD{ false }
    , m_IChannels{ 0 }
    , m_OChannels{ 0 }
    , m_PRS1{ true }
    , m_PRS2{ true }
    , m_D1{ false }
    , m_D2{ false }
    , m_P1{ true }
    , m_P2{ true }
    , m_KinectAngle{ static_cast<LONG>(0) }
{
    setSize (800, 600);

    setMaxPR(1.5f);
    setMinPR(0.5f);

    initKinectInfoUI();
    initLoadUI();
    initKinectTrackingUI();
    initGainSettingsUI();
    initPlayrateSettingsUI();
    initTHDSettingsUI();
    initPhaserSettingsUI();

    initKinect();

    enableUpdate(true);
    changeState(Loading);
    setPlayrate(1.f);

    m_FormatManager.registerBasicFormats();
}

void MainComponent::initPlayrateSettingsUI()
{
    const int tbW{ 40 };
    const int tbH{ 20 };

    // Max
    addAndMakeVisible(&m_PRMaxSlider);
    m_PRMaxSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_PRMaxSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_PRMaxSlider.setRange(m_MinPR, m_MaxPR, 0.01f);
    m_PRMaxSlider.setValue(m_CurrentMaxPR, juce::dontSendNotification);
    m_PRMaxSlider.onDragEnd = [this]
    {
        float value = m_PRMaxSlider.getValue();

        if (value < m_MinPR)
            m_PRMinSlider.setValue(value - 0.05f, juce::sendNotification);

        setMaxPR(m_PRMaxSlider.getValue());
    };

    m_PRMaxLabel.setFont(juce::Font(18.f, juce::Font::bold));
    m_PRMaxLabel.setText("Max", juce::dontSendNotification);
    m_PRMaxLabel.setJustificationType(juce::Justification::centredTop);
    m_PRMaxLabel.attachToComponent(&m_PRMaxSlider, false);

    // Min
    addAndMakeVisible(&m_PRMinSlider);
    m_PRMinSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_PRMinSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_PRMinSlider.setRange(m_MinPR, m_MaxPR, 0.01f);
    m_PRMinSlider.setValue(m_CurrentMinPR, juce::sendNotification);
    m_PRMinSlider.onDragEnd = [this]
    {
        float value = m_PRMinSlider.getValue();

        if (value > m_MaxPR)
            m_PRMaxSlider.setValue(value + 0.05f, juce::dontSendNotification);

        setMinPR(m_PRMinSlider.getValue());
    };

    m_PRMinLabel.setFont(juce::Font(18.f, juce::Font::bold));
    m_PRMinLabel.setText("Min", juce::dontSendNotification);
    m_PRMinLabel.setJustificationType(juce::Justification::centredTop);
    m_PRMinLabel.attachToComponent(&m_PRMinSlider, false);

    // Toggle
    addAndMakeVisible(m_EnableTreshold);
    m_EnableTreshold.setToggleState(m_UseTreshold, juce::dontSendNotification);
    m_EnableTreshold.onStateChange = [this]
    {
        m_UseTreshold = m_EnableTreshold.getToggleState();
    };

    addAndMakeVisible(m_Treshold);
    m_Treshold.setFont(juce::Font(18.f, juce::Font::bold));
    m_Treshold.setText("THRESHOLD", juce::dontSendNotification);
    m_Treshold.setJustificationType(juce::Justification::centredTop);

    // Toggle PRS
    addAndMakeVisible(m_TogglePRS1);
    m_TogglePRS1.setToggleState(m_PRS1, juce::dontSendNotification);
    m_TogglePRS1.onStateChange = [this]
    {
        m_PRS1 = m_TogglePRS1.getToggleState();
    };

    addAndMakeVisible(m_TogglePRS2);
    m_TogglePRS2.setToggleState(m_PRS2, juce::dontSendNotification);
    m_TogglePRS2.onStateChange = [this]
    {
        m_PRS2 = m_TogglePRS2.getToggleState();
    };
}

void MainComponent::initPhaserSettingsUI()
{
    const int tbW{ 40 };
    const int tbH{ 20 };

    // Rate 1
    addAndMakeVisible(&m_PhaserRate);
    m_PhaserRate.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_PhaserRate.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_PhaserRate.setRange(0.f, m_Phaser.GetTotalMax(), 0.01f);
    m_PhaserRate.setValue(m_Phaser.GetMaxRate(), juce::dontSendNotification);
    m_PhaserRate.onDragEnd = [this]
    {
        m_Phaser.SetMax(m_PhaserRate.getValue());
    };

    addAndMakeVisible(m_PhaserRateLabel);
    m_PhaserRateLabel.setFont(juce::Font(15.f, juce::Font::bold));
    m_PhaserRateLabel.setText("Rate 1", juce::dontSendNotification);
    m_PhaserRateLabel.setJustificationType(juce::Justification::centredTop);
    m_PhaserRateLabel.attachToComponent(&m_PhaserRate, false);

    // Rate 2
    addAndMakeVisible(&m_PhaserRate2);
    m_PhaserRate2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_PhaserRate2.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_PhaserRate2.setRange(0.f, m_Phaser2.GetTotalMax(), 0.01f);
    m_PhaserRate2.setValue(m_Phaser2.GetMaxRate(), juce::dontSendNotification);
    m_PhaserRate2.onDragEnd = [this]
    {
        m_Phaser2.SetMax(m_PhaserRate2.getValue());
    };

    addAndMakeVisible(m_PhaserRateLabel2);
    m_PhaserRateLabel2.setFont(juce::Font(15.f, juce::Font::bold));
    m_PhaserRateLabel2.setText("Rate 2", juce::dontSendNotification);
    m_PhaserRateLabel2.setJustificationType(juce::Justification::centredTop);
    m_PhaserRateLabel2.attachToComponent(&m_PhaserRate2, false);

    // Depth
    addAndMakeVisible(&m_Depth);
    m_Depth.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_Depth.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_Depth.setRange(0.f, 1.f, 0.01f);
    m_Depth.setValue(m_Phaser.GetDepth(), juce::dontSendNotification);
    m_Depth.onDragEnd = [this]
    {
        m_Phaser.SetDepth(m_Depth.getValue());
        m_Phaser2.SetDepth(m_Depth.getValue());
    };

    addAndMakeVisible(m_DepthLabel);
    m_DepthLabel.setFont(juce::Font(18.f, juce::Font::bold));
    m_DepthLabel.setText("Depth", juce::dontSendNotification);
    m_DepthLabel.setJustificationType(juce::Justification::centredTop);
    m_DepthLabel.attachToComponent(&m_Depth, false);

    // Toggle P
    addAndMakeVisible(m_ToggleP1);
    m_ToggleP1.setToggleState(m_P1, juce::dontSendNotification);
    m_ToggleP1.onStateChange = [this]
    {
        m_P1 = m_ToggleP1.getToggleState();
    };

    addAndMakeVisible(m_ToggleP2);
    m_ToggleP2.setToggleState(m_P2, juce::dontSendNotification);
    m_ToggleP2.onStateChange = [this]
    {
        m_P2 = m_ToggleP2.getToggleState();
    };
}

void MainComponent::initTHDSettingsUI()
{
    const int tbW{ 40 };
    const int tbH{ 20 };

    // Max
    addAndMakeVisible(&m_THDAmount);
    m_THDAmount.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_THDAmount.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_THDAmount.setRange(0.f, m_THD.GetTotalMax(), 0.01f);
    m_THDAmount.setValue(m_THD.GetAmount(), juce::dontSendNotification);
    m_THDAmount.onDragEnd = [this]
    {
        m_THD.SetMax(m_THDAmount.getValue());
    };

    addAndMakeVisible(m_THDLabel);
    m_THDLabel.setFont(juce::Font(18.f, juce::Font::bold));
    m_THDLabel.setText("Disto", juce::dontSendNotification);
    m_THDLabel.setJustificationType(juce::Justification::centredTop);

    // Toggle
    addAndMakeVisible(m_EnableTresholdTHD);
    m_EnableTresholdTHD.setToggleState(m_UseTresholdTHD, juce::dontSendNotification);
    m_EnableTresholdTHD.onStateChange = [this]
    {
        m_UseTresholdTHD = m_EnableTresholdTHD.getToggleState();
    };

    addAndMakeVisible(m_TresholdTHD);
    m_TresholdTHD.setFont(juce::Font(18.f, juce::Font::bold));
    m_TresholdTHD.setText("THRESHOLD", juce::dontSendNotification);
    m_TresholdTHD.setJustificationType(juce::Justification::centredTop);

    // Toggle D
    addAndMakeVisible(m_ToggleD1);
    m_ToggleD1.setToggleState(m_D1, juce::dontSendNotification);
    m_ToggleD1.onStateChange = [this]
    {
        m_D1 = m_ToggleD1.getToggleState();
    };

    addAndMakeVisible(m_ToggleD2);
    m_ToggleD2.setToggleState(m_D2, juce::dontSendNotification);
    m_ToggleD2.onStateChange = [this]
    {
        m_D2 = m_ToggleD2.getToggleState();
    };
}

void MainComponent::initGainSettingsUI()
{
    const int tbW{ 40 };
    const int tbH{ 20 };

    // Min
    addAndMakeVisible(&m_GainDry);
    m_GainDry.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_GainDry.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_GainDry.setRange(m_Gain.GetMin(), m_Gain.GetMax(), 0.01f);
    m_GainDry.setValue(m_Gain.GetDry(), juce::dontSendNotification);
    m_GainDry.onValueChange = [this]
    {
        m_Gain.SetDry(m_GainDry.getValue());
    };

    m_GainDryLabel.setFont(juce::Font(18.f, juce::Font::bold));
    m_GainDryLabel.setText("Min", juce::dontSendNotification);
    m_GainDryLabel.setJustificationType(juce::Justification::centredTop);
    m_GainDryLabel.attachToComponent(&m_GainDry, false);

    // Volume 1
    addAndMakeVisible(&m_GainVolume);
    m_GainVolume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_GainVolume.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_GainVolume.setRange(m_Gain.GetMin(), m_Gain.GetMax(), 0.01f);
    m_GainVolume.setValue(m_Gain.GetVolume(), juce::dontSendNotification);

    m_GainVolumeLabel.setFont(juce::Font(15.f, juce::Font::bold));
    m_GainVolumeLabel.setText("Gain 1", juce::dontSendNotification);
    m_GainVolumeLabel.setJustificationType(juce::Justification::centredTop);
    m_GainVolumeLabel.attachToComponent(&m_GainVolume, false);

    // Volume 2
    addAndMakeVisible(&m_GainVolume2);
    m_GainVolume2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_GainVolume2.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tbW, tbH);
    m_GainVolume2.setRange(m_Gain.GetMin(), m_Gain.GetMax(), 0.01f);
    m_GainVolume2.setValue(m_Gain.GetVolume(), juce::dontSendNotification);

    m_GainVolumeLabel2.setFont(juce::Font(15.f, juce::Font::bold));
    m_GainVolumeLabel2.setText("Gain 2", juce::dontSendNotification);
    m_GainVolumeLabel2.setJustificationType(juce::Justification::centredTop);
    m_GainVolumeLabel2.attachToComponent(&m_GainVolume2, false);
}

void MainComponent::initLoadUI()
{
    addAndMakeVisible(&m_OpenButton);
    m_OpenButton.setButtonText("sample 1");
    m_OpenButton.onClick = [this] { loadFile(false); };

    addAndMakeVisible(&m_OpenButton2);
    m_OpenButton2.setButtonText("sample 2");
    m_OpenButton2.onClick = [this] { loadFile(true); };

    addAndMakeVisible(&m_PlayButton);
    m_PlayButton.setButtonText("Play");
    m_PlayButton.onClick = [this] { changeState(Playing); };
    m_PlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    m_PlayButton.setEnabled(false);

    addAndMakeVisible(&m_StopButton);
    m_StopButton.setButtonText("Stop");
    m_StopButton.onClick = [this] { changeState(Stopped); };
    m_StopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    m_StopButton.setEnabled(false);
}

void MainComponent::initKinectTrackingUI()
{
    addAndMakeVisible(m_LeftHand);
    m_LeftHand.setFont(juce::Font(m_FontSize, juce::Font::bold));
    m_LeftHand.setText("L", juce::dontSendNotification);
    m_LeftHand.setColour(juce::Label::textColourId, juce::Colours::rebeccapurple);

    addAndMakeVisible(m_RightHand);
    m_RightHand.setFont(juce::Font(m_FontSize, juce::Font::bold));
    m_RightHand.setText("R", juce::dontSendNotification);
    m_RightHand.setColour(juce::Label::textColourId, juce::Colours::yellow);
}

void MainComponent::initKinectInfoUI()
{
    addAndMakeVisible(m_ConnectedLabel);
    m_ConnectedLabel.setFont(juce::Font(24.f, juce::Font::bold));
    m_ConnectedLabel.setText("not connected", juce::dontSendNotification);
    m_ConnectedLabel.setColour(juce::Label::textColourId, juce::Colours::red);

    addAndMakeVisible(m_InfoLX);
    m_InfoLX.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoLX.setText("LX", juce::dontSendNotification);
    m_InfoLX.setColour(juce::Label::textColourId, juce::Colours::rebeccapurple);

    addAndMakeVisible(m_InfoLY);
    m_InfoLY.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoLY.setText("LY", juce::dontSendNotification);
    m_InfoLY.setColour(juce::Label::textColourId, juce::Colours::rebeccapurple);

    addAndMakeVisible(m_InfoRX);
    m_InfoRX.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoRX.setText("RX", juce::dontSendNotification);
    m_InfoRX.setColour(juce::Label::textColourId, juce::Colours::yellow);

    addAndMakeVisible(m_InfoRY);
    m_InfoRY.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoRY.setText("RY", juce::dontSendNotification);
    m_InfoRY.setColour(juce::Label::textColourId, juce::Colours::yellow);

    addAndMakeVisible(m_AngleLabel);
    m_AngleLabel.setFont(juce::Font(10.f, juce::Font::bold));
    m_AngleLabel.setText("kinect                     angle", juce::dontSendNotification); // size 10, spaces 21

    addAndMakeVisible(m_KinectAngleSlider);
    m_KinectAngleSlider.setNumDecimalPlacesToDisplay(0);
    m_KinectAngleSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    m_KinectAngleSlider.setRange(0, 20, 1);
    m_KinectAngleSlider.setValue(m_KinectAngle, juce::dontSendNotification);
    m_KinectAngleSlider.onDragEnd = [this]
    {
        m_KinectAngle = m_KinectAngleSlider.getValue();
        m_pTracker->SetAngle(m_KinectAngle);
    };
}

void MainComponent::initKinect()
{
    m_pTracker = std::make_unique<KSkeletonTracker>();

    if (SUCCEEDED(m_pTracker->Init()))
    {
        m_ConnectedLabel.setText("connected", juce::dontSendNotification);
        m_ConnectedLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

        m_pTracker->SetAngle(m_KinectAngle);
        m_pTracker->AddJointForTracking(NUI_SKELETON_POSITION_WRIST_LEFT);
        m_pTracker->AddJointForTracking(NUI_SKELETON_POSITION_WRIST_RIGHT);
    }
}

void MainComponent::initInputChannels()
{
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2);
                                    m_IChannels = granted ? 2 : 0;
                                    m_OChannels = 2; });
    }
    else
    {
        m_IChannels = 2;
        m_OChannels = 2;
        setAudioChannels(2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    m_SampleRate = sampleRate;
    m_SamplesPerBlock = samplesPerBlockExpected;

    if (m_pResamplingSource != nullptr)
    {
        m_pResamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
        m_pResamplingSource->setResamplingRatio(m_Playrate);
    }

    if (m_pResamplingSource2 != nullptr)
    {
        m_pResamplingSource2->prepareToPlay(samplesPerBlockExpected, sampleRate);
        m_pResamplingSource2->setResamplingRatio(m_Playrate);
    }

    m_Phaser.prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_Phaser.initVoices();

    m_Phaser2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_Phaser2.initVoices();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (m_State != Playing)
        return;


    if (m_pReaderSource2.get() == nullptr && m_pReaderSource.get() == nullptr)
        return;

    updateEffects();

    juce::AudioSourceChannelInfo buffer1(bufferToFill);
    juce::AudioSourceChannelInfo buffer2(bufferToFill);
    juce::AudioBuffer<float> tempBuffer1(buffer1.buffer->getNumChannels(), buffer1.numSamples);
    juce::AudioBuffer<float> tempBuffer2(buffer2.buffer->getNumChannels(), buffer2.numSamples);
    buffer1.buffer = &tempBuffer1;
    buffer2.buffer = &tempBuffer2;


    if (m_pResamplingSource != nullptr)
    {
        if(m_PRS1)
            m_pResamplingSource->setResamplingRatio(m_Playrate);
        else
            m_pResamplingSource->setResamplingRatio(1.f);

        m_pResamplingSource->getNextAudioBlock(buffer1);

        if(m_D1)
            m_THD.processBlock(buffer1);

        if (m_P1)
        {
            m_Phaser.SetRate(m_Phaser.GetMaxRate() * (1.f - m_RightWrist.x));
            m_Phaser.processBlock(buffer1);
        }

        m_Gain.SetVolume(m_GainVolume.getValue());
        m_Gain.processBlock(buffer1, 1 - m_LeftWrist.y);
    }

    if (m_pResamplingSource2 != nullptr)
    {
        if(m_PRS2)
            m_pResamplingSource2->setResamplingRatio(m_Playrate);
        else
            m_pResamplingSource2->setResamplingRatio(1.f);


        m_pResamplingSource2->getNextAudioBlock(buffer2);

        if(m_D2)
            m_THD.processBlock(buffer2);

        if (m_P2)
        {
            m_Phaser2.SetRate(m_Phaser2.GetMaxRate() * (1.f - m_RightWrist.x));
            m_Phaser2.processBlock(buffer2);
        }

        m_Gain.SetVolume(m_GainVolume2.getValue());
        m_Gain.processBlock(buffer2, m_LeftWrist.y);
    }

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        const float* buffer1Data = buffer1.buffer->getReadPointer(channel, buffer1.startSample);
        const float* buffer2Data = buffer2.buffer->getReadPointer(channel, buffer2.startSample);
        float* outputData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            if (m_pResamplingSource == nullptr)
                outputData[sample] = buffer2Data[sample];
            else if(m_pResamplingSource2 == nullptr)
                outputData[sample] = buffer1Data[sample];
            else
                outputData[sample] = buffer1Data[sample] + buffer2Data[sample];
        }
    }
}

void MainComponent::updateEffects()
{
    if (m_UseTreshold)
    {
        if (m_LeftWrist.x < m_MinTreshHold)
            setPlayrate(m_CurrentMinPR);
        else if (m_LeftWrist.x > m_MaxTreshHold)
            setPlayrate(m_CurrentMaxPR);
        else
            setPlayrate(std::abs(m_CurrentMaxPR - m_CurrentMinPR));
    }
    else
    {
        const float playrate{ m_CurrentMinPR + (m_CurrentMaxPR - m_CurrentMinPR) * m_LeftWrist.x };
        setPlayrate(playrate);
    }

    if (m_UseTresholdTHD)
    {
        if (m_RightWrist.y < 0.5f)
            m_THD.SetAmount(0.f);
        else
            m_THD.SetAmount(m_THD.GetMax() * 0.8f);
    }
    else
    {
        const float distortion{ m_THD.GetMax() * m_RightWrist.y };
        m_THD.SetAmount(distortion);
    }
}

void MainComponent::releaseResources()
{
    if(m_pResamplingSource != nullptr)
        m_pResamplingSource->releaseResources();

    if(m_pResamplingSource2 != nullptr)
        m_pResamplingSource2->releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Background rectangles
    juce::Colour rectColor{ 29, 37, 41};
    g.setColour(rectColor);

    g.fillRect(static_cast<int>(m_InfoPos.x) + 11, static_cast<int>(m_InfoPos.y) + 165, 145, 125);

    g.fillRect(static_cast<int>(m_SquarePos.x), static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);
    g.fillRect(static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing, static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);

    const int settingsW{ (getWidth() - (5 * m_SettingsSpacing)) / 4 };
    const int settingsH{ getHeight() - m_SettingsY - m_SettingsSpacing };
    g.fillRect(m_SettingsSpacing, m_SettingsY, settingsW, settingsH);
    g.fillRect(m_SettingsSpacing + settingsW + m_SettingsSpacing, m_SettingsY, settingsW, settingsH);
    g.fillRect(m_SettingsSpacing + (settingsW + m_SettingsSpacing) * 2, m_SettingsY, settingsW, settingsH);
    g.fillRect(m_SettingsSpacing + (settingsW + m_SettingsSpacing) * 3, m_SettingsY, settingsW, settingsH);

    // Treshold lines
    g.setColour(juce::Colours::beige);
    g.fillRect(static_cast<int>(m_SquarePos.x + m_MinTreshHold * m_SquareWidth), static_cast<int>(m_SquarePos.y), 3, m_SquareWidth);
    g.fillRect(static_cast<int>(m_SquarePos.x + m_MaxTreshHold * m_SquareWidth), static_cast<int>(m_SquarePos.y), 3, m_SquareWidth);

    g.fillRect(static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing, static_cast<int>(m_SquarePos.y + 0.5f * m_SquareWidth), m_SquareWidth, 3);

    // Axis names
    const int fontSize{ 20 };
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(fontSize, juce::Font::bold));

    g.drawMultiLineText("GAIN", static_cast<int>(m_SquarePos.x) - 15, static_cast<int>(m_InfoPos.y) + 120, fontSize / 2, juce::Justification::centred);
    g.drawText("GAIN", 80, m_SettingsY - 30, 100, 50, juce::Justification::bottomLeft);

    g.drawText("PLAYRATE", static_cast<int>(m_SquarePos.x) + 88, static_cast<int>(m_InfoPos.y) - 28, 100, 50, juce::Justification::bottomLeft);
    g.drawText("PLAYRATE", 257, m_SettingsY - 30, 100, 50, juce::Justification::bottomLeft);

    g.drawMultiLineText("DISTORTION", static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing - 15, static_cast<int>(m_InfoPos.y) + 55, fontSize / 2, juce::Justification::centred);
    g.drawText("DISTORTION", 437, m_SettingsY - 30, 250, 50, juce::Justification::bottomLeft);

    g.drawText("PHASE", static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing + 102, static_cast<int>(m_InfoPos.y) - 28, 100, 50, juce::Justification::bottomLeft);
    g.drawText("PHASE", 660, m_SettingsY - 30, 100, 50, juce::Justification::bottomLeft);
}

void MainComponent::resized()
{
    // Kinect info
    const int offset{ 135 };
    const int offsetX{ 15 };
    m_ConnectedLabel.setBounds(m_InfoPos.x + 19, m_InfoPos.y - 2, 200, 50);
    m_KinectAngleSlider.setBounds(m_InfoPos.x + 15, m_InfoPos.y + 38, 133, 23);
    m_AngleLabel.setBounds(m_InfoPos.x + 20, m_InfoPos.y + 40, 200, 20);
    m_InfoLX.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 30 + offset, 200, 50);
    m_InfoLY.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 52 + offset, 200, 50);
    m_InfoRX.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 80 + offset, 200, 50);
    m_InfoRY.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 102 + offset, 200, 50);

    // Sample load, play and stop
    const int bX{ 24 };
    const int bY{ 65 };
    const int bW{ 135 };
    const int bH{ 26 };
    const int bOffset{ bH + 5 };
    const int loadOffset{ 3 };
    m_OpenButton.setBounds(bX, bY, bW / 2 - loadOffset, bH);
    m_OpenButton2.setBounds(bX + bW - (bW / 2 - loadOffset), bY, bW / 2 - loadOffset, bH);
    m_PlayButton.setBounds(bX, bY + bOffset, bW, bH);
    m_StopButton.setBounds(bX, bY + bOffset * 2, bW, bH);

    // Effect settings
    int const sliderSize1{ 150 };
    int const sliderSize2{ 100 };
    int const sliderSize3{ 85 };
    int const bottomY{ m_SettingsY + 100 };
    int const toggleSpace{ 140 };

    // Gain settings
    m_GainDry.setBounds(10, bottomY - 5, sliderSize2, sliderSize2);
    m_GainVolume.setBounds(95, bottomY - 43, sliderSize2, sliderSize3);
    m_GainVolume2.setBounds(95, bottomY + 78, sliderSize2, sliderSize3);

    // Playrate settings
    m_EnableTreshold.setBounds(290, bottomY - 30, 40, 40);
    m_Treshold.setBounds(232, bottomY - 55, 145, 40);
    m_PRMaxSlider.setBounds(297, bottomY + 40, sliderSize2, sliderSize2);
    m_PRMinSlider.setBounds(210, bottomY + 40, sliderSize2, sliderSize2);
    m_TogglePRS1.setBounds(220, m_SettingsY - 5, 40, 40);
    m_TogglePRS2.setBounds(220 + toggleSpace, m_SettingsY - 5, 40, 40);

    // THD settings
    m_TresholdTHD.setBounds(426, bottomY - 55, 145, 40);
    m_EnableTresholdTHD.setBounds(485, bottomY - 30, 40, 40);
    m_THDAmount.setBounds(447, bottomY + 10, sliderSize2, sliderSize1);
    m_THDLabel.setBounds(447, bottomY + 10, 100, 50);
    m_ToggleD1.setBounds(415 - 3, m_SettingsY - 5, 40, 40);
    m_ToggleD2.setBounds(415 + toggleSpace + 3, m_SettingsY - 5, 40, 40);

    // Phaser settings
    m_PhaserRate.setBounds(600, bottomY - 43, sliderSize2, sliderSize3);
    m_PhaserRate2.setBounds(600, bottomY + 78, sliderSize2, sliderSize3);
    m_Depth.setBounds(687, bottomY, sliderSize2, sliderSize2);
    m_ToggleP1.setBounds(610, m_SettingsY - 5, 40, 40);
    m_ToggleP2.setBounds(610 + toggleSpace, m_SettingsY - 5, 40, 40);

    renderKinectTracking();
}

void MainComponent::loadFile(bool second)
{
    changeState(Loading);

    m_Chooser = std::make_unique<juce::FileChooser>("Select a Wave file",
        juce::File{},
        "*.wav");

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    m_Chooser->launchAsync(chooserFlags, [this, second](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                auto* reader = m_FormatManager.createReaderFor(file);

                if (reader != nullptr)
                {
                    if (second)
                    {
                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        m_pResamplingSource2 = std::make_unique<juce::ResamplingAudioSource>(newSource.get(), false);
                        m_PlayButton.setEnabled(true);
                        m_pReaderSource2.reset(newSource.release());

                        m_pReaderSource2->setLooping(true);
                        m_pResamplingSource2->setResamplingRatio(0.5f);
                    }
                    else
                    {
                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        m_pResamplingSource = std::make_unique<juce::ResamplingAudioSource>(newSource.get(), false);
                        m_PlayButton.setEnabled(true);
                        m_pReaderSource.reset(newSource.release());

                        m_pReaderSource->setLooping(true);
                        m_pResamplingSource->setResamplingRatio(0.5f);
                    }

                    initInputChannels();
                    changeState(Stopped);
                }
            }
        });
}

void MainComponent::timerCallback()
{
    m_pTracker->Update();
    Vector4 left = m_pTracker->GetTrackingJointPosition(NUI_SKELETON_POSITION_WRIST_LEFT);
    Vector4 right = m_pTracker->GetTrackingJointPosition(NUI_SKELETON_POSITION_WRIST_RIGHT);

    left.x = map(left.x, -1, 0, 0, 1);
    left.y = std::clamp(1 - map(left.y, -0.5, 0.5, 0, 1), 0.f, 1.f);

    right.x = map(right.x, -0.5, 0.5, 0, 1);
    right.y = std::clamp(1 - map(right.y, -0.5, 0.5, 0, 1), 0.f, 1.f);

    smooth(m_LeftWrist.x, left.x);
    smooth(m_LeftWrist.y, left.y);
    smooth(m_RightWrist.x, right.x);
    smooth(m_RightWrist.y, right.y);

    renderKinectTracking();

    juce::MessageManagerLock lock;
    m_InfoLX.setText("LX: " + juce::String::formatted("%f", m_LeftWrist.x), juce::dontSendNotification);
    m_InfoLY.setText("LY: " + juce::String::formatted("%f", m_LeftWrist.y), juce::dontSendNotification);
    m_InfoRX.setText("RX: " + juce::String::formatted("%f", m_RightWrist.x), juce::dontSendNotification);
    m_InfoRY.setText("RY: " + juce::String::formatted("%f", m_RightWrist.y), juce::dontSendNotification);
}

void MainComponent::renderKinectTracking()
{
    const int offsetX{ -12 };
    const int offsetY{ -40 };

    m_LeftHand.setBounds(static_cast<int>(m_SquarePos.x) + (m_LeftWrist.x * m_SquareWidth) + offsetX,
        static_cast<int>(m_SquarePos.y) + (m_LeftWrist.y * m_SquareWidth) + offsetY,
        80, 80);

    m_RightHand.setBounds(static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing + (m_RightWrist.x * m_SquareWidth) + offsetX,
        static_cast<int>(m_SquarePos.y) + (m_RightWrist.y * m_SquareWidth) + offsetY,
        80, 80);
}

void MainComponent::enableUpdate(bool enable)
{
    m_EnableUpdate = enable;

    if (m_EnableUpdate)
        startTimer(m_UpdateIntervalMs);
    else
        stopTimer();
}

void MainComponent::setUpdateInterval(int intervalMs)
{
    m_UpdateIntervalMs = intervalMs;

    if (m_EnableUpdate)
    {
        stopTimer();
        startTimer(m_UpdateIntervalMs);
    }
}

void MainComponent::setPlayrate(float rate)
{
    m_Playrate = std::clamp(rate, m_MinPR, m_MaxPR);
}

void MainComponent::setMinPR(float rate)
{
    m_CurrentMinPR = std::clamp(rate, m_MinPR, m_MaxPR);
}

void MainComponent::setMaxPR(float rate)
{
    m_CurrentMaxPR = std::clamp(rate, m_MinPR, m_MaxPR);
}

float MainComponent::map(float value, float min, float max, float toMin, float toMax)
{
    value = std::clamp(value, min, max);
    return toMin + (value - min) * (toMax - toMin) / (max - min);
}

void MainComponent::smooth(float& input, float target)
{
    input = input + ((target - input) * m_SmoothingFactor);
}

void MainComponent::changeState(TransportState newState)
{
    if (m_State != newState)
    {
        m_State = newState;

        switch (m_State)
        {
        case Stopped:
            m_StopButton.setEnabled(false);
            m_PlayButton.setEnabled(true);
            break;

        case Playing:
            m_PlayButton.setEnabled(false);
            m_StopButton.setEnabled(true);
            
            if (m_pReaderSource != nullptr)
                m_pReaderSource->setNextReadPosition(0);

            if (m_pReaderSource2 != nullptr)
                m_pReaderSource2->setNextReadPosition(0);

            break;

        case Loading:
            m_PlayButton.setEnabled(false);
            m_StopButton.setEnabled(false);
            break;
        }
    }
}