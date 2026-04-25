#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);

    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(0.0, 0.3, 0.05);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextValueSuffix(" Volume");
    slider.setValue(0.1);

    slider.addListener(this);

    addAndMakeVisible(&slider);

    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setRange(50, 5000, 0.1f);
    frequencySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    frequencySlider.setPopupDisplayEnabled(true, false, this);
    frequencySlider.setTextValueSuffix(" Frequency");
    frequencySlider.setValue(synthEngine.getBaseFrequency(), juce::dontSendNotification);
    frequencySlider.setSkewFactorFromMidPoint(500);

    frequencySlider.addListener(this);

    addAndMakeVisible(&frequencySlider);

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
    synthEngine.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    synthEngine.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    synthEngine.releaseResources();
}

void MainComponent::sliderValueChanged (juce::Slider* s)
{
    if (s == &slider)
    {
        synthEngine.setTargetAmplitude ((float) s->getValue());
    }
    else if (s == &frequencySlider)
    {
        synthEngine.setBaseFrequency ((float) s->getValue());
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
