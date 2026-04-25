#pragma once

#include <JuceHeader.h>
#include <vector>
#include <atomic>

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
    static constexpr size_t MAX_FREQUENCIES = 20;

    struct Voice
    {
        float currentFrequency = 0.0f;
        float targetFrequency  = 0.0f;
        float currentAngle     = 0.0f;
        float angleDelta       = 0.0f;
    };

    juce::SmoothedValue<float> m_amplitude;
    float storedAmplitude;
    std::atomic<float> targetAmplitude;
    juce::Slider slider;
    juce::Slider frequencySlider;
    juce::TextButton muteButton;

    std::vector<Voice> voices;
    float currentSampleRate = 44100.0f;

    float updateAngleData (float frequency);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
