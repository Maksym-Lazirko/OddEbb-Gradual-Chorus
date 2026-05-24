#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OddEbbLAF : public juce::LookAndFeel_V4
{
public:
    OddEbbLAF();
    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
        float sliderPos, float startAngle, float endAngle,
        juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int w, int h, bool isDown,
        int bx, int by, int bw, int bh, juce::ComboBox&) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
    juce::Font getLabelFont(juce::Label&) override;

    static juce::Colour bg() { return juce::Colour(0xffb8b8b8); }
    static juce::Colour panel() { return juce::Colour(0xffababab); }
    static juce::Colour dark() { return juce::Colour(0xff1a1a1a); }
    static juce::Colour teal() { return juce::Colour(0xff00d4c8); }
    static juce::Colour knobBg() { return juce::Colour(0xffcccccc); }
    static juce::Colour text() { return juce::Colour(0xff1a1a1a); }
};

class LabelledKnob : public juce::Component
{
public:
    LabelledKnob(const juce::String& name, juce::LookAndFeel& laf);
    void resized() override;

    juce::Slider knob;
    juce::Label label;
};

class ToggleSwitch : public juce::ToggleButton
{
public:
    ToggleSwitch(const juce::String& onLabel = "ON", const juce::String& offLabel = "OFF");
    void paint(juce::Graphics&) override;

private:
    juce::String onLbl, offLbl;
};

class MacroLEDBar : public juce::Component, private juce::Timer
{
public:
    MacroLEDBar();
    void setLevel(float v);
    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;
    juce::Atomic<float> level{ 0.0f };
    float displayLevel = 0.0f;
    static constexpr int NUM_LEDS = 16;
};

class SectionPanel : public juce::Component
{
public:
    explicit SectionPanel(const juce::String& titleText) : title(titleText) {}
    void paint(juce::Graphics&) override;

private:
    juce::String title;
};

class OddEbbChorusEditor : public juce::AudioProcessorEditor,
    private juce::Timer
{
public:
    explicit OddEbbChorusEditor(OddEbbChorusProcessor&);
    ~OddEbbChorusEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void layoutTopBar(juce::Rectangle<int>);
    void layoutInputSection(juce::Rectangle<int>);
    void layoutProcSection(juce::Rectangle<int>);
    void layoutLFOSection(juce::Rectangle<int>);
    void layoutTopoSection(juce::Rectangle<int>);
    void layoutSpaceSection(juce::Rectangle<int>);
    void layoutOutputSection(juce::Rectangle<int>);
    void styleTextButton(juce::TextButton&);
    void updateTopologyButtons();
    void updateModIndicators();

    OddEbbChorusProcessor& proc;
    OddEbbLAF laf;

    juce::Label titleLabel, modeLabel, footerTitleLabel, footerModelLabel, footerBrandLabel, scLabel, scIntLabel, scExtLabel, modeGroupLabel,
        linkLabel, linkOffLabel, linkOnLabel, shapeLabel, macroLabel,
        wetDryLabel, pseudoLabel, trueLabel, mixModLabel, delayModLabel, widthModLabel, fbModLabel,
        sidechainControlsLabel,
        rateModLabel, depthModLabel, detuneLabel,
        lfoSidechainLabel,
        monoSafeLabel, msOffLabel, msOnLabel, limitLabel, limOffLabel, limOnLabel;

    juce::TextButton prevBtn, nextBtn, loadBtn, saveBtn, compareBtn, infoBtn, settingsBtn;
    SectionPanel secInput, secProc, secLFO, secTopo, secSpace, secOut;

    LabelledKnob knobInput, knobRate, knobDepth, knobDelay, knobWidth, knobFeedback, knobMix, knobOut;
    LabelledKnob knobDetune;
    ToggleSwitch switchSidechain, switchLink, switchMonoSafe, switchLimit, swMixMod, swDelayMod, swWidthMod, swFbMod;
    ToggleSwitch swRateMod, swDepthMod;
    ToggleSwitch swLfoSidechain;
    ToggleSwitch swDetuneMod;
    juce::ComboBox modeCombo, shapeCombo;
    MacroLEDBar macroBar;
    MacroLEDBar scMacroBar;
    juce::Label scLevelLabel;
    juce::TextButton topoWetDry, topoPseudo, topoTrue;

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAtt> attInput, attRate, attDepth, attDelay, attWidth, attFeedback, attMix, attOut;
    std::unique_ptr<SliderAtt> attDetune;
    std::unique_ptr<ButtonAtt> attSidechain, attLink, attMonoSafe, attLimit, attMixMod, attDelayMod, attWidthMod, attFbMod;
    std::unique_ptr<ButtonAtt> attDetuneMod;
    std::unique_ptr<ButtonAtt> attRateMod, attDepthMod;
    std::unique_ptr<ButtonAtt> attLfoSidechain;
    std::unique_ptr<ComboAtt> attMode, attShape;
    std::vector<juce::Rectangle<int>> dots;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OddEbbChorusEditor)
};