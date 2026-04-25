#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);
    setAudioChannels(0, 2);

    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(0.0, 0.3, 0.05);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextValueSuffix(" Volume");
    slider.setValue(0.1);

    slider.addListener(this);

    addAndMakeVisible(&slider);

    voices.reserve (MAX_FREQUENCIES);
    for (int i = 1; i <= 3; ++i)
    {
        Voice v;
        v.currentFrequency = 440.0f / (float) i;
        v.targetFrequency  = v.currentFrequency;
        voices.push_back (v);
    }

    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setRange(50, 5000, 0.1f);
    frequencySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    frequencySlider.setPopupDisplayEnabled(true, false, this);
    frequencySlider.setTextValueSuffix(" Frequency");
    frequencySlider.setValue(voices[0].targetFrequency, juce::dontSendNotification);
    frequencySlider.setSkewFactorFromMidPoint(500);

    frequencySlider.addListener(this);

    addAndMakeVisible(&frequencySlider);

    m_amplitude     = juce::SmoothedValue<float> (0.1f);

    muteButton.setButtonText ("mute");
    muteButton.onClick = [this] {
        if (slider.getValue() <= 0.0f)
        {
            muteButton.setButtonText("mute");
            slider.setValue(storedAmplitude);
        }
        else 
        {
            storedAmplitude = slider.getValue();
            muteButton.setButtonText("unmute");
            slider.setValue(0.0f);
        }
    };

    addAndMakeVisible(&muteButton);
   

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::String message;
    message << "Preparing to play audio ...\n";
    message << "samplesPerBlockExpected=" << samplesPerBlockExpected << "\n";
    message << "samplerate=" << sampleRate << "\n";
    juce::Logger::getCurrentLogger()->writeToLog(message);

    currentSampleRate = sampleRate;
    for(size_t i = 0; i < voices.size(); i++ )
    {
        voices[i].angleDelta = updateAngleData(voices[i].currentFrequency);
    }

    m_amplitude.reset(currentSampleRate, 0.02); // 20 ms ramp
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (voices.empty())
        return;

    m_amplitude.setTargetValue (targetAmplitude);
    const auto numChannels = bufferToFill.buffer->getNumChannels();
    const auto numSamples  = bufferToFill.numSamples;

    // Frequency-Ramp per voice for this block
    std::array<float, MAX_FREQUENCIES> freqIncrement {};
    for (size_t i = 0; i < voices.size(); ++i)
    {
        freqIncrement[i] = (voices[i].targetFrequency - voices[i].currentFrequency) / (float) numSamples;
    }

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto gain = m_amplitude.getNextValue();
        auto currentSample = 0.0f;

        for (size_t i = 0; i < voices.size(); ++i)
        {
            currentSample += std::sin (voices[i].currentAngle);
            voices[i].currentFrequency += freqIncrement[i];
            voices[i].angleDelta = updateAngleData (voices[i].currentFrequency);
            voices[i].currentAngle += voices[i].angleDelta;
            if (voices[i].currentAngle >= juce::MathConstants<float>::twoPi)
                voices[i].currentAngle -= juce::MathConstants<float>::twoPi;
        }
        currentSample /= (float) voices.size();

        for (auto channel = 0; channel < numChannels; ++channel)
        {
            bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample)[sample] = currentSample * gain;
        }
    }
}

void MainComponent::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resource ...\n");
}

void MainComponent::sliderValueChanged (juce::Slider* s)
{
    if (s == &slider)
    {
        targetAmplitude = (float) s->getValue();
    }
    else if (s == &frequencySlider)
    {
        const auto base = (float) s->getValue();
        voices[0].targetFrequency = base;

        // Sub-harmonics: voices[i] = base / (i + 1)
        for (size_t i = 1; i < voices.size(); ++i)
        {
            voices[i].targetFrequency = std::max (base / float (i + 1), 20.0f);
        }
    }
}


//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    slider.setBounds(40, 30, getWidth() - 80, 20);
    frequencySlider.setBounds(40, 60, getWidth() - 80, 20);
    muteButton.setBounds(40, 90, 100, 20);
}

float MainComponent::updateAngleData(float frequency)
{
    auto cyclesPerSample = frequency / currentSampleRate;
    return cyclesPerSample * 2.0f * juce::MathConstants<float>::pi;
}

