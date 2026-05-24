#include "PluginProcessor.h"
#include "PluginEditor.h"

void ChorusVoice::prepare(double, int maxDelaySamples)
{
    bufferSize = juce::jmax(8, maxDelaySamples + 8);
    buffer.assign((size_t)bufferSize, 0.0f);
    writePos = 0;
    lastOut = 0.0f;
}

void ChorusVoice::reset()
{
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writePos = 0;
    lastOut = 0.0f;
}

float ChorusVoice::readDelayFrac(float delaySamples) const
{
    auto readPos = (float)writePos - delaySamples;
    while (readPos < 0.0f)
        readPos += (float)bufferSize;

    int i0 = (int)std::floor(readPos);
    // normalize
    if (bufferSize > 0) {
        i0 %= bufferSize;
        if (i0 < 0) i0 += bufferSize;
    } else {
        return 0.0f;
    }
    int i1 = i0 + 1;
    if (i1 >= bufferSize) i1 -= bufferSize;
    const auto frac = readPos - std::floor(readPos);
    return buffer[(size_t)i0] + frac * (buffer[(size_t)i1] - buffer[(size_t)i0]);
}

void ChorusVoice::writeDelay(float sample)
{
    buffer[(size_t)writePos] = sample;
    writePos = (writePos + 1) % bufferSize;
}

float ChorusVoice::process(float input, float delayMs, float depthMs, float feedback, float lfoValue, double sampleRate)
{
    const auto modulatedDelayMs = juce::jmax(1.0f, delayMs + depthMs * lfoValue);
    const auto totalDelaySamples = modulatedDelayMs * (float)sampleRate / 1000.0f;
    const auto delayed = readDelayFrac(totalDelaySamples);
    const auto toWrite = std::tanh(input + feedback * lastOut);
    writeDelay(toWrite);
    lastOut = delayed;
    return delayed;
}

void LFO::prepare(double sampleRate)
{
    sr = sampleRate;
    phase = 0.0;
}

void LFO::reset()
{
    phase = 0.0;
}

float LFO::tick(float rateHz, int shape)
{
    phase += (double)rateHz / sr;
    if (phase >= 1.0)
        phase -= 1.0;

    const auto p = (float)phase;
    switch (shape)
    {
    case 0: return std::sin(p * juce::MathConstants<float>::twoPi);
    case 1: return p < 0.5f ? (4.0f * p - 1.0f) : (3.0f - 4.0f * p);
    case 2: return 2.0f * p - 1.0f;
    case 3: return p < 0.5f ? 1.0f : -1.0f;
    case 4: return 1.0f - 2.0f * p;
    default: break;
    }
    return 0.0f;
}

juce::AudioProcessorValueTreeState::ParameterLayout OddEbbChorusProcessor::createParams()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    using NR = NormalisableRange<float>;

    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::INPUT_GAIN, 1 }, "Input Gain", NR(-24.0f, 24.0f, 0.01f), 0.0f));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::SIDECHAIN_EXT, 1 }, "Ext Sidechain", false));
    p.push_back(std::make_unique<AudioParameterChoice>(ParameterID{ ParamID::PROC_MODE, 1 }, "Processing Mode", StringArray{ "MONO", "L/R", "M/S", "T/S" }, 1));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::LINK, 1 }, "Link", false));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::LFO_RATE, 1 }, "LFO Rate", NR(0.05f, 10.0f, 0.001f, 0.4f), 0.5f));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::LFO_DEPTH, 1 }, "LFO Depth", NR(0.0f, 1.0f, 0.001f), 0.3f));
    p.push_back(std::make_unique<AudioParameterChoice>(ParameterID{ ParamID::LFO_SHAPE, 1 }, "LFO Shape", StringArray{ "SINE", "TRI", "SAW", "SQUARE", "REV SAW" }, 0));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::MACRO_DEPTH, 1 }, "Macro Depth", NR(0.0f, 1.0f, 0.001f), 0.5f));
    p.push_back(std::make_unique<AudioParameterChoice>(ParameterID{ ParamID::TOPOLOGY, 1 }, "Topology", StringArray{ "~_", "~\xe2\x88\x85", "\xe2\x89\x88" }, 0));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_MIX_MOD, 1 }, "Topo Mix Mod", true));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_WIDTH_MOD, 1 }, "Topo Width Mod", true));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_FB_MOD, 1 }, "Topo Fb Mod", false));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::DELAY_TIME, 1 }, "Delay Time", NR(5.0f, 50.0f, 0.01f), 20.0f));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::WIDTH, 1 }, "Width", NR(0.0f, 1.0f, 0.001f), 0.5f));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::FEEDBACK, 1 }, "Feedback", NR(0.0f, 0.95f, 0.001f), 0.2f));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::MONO_SAFE, 1 }, "Mono Safe", false));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::MIX, 1 }, "Mix", NR(0.0f, 1.0f, 0.001f), 0.5f));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::OUTPUT_GAIN, 1 }, "Output Gain", NR(-24.0f, 12.0f, 0.01f), 0.0f));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::LIMIT, 1 }, "Limiter", false));

    // new params added for editor features
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ ParamID::DETUNE, 1 }, "Detune", NR(0.95f, 1.05f, 0.0001f), 1.013f));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_RATE_MOD, 1 }, "Topo Rate Mod", false));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_DEPTH_MOD, 1 }, "Topo Depth Mod", false));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::TOPO_DETUNE_MOD, 1 }, "Topo Detune Mod", false));
    p.push_back(std::make_unique<AudioParameterBool>(ParameterID{ ParamID::LFO_SIDECHAIN, 1 }, "LFO Sidechain", false));

    return { p.begin(), p.end() };
}

OddEbbChorusProcessor::OddEbbChorusProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParams())
{
    for (auto* pid : { ParamID::INPUT_GAIN, ParamID::LFO_RATE, ParamID::LFO_DEPTH, ParamID::DELAY_TIME,
                       ParamID::WIDTH, ParamID::FEEDBACK, ParamID::MIX, ParamID::OUTPUT_GAIN, ParamID::MACRO_DEPTH })
        apvts.addParameterListener(pid, this);
}

OddEbbChorusProcessor::~OddEbbChorusProcessor()
{
    for (auto* pid : { ParamID::INPUT_GAIN, ParamID::LFO_RATE, ParamID::LFO_DEPTH, ParamID::DELAY_TIME,
                       ParamID::WIDTH, ParamID::FEEDBACK, ParamID::MIX, ParamID::OUTPUT_GAIN, ParamID::MACRO_DEPTH })
        apvts.removeParameterListener(pid, this);
}

bool OddEbbChorusProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void OddEbbChorusProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    spec = { sampleRate, (juce::uint32)samplesPerBlock, 2 };

    const auto maxDelay = (int)(sampleRate * 0.1);
    voiceL.prepare(sampleRate, maxDelay);
    voiceR.prepare(sampleRate, maxDelay);
    lfoL.prepare(sampleRate);
    lfoR.prepare(sampleRate);

    limiter.prepare(spec);
    limiter.setThreshold(-0.1f);
    limiter.setRelease(80.0f);

    constexpr float rampSeconds = 0.03f;
    smoothInput.reset(sampleRate, rampSeconds);
    smoothRate.reset(sampleRate, rampSeconds);
    smoothDepth.reset(sampleRate, rampSeconds);
    smoothDelay.reset(sampleRate, rampSeconds);
    smoothWidth.reset(sampleRate, rampSeconds);
    smoothFeedback.reset(sampleRate, rampSeconds);
    smoothMix.reset(sampleRate, rampSeconds);
    smoothOutGain.reset(sampleRate, rampSeconds);
    smoothMacro.reset(sampleRate, rampSeconds);

    smoothInput.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamID::INPUT_GAIN)->load()));
    smoothRate.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::LFO_RATE)->load());
    smoothDepth.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::LFO_DEPTH)->load());
    smoothDelay.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::DELAY_TIME)->load());
    smoothWidth.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::WIDTH)->load());
    smoothFeedback.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::FEEDBACK)->load());
    smoothMix.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::MIX)->load());
    smoothOutGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(apvts.getRawParameterValue(ParamID::OUTPUT_GAIN)->load()));
    smoothMacro.setCurrentAndTargetValue(apvts.getRawParameterValue(ParamID::MACRO_DEPTH)->load());
}

void OddEbbChorusProcessor::releaseResources()
{
    voiceL.reset();
    voiceR.reset();
    lfoL.reset();
    lfoR.reset();
}

void OddEbbChorusProcessor::parameterChanged(const juce::String& pid, float value)
{
    if (pid == ParamID::INPUT_GAIN)   smoothInput.setTargetValue(juce::Decibels::decibelsToGain(value));
    if (pid == ParamID::LFO_RATE)     smoothRate.setTargetValue(value);
    if (pid == ParamID::LFO_DEPTH)    smoothDepth.setTargetValue(value);
    if (pid == ParamID::DELAY_TIME)   smoothDelay.setTargetValue(value);
    if (pid == ParamID::WIDTH)        smoothWidth.setTargetValue(value);
    if (pid == ParamID::FEEDBACK)     smoothFeedback.setTargetValue(value);
    if (pid == ParamID::MIX)          smoothMix.setTargetValue(value);
    if (pid == ParamID::OUTPUT_GAIN)  smoothOutGain.setTargetValue(juce::Decibels::decibelsToGain(value));
    if (pid == ParamID::MACRO_DEPTH)  smoothMacro.setTargetValue(value);
}

float OddEbbChorusProcessor::computeSidechainLevel(const juce::AudioBuffer<float>* sidechain, int numSamples)
{
    if (sidechain == nullptr || sidechain->getNumChannels() == 0 || numSamples <= 0)
    {
        sidechainLevel.set(0.0f);
        DBG("computeSidechainLevel: no sidechain buffer or empty (ptr=" + juce::String::toHexString((juce::pointer_sized_int)sidechain) + ")");
        return 0.0f;
    }

    double accum = 0.0;
    const auto channels = juce::jmin(2, sidechain->getNumChannels());

    for (int ch = 0; ch < channels; ++ch)
    {
        const auto* rd = sidechain->getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
            accum += (double)rd[i] * (double)rd[i];
    }

    const auto rms = std::sqrt(accum / juce::jmax(1, channels * numSamples));
    const auto value = (float)juce::jlimit(0.0, 1.0, rms * 4.0);
    sidechainLevel.set(value);
    DBG("computeSidechainLevel: channels=" + juce::String(channels) + " samples=" + juce::String(numSamples) + " rms=" + juce::String(rms) + " value=" + juce::String(value));
    return value;
}

void OddEbbChorusProcessor::applyTopology(float dryL, float dryR, float wetL, float wetR, float width, int topology, float& outL, float& outR) const
{
    switch (topology)
    {
    case 0: // ~_ mono-safe wet chorus spread
    {
        const auto monoWet = 0.5f * (wetL + wetR);
        const auto side = (wetL - wetR) * 0.25f * width;
        outL = dryL + monoWet + side;
        outR = dryR + monoWet - side;
        break;
    }
    case 1: // ~∅ pseudo stereo
    {
        const auto phaseAmt = 0.35f + 0.65f * width;
        outL = dryL + wetL;
        outR = dryR - wetR * phaseAmt;
        break;
    }
    case 2: // ≈ true stereo
    default:
    {
        const auto mid = 0.5f * (wetL + wetR);
        const auto side = 0.5f * (wetL - wetR) * (0.25f + 0.75f * width);
        outL = dryL + mid + side;
        outR = dryR + mid - side;
        break;
    }
    }
}

void OddEbbChorusProcessor::processStereo(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* sidechain, int mode)
{
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    const auto numSamples = buffer.getNumSamples();

    const auto shape = (int)apvts.getRawParameterValue(ParamID::LFO_SHAPE)->load();
    const auto topology = (int)apvts.getRawParameterValue(ParamID::TOPOLOGY)->load();
    const auto link = apvts.getRawParameterValue(ParamID::LINK)->load() > 0.5f;
    const auto monoSafe = apvts.getRawParameterValue(ParamID::MONO_SAFE)->load() > 0.5f;
    const auto mixMod = apvts.getRawParameterValue(ParamID::TOPO_MIX_MOD)->load() > 0.5f;
    const auto widthMod = apvts.getRawParameterValue(ParamID::TOPO_WIDTH_MOD)->load() > 0.5f;
    const auto fbMod = apvts.getRawParameterValue(ParamID::TOPO_FB_MOD)->load() > 0.5f;
    const auto sc = computeSidechainLevel(sidechain, numSamples) * smoothMacro.getNextValue();

    for (int i = 0; i < numSamples; ++i)
    {
        auto dryL = left[i];
        auto dryR = right[i];

        float workL = dryL;
        float workR = dryR;

        if (mode == 0)
        {
            const auto mono = 0.5f * (dryL + dryR);
            workL = mono;
            workR = mono;
        }
        else if (mode == 2)
        {
            const auto mid = 0.5f * (dryL + dryR);
            const auto side = 0.5f * (dryL - dryR);
            workL = mid;
            workR = side;
        }
        else if (mode == 3)
        {
            // T/S (transient / stereo) - use signed difference so energy preserves balance
            const auto transient = 0.5f * (dryL - dryR);
            workL = transient;
            workR = 0.5f * (dryL + dryR);
        }

    const auto baseRate = smoothRate.getNextValue();
    const auto baseDepth = smoothDepth.getNextValue();
    const auto rate = juce::jlimit(0.05f, 12.0f, baseRate + sc * 2.0f);
    const auto depthMs = juce::jlimit(0.0f, 20.0f, (0.5f + 14.5f * baseDepth) * (1.0f + 0.6f * sc));
        const auto delayMs = smoothDelay.getNextValue();
        const auto width = juce::jlimit(0.0f, 1.0f, smoothWidth.getNextValue() + (widthMod ? sc * 0.25f : 0.0f));
        const auto feedback = juce::jlimit(0.0f, 0.97f, smoothFeedback.getNextValue() + (fbMod ? sc * 0.15f : 0.0f));
        const auto mix = juce::jlimit(0.0f, 1.0f, smoothMix.getNextValue() + (mixMod ? sc * 0.15f : 0.0f));

        // LFO-sidechain override: when enabled, read the instantaneous sidechain sample and use
        // it as the LFO value (normalized). Otherwise use oscillator ticks.
        float lfoValL = 0.0f;
        float lfoValR = 0.0f;
        const auto lfoSideEnabled = apvts.getRawParameterValue(ParamID::LFO_SIDECHAIN)->load() > 0.5f;
        const auto detuneMul = apvts.getRawParameterValue(ParamID::DETUNE)->load();
        if (lfoSideEnabled && sidechain != nullptr)
        {
            // simple per-sample read of sidechain channels (up to 2), normalized to -1..1 range by RMS estimate
            float scSample = 0.0f;
            if (sidechain->getNumChannels() > 0)
                scSample += sidechain->getReadPointer(0)[i];
            if (sidechain->getNumChannels() > 1)
                scSample += sidechain->getReadPointer(1)[i];
            scSample *= 0.5f; // average
            // use the waveform directly (clamp)
            lfoValL = juce::jlimit(-1.0f, 1.0f, scSample);
            lfoValR = link ? lfoValL : juce::jlimit(-1.0f, 1.0f, scSample * detuneMul);
        }
        else
        {
            lfoValL = lfoL.tick(rate, shape);
            lfoValR = link ? lfoValL : lfoR.tick(rate * detuneMul, shape);
        }

        auto wetL = voiceL.process(workL, delayMs, depthMs, feedback, lfoValL, currentSampleRate);
        auto wetR = voiceR.process(workR, delayMs, depthMs, feedback, lfoValR, currentSampleRate);

        if (mode == 2)
        {
            const auto mid = wetL;
            const auto side = wetR;
            wetL = mid + side;
            wetR = mid - side;
        }
        else if (mode == 3)
        {
            // symmetric blend to avoid hard right-bias
            const auto wl = wetL;
            const auto wr = wetR;
            wetL = 0.6f * wl + 0.4f * wr;
            wetR = 0.6f * wr + 0.4f * wl;
        }

        float topoL = 0.0f, topoR = 0.0f;
        applyTopology(dryL, dryR, wetL, wetR, width, topology, topoL, topoR);

        auto outL = dryL * (1.0f - mix) + topoL * mix;
        auto outR = dryR * (1.0f - mix) + topoR * mix;

        if (monoSafe)
        {
            // Create a mono-safe mix that preserves chorus energy:
            // compute mono dry and a wet mono that keeps side energy by using abs(side).
            const auto monoDry = 0.5f * (dryL + dryR);
            const auto wetMid = 0.5f * (wetL + wetR);
            const auto wetSide = 0.5f * (wetL - wetR);
            const auto monoWet = wetMid + std::abs(wetSide);
            const auto monoOut = monoDry * (1.0f - mix) + monoWet * mix;
            outL = monoOut;
            outR = monoOut;
        }

        left[i] = outL;
        right[i] = outR;
    }
}

void OddEbbChorusProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    for (int ch = 2; ch < totalChannels; ++ch)
        buffer.clear(ch, 0, numSamples);

    buffer.applyGain(smoothInput.getNextValue());

    const juce::AudioBuffer<float>* sidechain = nullptr;
    // If external sidechain is enabled, try to use the sidechain bus; otherwise use the main input buffer
    if (apvts.getRawParameterValue(ParamID::SIDECHAIN_EXT)->load() > 0.5f)
    {
        DBG("processBlock: SIDECHAIN_EXT=ON, input bus count=" + juce::String((int)getBusCount(true)));
        if (getBusCount(true) > 1)
        {
            // search for any non-empty input bus other than the main bus (index 0)
            bool found = false;
            for (int busIndex = 1; busIndex < getBusCount(true); ++busIndex)
            {
                auto& sc = getBusBuffer(buffer, true, busIndex);
                DBG("processBlock: checking input bus " + juce::String(busIndex) + ", channels=" + juce::String(sc.getNumChannels()));
                if (sc.getNumChannels() > 0)
                {
                    sidechain = &sc;
                    DBG("processBlock: using external sidechain bus index=" + juce::String(busIndex));
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                sidechain = &buffer;
                DBG("processBlock: no non-empty external sidechain bus found, falling back to main buffer");
            }
        }
        else
        {
            sidechain = &buffer;
            DBG("processBlock: no external sidechain bus available, falling back to main buffer");
        }
    }
    else
    {
        sidechain = &buffer;
        DBG("processBlock: SIDECHAIN_EXT=OFF, using main input buffer as sidechain");
    }

    const auto mode = (int)apvts.getRawParameterValue(ParamID::PROC_MODE)->load();
    processStereo(buffer, sidechain, mode);

    buffer.applyGain(smoothOutGain.getNextValue());

    if (apvts.getRawParameterValue(ParamID::LIMIT)->load() > 0.5f)
    {
        // limiter was prepared for 2 channels; if the incoming buffer contains extra
        // channels (e.g., sidechain bus included), process only the main stereo channels
        if (buffer.getNumChannels() == 2)
        {
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            limiter.process(ctx);
        }
        else if (buffer.getNumChannels() > 2)
        {
            // copy main stereo into a temporary 2-channel buffer, process limiter, copy back
            juce::AudioBuffer<float> tmp(2, buffer.getNumSamples());
            tmp.clear();
            tmp.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
            tmp.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());

            juce::dsp::AudioBlock<float> block(tmp);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            limiter.process(ctx);

            // write processed main channels back
            buffer.copyFrom(0, 0, tmp, 0, 0, tmp.getNumSamples());
            buffer.copyFrom(1, 0, tmp, 1, 0, tmp.getNumSamples());
        }
    }
}

void OddEbbChorusProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OddEbbChorusProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* OddEbbChorusProcessor::createEditor()
{
    return new OddEbbChorusEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OddEbbChorusProcessor();
}