#include "SynthEngine.h"


SynthEngine::SynthEngine()
{
    voices.reserve (MAX_FREQUENCIES);
    for (int i = 1; i <= 3; ++i)
    {
        Voice v;
        v.currentFrequency = 440.0f / (float) i;
        v.targetFrequency  = v.currentFrequency;
        voices.push_back (v);
    }

    baseFrequency = voices[0].targetFrequency;

     m_amplitude     = juce::SmoothedValue<float> (0.1f);
}

SynthEngine::~SynthEngine()
{

}

void SynthEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
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

void SynthEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (voices.empty())
        return;

    voices[0].targetFrequency = baseFrequency;

    // Sub-harmonics: voices[i] = base / (i + 1)
    for (size_t i = 1; i < voices.size(); ++i)
    {
        voices[i].targetFrequency = std::max (baseFrequency / float (i + 1), 20.0f);
    }

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

void SynthEngine::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resource ...\n");
}

float SynthEngine::updateAngleData(float frequency)
{
    auto cyclesPerSample = frequency / currentSampleRate;
    return cyclesPerSample * 2.0f * juce::MathConstants<float>::pi;
}


