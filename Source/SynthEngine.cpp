#include "SynthEngine.h"

SynthEngine::SynthEngine()
{
    for (size_t i = 0; i < numActiveVoices; ++i)
    {
        const auto freq = baseFrequency.load() / float (i + 1);
        voices[i].currentFrequency = freq;
        voices[i].targetFrequency  = freq;
    }

    m_amplitude = juce::SmoothedValue<float> (targetAmplitude.load());

}

SynthEngine::~SynthEngine() = default;

void SynthEngine::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::String message;
    message << "Preparing to play audio ...\n";
    message << "samplesPerBlockExpected=" << samplesPerBlockExpected << "\n";
    message << "samplerate=" << sampleRate << "\n";
    juce::Logger::getCurrentLogger()->writeToLog (message);

    currentSampleRate = (float) sampleRate;
    for (size_t i = 0; i < numActiveVoices; ++i)
        voices[i].angleDelta = updateAngleData (voices[i].currentFrequency);

    m_amplitude.reset (currentSampleRate, AMPLITUDE_RAMP);

    envelope.setParameters(envelopeParams);
    envelope.setSampleRate(sampleRate);
    
    lowpassFilter.setMode(juce::dsp::LadderFilterMode::LPF12);
    lowpassFilter.setCutoffFrequencyHz(cutoffFrequency.load());
    juce::dsp::ProcessSpec filterSpec { sampleRate, (juce::uint32)samplesPerBlockExpected, 2 };
    lowpassFilter.prepare(filterSpec);


}

void SynthEngine::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (numActiveVoices == 0)
        return;

    if(envDirty.exchange(false)) {
        envelope.setParameters ({ envAttack.load(), envDecay.load(), envSustain.load(), envRelease.load() });
    }

    if(noteOnRequested.exchange(false))
        envelope.noteOn();
    if(noteOffRequested.exchange(false))
        envelope.noteOff();

    lowpassFilter.setCutoffFrequencyHz(cutoffFrequency.load());

    const auto base = baseFrequency.load();
    voices[0].targetFrequency = base;
    for (size_t i = 1; i < numActiveVoices; ++i)
        voices[i].targetFrequency = std::max (base / float (i + 1), MIN_VOICE_FREQ);

    m_amplitude.setTargetValue (targetAmplitude);

    const auto numChannels = bufferToFill.buffer->getNumChannels();
    const auto numSamples  = bufferToFill.numSamples;

    std::array<float, MAX_VOICES> freqIncrement {};
    for (size_t i = 0; i < numActiveVoices; ++i)
        freqIncrement[i] = (voices[i].targetFrequency - voices[i].currentFrequency) / (float) numSamples;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto gain = m_amplitude.getNextValue();
        const auto env = envelope.getNextSample();
        auto currentSample = 0.0f;

        for (size_t i = 0; i < numActiveVoices; ++i)
        {
            currentSample += std::sin (voices[i].currentAngle);
            voices[i].currentFrequency += freqIncrement[i];
            voices[i].angleDelta = updateAngleData (voices[i].currentFrequency);
            voices[i].currentAngle += voices[i].angleDelta;
            if (voices[i].currentAngle >= juce::MathConstants<float>::twoPi)
                voices[i].currentAngle -= juce::MathConstants<float>::twoPi;
        }
        currentSample /= (float) numActiveVoices;

        for (auto channel = 0; channel < numChannels; ++channel)
            bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample)[sample] = currentSample * env * gain;
    }

    juce::dsp::AudioBlock<float> block (*bufferToFill.buffer);
    auto subBlock = block.getSubBlock ((size_t) bufferToFill.startSample, (size_t) numSamples);
    juce::dsp::ProcessContextReplacing<float> ctx (subBlock);
    lowpassFilter.process (ctx);
}

void SynthEngine::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog ("Releasing audio resource ...\n");
}

float SynthEngine::updateAngleData (float frequency)
{
    auto cyclesPerSample = frequency / currentSampleRate;
    return cyclesPerSample * 2.0f * juce::MathConstants<float>::pi;
}
