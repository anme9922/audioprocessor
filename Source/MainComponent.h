#pragma once

#include <JuceHeader.h>
#include "SynthEngine.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                    public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void sliderValueChanged (juce::Slider* slider) override;
  
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    float storedAmplitude;
    juce::Slider slider;
    juce::Slider frequencySlider;
    juce::TextButton muteButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    std::array<juce::Slider*, 4> envSliders {&attackSlider, &decaySlider, &sustainSlider, &releaseSlider};

    SynthEngine synthEngine;

    struct SliderParams 
    {
        float rangeMin = 0.0f;
        float rangeMax = 0.0f;
        const char* text = "";
        float initValue = 0.0f;
    };

    SliderParams attackParams {0.0f, 5.0f, " Attack", 0.05f};
    SliderParams decayParams {0.0f, 5.0f, " Decay", 0.1f};
    SliderParams sustainParams {0.0f, 1.0f, " Sustain", 0.1f};
    SliderParams releaseParams {0.0f, 5.0f, " Release", 0.07f};

    void initSlider(juce::Slider &slider, SliderParams &sliderParams);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
