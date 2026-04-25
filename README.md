# SimpleNoiseSynth

Ein additiver Sinus-Synthesizer in C++ mit JUCE. Spielt einen Grundton mit zwei
Sub-Harmonischen (f, f/2, f/3) und smoothem Volume-Ramping.

## Features

- Drei-stimmiger additiver Synthesizer (Grundton + Sub-Harmonische)
- Frequenz-Slider mit Skew-Mapping (50 Hz – 5 kHz)
- Volume-Slider mit 20 ms Smoothing
- Mute/Unmute mit Wiederherstellung der vorherigen Lautstärke

## Architektur-Highlights

- **Real-Time-sicherer Audio-Callback**: keine Heap-Allokationen, keine Locks,
  keine Syscalls auf dem Audio-Thread.
- **Lock-free Parameter-Updates**: GUI-Thread schreibt in `std::atomic<float>`,
  Audio-Thread liest einmal pro Block.
- **GUI als Single Source of Truth**: der Audio-Thread spiegelt nur,
  was der Slider vorgibt — `juce::SmoothedValue` wird nie vom GUI-Thread berührt.
- **Voice-Struct** statt paralleler Vektoren für klare Zustandskapselung.

## Build

JUCE 7+ und Projucer benötigt.

```
# Linux
cd Builds/LinuxMakefile
make CONFIG=Release

# macOS / Windows: jeweiliges Builds/-Unterverzeichnis öffnen
```

## Roadmap

- [ ] MIDI-Eingabe (Note-On/Off, Velocity, polyphone Voice-Allocation)
- [ ] ADSR-Envelope pro Voice
- [ ] Biquad-Filter (LPF/HPF mit Cutoff + Resonance)
- [ ] Plugin-Format (VST3) statt Standalone
- [ ] Unit-Tests für DSP-Komponenten
