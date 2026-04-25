#pragma once

#include <JuceHeader.h>
#include <vector>
#include <atomic>
class SynthEngine
{
public:
    SynthEngine();
    ~SynthEngine();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    struct Voice
    {
        float currentFrequency = 0.0f;
        float targetFrequency  = 0.0f;
        float currentAngle     = 0.0f;
        float angleDelta       = 0.0f;
    };
    std::vector<Voice> voices;
    std::atomic<float> targetAmplitude;
    std::atomic<float> baseFrequency;

private:
    static constexpr size_t MAX_FREQUENCIES = 20;


    juce::SmoothedValue<float> m_amplitude;

    float currentSampleRate = 44100.0f;

    float updateAngleData (float frequency);
};