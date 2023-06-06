#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : m_LeftWrist{ 0, 0, 0, 0 }
    , m_RightWrist{ 0, 0, 0, 0 }
    , m_UpdateIntervalMs{ 20 }
    , m_EnableUpdate{ false }
    , m_InfoPos{ 10, 0 }
    , m_SquareWidth{ 270 }
    , m_SquareSpacing{ 20 }
    , m_SquarePos{ 180, 20, 0, 0 }
    , m_CircleRadius{ 30 }
    , m_FontSize{ 24 }
    , m_SmoothingFactor{ 0.2f }
{
    setSize (800, 600);

    initKinectInfoUI();
    initLoadUI();
    initKinectTrackingUI();

    initInputChannels();
    initKinect();

    enableUpdate(true);

    m_FormatManager.registerBasicFormats();
    m_TransportSource.addChangeListener(this);
}

void MainComponent::initLoadUI()
{
    addAndMakeVisible(&m_OpenButton);
    m_OpenButton.setButtonText("Open...");
    m_OpenButton.onClick = [this] { loadFile(); };

    addAndMakeVisible(&m_PlayButton);
    m_PlayButton.setButtonText("Play");
    m_PlayButton.onClick = [this] { changeState(Starting); };
    m_PlayButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    m_PlayButton.setEnabled(false);

    addAndMakeVisible(&m_StopButton);
    m_StopButton.setButtonText("Stop");
    m_StopButton.onClick = [this] { changeState(Stopping); };
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
}

void MainComponent::initKinect()
{
    m_pTracker = std::make_unique<KSkeletonTracker>();

    if (SUCCEEDED(m_pTracker->Init()))
    {
        m_ConnectedLabel.setText("connected", juce::dontSendNotification);
        m_ConnectedLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

        m_pTracker->SetAngle(5);
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
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels(2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    m_TransportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    if (m_pReaderSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    m_TransportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    m_TransportSource.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    juce::Colour rectColor{ 29, 37, 41};
    g.setColour(rectColor);
    g.fillRect(static_cast<int>(m_InfoPos.x) + 7, static_cast<int>(m_InfoPos.y) + 165, 145, 125);
    g.fillRect(static_cast<int>(m_SquarePos.x), static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);
    g.fillRect(static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing, static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);
}

void MainComponent::resized()
{
    // Kinect info
    const int offset{ 135 };
    const int offsetX{ 12 };
    m_ConnectedLabel.setBounds(m_InfoPos.x + 16, m_InfoPos.y + 1, 200, 50);
    m_InfoLX.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 22 + offset, 200, 50);
    m_InfoLY.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 44 + offset, 200, 50);
    m_InfoRX.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 88 + offset, 200, 50);
    m_InfoRY.setBounds(m_InfoPos.x + offsetX, m_InfoPos.y + 110 + offset, 200, 50);

    m_OpenButton.setBounds(200, 0, 50, 30);
    m_PlayButton.setBounds(200, 40, 50, 30);
    m_StopButton.setBounds(200, 80, 50, 30);

    renderKinectTracking();
}

void MainComponent::loadFile()
{
    m_Chooser = std::make_unique<juce::FileChooser>("Select a Wave file",
        juce::File{},
        "*.wav");

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    m_Chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                auto* reader = m_FormatManager.createReaderFor(file);

                if (reader != nullptr)
                {
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                    m_TransportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                    m_PlayButton.setEnabled(true);
                    m_pReaderSource.reset(newSource.release());

                    m_pReaderSource->setLooping(true);
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

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &m_TransportSource)
    {
        if (m_TransportSource.isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);
    }
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
            m_TransportSource.setPosition(0.0);
            break;

        case Starting:
            m_PlayButton.setEnabled(false);
            m_TransportSource.start();
            break;

        case Playing:
            m_StopButton.setEnabled(true);
            break;

        case Stopping:
            m_TransportSource.stop();
            break;
        }
    }
}