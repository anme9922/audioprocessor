# SimpleNoiseSynth

An additive sine synthesizer in C++ with JUCE. It plays a base sound with two subharmonics and smoothed volume ramping.

## Features

- Additive sine synthesizer with base frequency and subharmonics
- Frequency slider with skew mapping
- Volume slider with smooth ramping
- Mute/Unmute button
- ADSR envelope
- LowPass filter based on Moog LadderFilter

## Architecture Highlights

- **Real-time-safe audio callback**: no heap allocations, no locks,
  no syscalls on the audio thread.
- **Lock-free parameter updates**: GUI thread writes to `std::atomic<float>`,
  audio thread reads once per block.
- **GUI as single source of truth**: the audio thread only mirrors
  what the slider provides — `juce::SmoothedValue` is never touched by the GUI thread.
- **Voice struct** instead of parallel vectors for clear state encapsulation.

## Build

JUCE 7+ and Projucer required.

```
# Linux
cd Builds/LinuxMakefile
make CONFIG=Release

# macOS / Windows: open build path
```

## Roadmap

- [ ] MIDI input (Note On/Off, velocity, polyphonic voice allocation)
- [x] ADSR envelope per voice
- [ ] Biquad filter (LPF/HPF with cutoff + resonance)
- [ ] Plugin format (VST3) instead of standalone
- [ ] Unit tests for DSP components
- [ ] Modern GUI design and sound visualization
