# OddEbb — Gradual Chorus Model CH301

> **Lazirko Records** | Sidechain Chorus Plugin  
> Inspired by the Eventide Instant Flanger Mk II and Instant Phaser Mk II
<img width="1539" height="459" alt="image" src="https://github.com/user-attachments/assets/d8cd9e38-5262-49b8-803c-28d0eb0f2b47" />

---

## Overview

**Gradual Chorus** is a JUCE-based stereo chorus plugin with deep sidechain modulation capabilities, inspired by the voltage-controlled flanger/phaser architecture of the Eventide Instant series. It offers per-parameter LFO routing, three output topologies, and four stereo processing modes — enabling anything from subtle pitch shimmer to sidechain-driven dynamic chorus effects.

Built with `juce::AudioProcessorValueTreeState`, all parameters are fully automatable and DAW-state-persistent.

---

## Features

- **Sidechain-driven LFO** — replace the internal oscillator with a live sidechain signal for transient-reactive modulation
- **4 Processing Modes** — `MONO`, `Left/Right`, `Mid/Side`, `Transient/Sustain`
- **3 Output Topologies**
  - `~_` — Mono-safe wet/dry spread
  - `~∅` — Pseudo stereo (phase inversion)
  - `≈` — True stereo M/S blend
- **5 LFO Shapes** — Sine, Triangle, Sawtooth, Square, Reverse Sawtooth
- **Per-parameter Modulation Routing** — individually toggle LFO modulation for Mix, Delay, Width, Feedback, Rate, Depth, and Detune
- **Stereo Detune** — LFO R-channel frequency offset for organic chorus width (0.95× – 1.05×)
- **Channel Link** — lock L/R LFOs to identical phase
- **Mono Safe Mode** — sums output to mono while preserving chorus energy via side-component folding
- **Output Limiter** — `juce::dsp::Limiter` at −0.1 dB threshold
- **16-segment LED meter** — real-time LFO macro / sidechain level metering
- 
---

## Building

### Requirements

- [JUCE](https://juce.com/) 7.x or 8.x
- C++17 compiler (MSVC 2022, Clang 14+, GCC 11+)
- CMake 3.22+ **or** Projucer

### CMake

```bash
git clone --recurse-submodules https://github.com/<your-org>/OddEbb-CH301.git
cd OddEbb-CH301
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

JUCE must be available as a subdirectory or via `find_package(JUCE)`. Add to `CMakeLists.txt`:

```cmake
juce_add_plugin(OddEbbCH301
    PLUGIN_MANUFACTURER_CODE Lazr
    PLUGIN_CODE OEb1
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "OddEbb CH301"
)
```

### Projucer

Open `OddEbbCH301.jucer`, set your JUCE path, and export to your IDE.

## Author

**Maksym (Max) Lazirko**  
Lazirko Records  
