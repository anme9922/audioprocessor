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
    juce::Slider cutoffSlider;
    juce::TextButton muteButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    juce::Label ampLabel;
    juce::Label freqLabel;
    juce::Label cutoffLabel;

    struct SliderComponent
    {
        juce::Slider* slider;
        juce::Label*  label;
    };

    std::array<SliderComponent, 4> envSliders {{
        { &attackSlider,  &attackLabel  },
        { &decaySlider,   &decayLabel   },
        { &sustainSlider, &sustainLabel },
        { &releaseSlider, &releaseLabel }
    }};

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

    SliderParams cutoffParams {50.0f, 5000.0f, " Cutoff", 500.0f};

    void initSlider(juce::Slider &slider, SliderParams &sliderParams);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
