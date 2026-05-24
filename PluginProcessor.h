#pragma once
#include <JuceHeader.h>

namespace ParamID {
    inline constexpr auto INPUT_GAIN = "inputGain";
    inline constexpr auto SIDECHAIN_EXT = "sidechainExt";
    inline constexpr auto PROC_MODE = "procMode";
    inline constexpr auto LINK = "link";
    inline constexpr auto LFO_RATE = "lfoRate";
    inline constexpr auto LFO_DEPTH = "lfoDepth";
    inline constexpr auto LFO_SHAPE = "lfoShape";
    inline constexpr auto MACRO_DEPTH = "macroDepth";
    inline constexpr auto LFO_SIDECHAIN = "lfoSidechain";
    inline constexpr auto TOPOLOGY = "topology";
    inline constexpr auto TOPO_MIX_MOD = "topoMixMod";
    inline constexpr auto TOPO_DELAY_MOD = "topoDelayMod";
    inline constexpr auto TOPO_WIDTH_MOD = "topoWidthMod";
    inline constexpr auto TOPO_RATE_MOD = "topoRateMod";
    inline constexpr auto TOPO_DEPTH_MOD = "topoDepthMod";
    inline constexpr auto TOPO_DETUNE_MOD = "topoDetuneMod";
    inline constexpr auto DETUNE = "detune";
    inline constexpr auto TOPO_FB_MOD = "topoFbMod";
    inline constexpr auto DELAY_TIME = "delayTime";
    inline constexpr auto WIDTH = "width";
    inline constexpr auto FEEDBACK = "feedback";
    inline constexpr auto MONO_SAFE = "monoSafe";
    inline constexpr auto MIX = "mix";
    inline constexpr auto OUTPUT_GAIN = "outputGain";
    inline constexpr auto LIMIT = "limit";
}

class ChorusVoice {
public:
    void prepare(double sampleRate, int maxDelaySamples);
    void reset();
    float process(float input, float delayMs, float depthMs, float feedback, float lfoValue, double sampleRate);

private:
    float readDelayFrac(float delaySamples) const;
    void writeDelay(float sample);

    std::vector<float> buffer;
    int writePos = 0;
    int bufferSize = 0;
    mutable float lastOut = 0.0f;
};

class LFO {
public:
    void prepare(double sampleRate);
    void reset();
    float tick(float rateHz, int shape);

private:
    double sr = 44100.0;
    double phase = 0.0;
};

class OddEbbChorusProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    OddEbbChorusProcessor();
    ~OddEbbChorusProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "OddEbb Gradual Chorus"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::Atomic<float> sidechainLevel{ 0.0f };
    std::atomic<int> sidechainBusIndex{ -1 };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParams();

    void processStereo(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* sidechain, int mode);
    void applyTopology(float dryL, float dryR, float wetL, float wetR, float width, int topology, float& outL, float& outR) const;
    float computeSidechainLevel(const juce::AudioBuffer<float>* sidechain, int numSamples);

    ChorusVoice voiceL, voiceR;
    LFO lfoL, lfoR;

    juce::dsp::Limiter<float> limiter;
    juce::dsp::ProcessSpec spec{};
    double currentSampleRate = 44100.0;

    juce::SmoothedValue<float> smoothInput, smoothRate, smoothDepth, smoothDelay,
        smoothWidth, smoothFeedback, smoothMix, smoothOutGain, smoothMacro;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OddEbbChorusProcessor)
};