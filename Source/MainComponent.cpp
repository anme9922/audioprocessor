#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);
    
    initSlider(slider, volumeParams);
    initSlider(attackSlider, attackParams);
    initSlider(decaySlider, decayParams);
    initSlider(sustainSlider, sustainParams);
    initSlider(releaseSlider, releaseParams);
    initSlider(cutoffSlider, cutoffParams);
    initSlider(numVoicesSlider, numVoicesParams);
    initSlider(frequencySlider, frequencyParams);

    attackLabel.setText("A", juce::NotificationType::dontSendNotification);
    decayLabel.setText("D", juce::NotificationType::dontSendNotification);
    sustainLabel.setText("S", juce::NotificationType::dontSendNotification);
    releaseLabel.setText("R", juce::NotificationType::dontSendNotification);

    ampLabel.setText("Vol", juce::NotificationType::dontSendNotification);
    freqLabel.setText("Freq", juce::NotificationType::dontSendNotification);
    cutoffLabel.setText("Cutoff", juce::NotificationType::dontSendNotification);
    numVoicesLabel.setText("Voices", juce::NotificationType::dontSendNotification);

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

    oscType.addItemList({"SINE", "WHITE", "BROWNIAN", "PINK"}, 1);
    oscType.addListener(this);
    oscType.setSelectedId(1, juce::NotificationType::dontSendNotification);

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
    addAndMakeVisible(&numVoicesLabel);

    addAndMakeVisible(&oscType);
   

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
    else if(s == &numVoicesSlider)
    {
        synthEngine.setNumActiveVoices ( (size_t) s->getValue() );
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    synthEngine.setOscillatorType(static_cast<OscillatorType>(comboBoxThatHasChanged->getSelectedId() - 1));
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

    oscType.setBounds(400, 90, 100, 20);

    cutoffSlider.setBounds(80, 250, getWidth() - 120, 20);
    cutoffLabel.setBounds(40, 250, 40, 20);

    numVoicesSlider.setBounds(80, 270, getWidth() - 120, 20);
    numVoicesLabel.setBounds(40, 270, 40, 20);

    int i = 0;
    for(const auto& s : envSliders)
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
