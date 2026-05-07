#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);

    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(0.0, 0.3, 0.005);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextValueSuffix(" Volume");
    slider.setValue(0.1);

    slider.addListener(this);

    addAndMakeVisible(&slider);

    
    initSlider(attackSlider, attackParams);
    initSlider(decaySlider, decayParams);
    initSlider(sustainSlider, sustainParams);
    initSlider(releaseSlider, releaseParams);
    initSlider(cutoffSlider, cutoffParams);

    attackLabel.setText("A", juce::NotificationType::dontSendNotification);
    decayLabel.setText("D", juce::NotificationType::dontSendNotification);
    sustainLabel.setText("S", juce::NotificationType::dontSendNotification);
    releaseLabel.setText("R", juce::NotificationType::dontSendNotification);

    ampLabel.setText("Vol", juce::NotificationType::dontSendNotification);
    freqLabel.setText("Freq", juce::NotificationType::dontSendNotification);
    cutoffLabel.setText("Cutoff", juce::NotificationType::dontSendNotification);

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

    playButton.setButtonText("Play");
    playButton.onClick = [this] {
        synthEngine.noteOn();
    };

    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] {
        synthEngine.noteOff();
    };

    addAndMakeVisible(&muteButton);
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&stopButton);

    addAndMakeVisible(&attackLabel);
    addAndMakeVisible(&decayLabel);
    addAndMakeVisible(&sustainLabel);
    addAndMakeVisible(&releaseLabel);
    addAndMakeVisible(&ampLabel);
    addAndMakeVisible(&freqLabel);
    addAndMakeVisible(&cutoffLabel);
   

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
    else if(s == &attackSlider)
    {
        synthEngine.setAttack ( (float) s->getValue() );
    }
    else if(s == &decaySlider)
    {
        synthEngine.setDecay ( (float) s->getValue() );
    }
    else if(s == &sustainSlider)
    {
        synthEngine.setSustain ( (float) s->getValue() );
    }
    else if(s == &releaseSlider)
    {
        synthEngine.setRelease ( (float) s->getValue() );
    }
    else if(s == &cutoffSlider)
    {
        synthEngine.setCutoffFrequency ( (float) s->getValue() );
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
    slider.setBounds(80, 30, getWidth() - 120, 20);
    ampLabel.setBounds(40, 30, 40, 20);
    frequencySlider.setBounds(80, 60, getWidth() - 120, 20);
    freqLabel.setBounds(40, 60, 40, 20);
    muteButton.setBounds(40, 90, 100, 20);
    playButton.setBounds(160, 90, 100, 20);
    stopButton.setBounds(280, 90, 100, 20);

    cutoffSlider.setBounds(80, 250, getWidth() - 120, 20);
    cutoffLabel.setBounds(40, 250, 40, 20);

    int i = 0;
    for(const auto s : envSliders)
    {
        s.slider->setBounds(60, 140 + (i*20), getWidth() - 80, 20);
        s.label->setBounds(40, 140 + (i*20), 20, 20);
        i++;
    }
}

void MainComponent::initSlider(juce::Slider &slider, SliderParams &sliderParams)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(sliderParams.rangeMin, sliderParams.rangeMax, 0.005);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextValueSuffix(sliderParams.text);
    slider.setValue(sliderParams.initValue);

    slider.addListener(this);

    addAndMakeVisible(&slider);
}
