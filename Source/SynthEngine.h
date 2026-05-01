#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

class SynthEngine
{
public:
    SynthEngine();
    ~SynthEngine();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void  setBaseFrequency   (float hz)   { baseFrequency   = hz; }
    void  setTargetAmplitude (float amp)  { targetAmplitude = amp; }
    float getBaseFrequency() const        { return baseFrequency.load(); }
    void  noteOn()  { noteOnRequested = true; }
    void  noteOff()  { noteOffRequested = true; }

    void setAttack(float a) {envAttack = a; envDirty = true;}
    void setDecay(float d) {envDecay = d; envDirty = true;}
    void setSustain(float s) {envSustain = s; envDirty = true;}
    void setRelease(float r) {envRelease = r; envDirty = true;}

private:
    static constexpr size_t MAX_VOICES     = 20;
    static constexpr size_t INITIAL_VOICES = 3;
    static constexpr float AMPLITUDE_RAMP  = 0.02f; 
    static constexpr float MIN_VOICE_FREQ  = 20.0f;

    struct Voice
    {
        float currentFrequency = 0.0f;
        float targetFrequency  = 0.0f;
        float currentAngle     = 0.0f;
        float angleDelta       = 0.0f;
    };
    
    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;
    std::atomic<bool> noteOnRequested { false };
    std::atomic<bool> noteOffRequested { false };
    
    float updateAngleData (float frequency);
    
    std::array<Voice, MAX_VOICES> voices {};
    size_t numActiveVoices = INITIAL_VOICES;
    
    juce::SmoothedValue<float> m_amplitude;
    float currentSampleRate = 44100.0f;
    
    std::atomic<float> targetAmplitude { 0.1f };
    std::atomic<float> baseFrequency   { 440.0f };
    
    std::atomic<float> envAttack {0.05f};
    std::atomic<float> envDecay { 0.1f };
    std::atomic<float> envSustain { 1.0f };
    std::atomic<float> envRelease { 0.5f };
    std::atomic<bool> envDirty { false };

    juce::Random random;

    juce::dsp::LadderFilter<float> lowpassFilter;
};
