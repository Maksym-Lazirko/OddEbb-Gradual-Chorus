#include "PluginEditor.h"

// Helper to draw topology glyphs in a font-independent way
namespace {
    // Draw a short wavy (tilde-like) path inside rect
    static void drawWavy(juce::Graphics& g, juce::Rectangle<float> r, float amplitude = 4.0f)
    {
        juce::Path p;
        const int steps = 24;
        for (int i = 0; i <= steps; ++i) {
            float t = (float)i / (float)steps;
            float x = r.getX() + t * r.getWidth();
            float y = r.getCentreY() + std::sin(t * juce::MathConstants<float>::twoPi * 1.5f) * amplitude;
            if (i == 0) p.startNewSubPath(x, y);
            else p.lineTo(x, y);
        }
        g.strokePath(p, juce::PathStrokeType(2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void drawTopologyGlyph(juce::Graphics& g, juce::Rectangle<int> r, int which)
    {
        auto rf = r.toFloat().reduced(6.0f);
        // background
        g.setColour(juce::Colours::black.withAlpha(0.10f));
        g.fillRoundedRectangle(r.toFloat(), 6.0f);
        g.setColour(juce::Colours::black.withAlpha(0.18f));
        g.fillRoundedRectangle(rf, 6.0f);

        g.setColour(juce::Colours::white);
        // draw tilde
        drawWavy(g, juce::Rectangle<float>(rf.getX(), rf.getY() + rf.getHeight() * 0.25f, rf.getWidth() * 0.6f, rf.getHeight() * 0.5f), 3.2f);

        if (which == 1) {
            // draw small circle for pseudo
            float cx = rf.getRight() - rf.getWidth() * 0.18f;
            float cy = rf.getCentreY();
            float cr = juce::jmin(rf.getWidth(), rf.getHeight()) * 0.12f;
            g.setColour(juce::Colours::white);
            g.drawEllipse(cx - cr, cy - cr, cr * 2.0f, cr * 2.0f, 2.0f);
        }
        else if (which == 2) {
            // draw approx waves (two small wavy strokes)
            drawWavy(g, juce::Rectangle<float>(rf.getX(), rf.getY() + rf.getHeight() * 0.15f, rf.getWidth() * 0.7f, rf.getHeight() * 0.28f), 2.2f);
            drawWavy(g, juce::Rectangle<float>(rf.getX(), rf.getY() + rf.getHeight() * 0.55f, rf.getWidth() * 0.7f, rf.getHeight() * 0.28f), 2.2f);
        }
    }
}

OddEbbLAF::OddEbbLAF()
{
    setColour(juce::Slider::thumbColourId, knobBg());
    setColour(juce::Slider::rotarySliderFillColourId, teal());
    setColour(juce::Label::textColourId, text());
    setColour(juce::ComboBox::backgroundColourId, dark());
    setColour(juce::ComboBox::textColourId, juce::Colours::white);
}

void OddEbbLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
    float sliderPos, float startAngle, float endAngle, juce::Slider&)
{
    const auto radius = (float)juce::jmin(w, h) * 0.5f - 4.0f;
    const auto cx = (float)x + (float)w * 0.5f;
    const auto cy = (float)y + (float)h * 0.5f;

    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(cx - radius + 2.0f, cy - radius + 2.0f, radius * 2.0f, radius * 2.0f);

    juce::ColourGradient grad(juce::Colour(0xffe0e0e0), cx - radius * 0.4f, cy - radius * 0.4f,
        juce::Colour(0xff888888), cx + radius * 0.4f, cy + radius * 0.4f, true);
    g.setGradientFill(grad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    g.setColour(juce::Colour(0xff606060));
    g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    juce::Path track;
    track.addCentredArc(cx, cy, radius + 5.0f, radius + 5.0f, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.strokePath(track, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const auto angle = startAngle + sliderPos * (endAngle - startAngle);
    juce::Path fill;
    fill.addCentredArc(cx, cy, radius + 5.0f, radius + 5.0f, 0.0f, startAngle, angle, true);
    g.setColour(teal());
    g.strokePath(fill, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const auto px = cx + (radius - 5.0f) * std::sin(angle);
    const auto py = cy - (radius - 5.0f) * std::cos(angle);
    g.setColour(dark());
    g.drawLine(cx, cy, px, py, 2.5f);
    g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

void OddEbbLAF::drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) {}

void OddEbbLAF::drawComboBox(juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&)
{
    g.setColour(dark());
    g.fillRoundedRectangle(0.0f, 0.0f, (float)w, (float)h, 4.0f);
    g.setColour(teal());
    g.drawRoundedRectangle(0.5f, 0.5f, (float)w - 1.0f, (float)h - 1.0f, 4.0f, 1.0f);
}

void OddEbbLAF::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(getLabelFont(label));
    g.drawText(label.getText(), label.getLocalBounds(), label.getJustificationType(), true);
}

juce::Font OddEbbLAF::getLabelFont(juce::Label&)
{
    return { 10.0f };
}

LabelledKnob::LabelledKnob(const juce::String& name, juce::LookAndFeel& laf)
{
    knob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knob.setLookAndFeel(&laf);
    addAndMakeVisible(knob);

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, OddEbbLAF::text());
    label.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    addAndMakeVisible(label);
}

void LabelledKnob::resized()
{
    auto area = getLocalBounds();
    label.setBounds(area.removeFromBottom(14));
    knob.setBounds(area);
}

ToggleSwitch::ToggleSwitch(const juce::String& onLabel, const juce::String& offLabel)
    : onLbl(onLabel), offLbl(offLabel) {
}

void ToggleSwitch::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    const auto on = getToggleState();
    const auto tw = b.getWidth() * 0.55f;
    const auto th = b.getHeight() * 0.9f;
    const auto tx = (b.getWidth() - tw) * 0.5f;
    const auto ty = (b.getHeight() - th) * 0.5f;
    const juce::Rectangle<float> body(tx, ty, tw, th);

    g.setColour(on ? OddEbbLAF::teal().withAlpha(0.25f) : OddEbbLAF::dark().withAlpha(0.6f));
    g.fillRoundedRectangle(body, 3.0f);
    g.setColour(on ? OddEbbLAF::teal() : juce::Colour(0xff555555));
    g.drawRoundedRectangle(body, 3.0f, 1.0f);

    const auto kw = tw * 0.45f;
    const auto kh = th * 0.85f;
    const auto kx = on ? (tx + tw - kw - 2.0f) : (tx + 2.0f);
    const auto ky = ty + (th - kh) * 0.5f;
    juce::ColourGradient kg(juce::Colour(0xffe0e0e0), kx, ky, juce::Colour(0xff909090), kx + kw, ky + kh, false);
    g.setGradientFill(kg);
    g.fillRoundedRectangle(kx, ky, kw, kh, 2.0f);

    g.setColour(on ? OddEbbLAF::teal() : juce::Colour(0xff333333));
    g.fillEllipse(tx + tw + 4.0f, ty + th * 0.5f - 3.0f, 6.0f, 6.0f);
}

MacroLEDBar::MacroLEDBar()
{
    startTimerHz(30);
}

void MacroLEDBar::setLevel(float v)
{
    level.set(juce::jlimit(0.0f, 1.0f, v));
}

void MacroLEDBar::timerCallback()
{
    displayLevel += (level.get() - displayLevel) * 0.2f;
    repaint();
}

void MacroLEDBar::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    const auto ledW = b.getWidth() / (float)NUM_LEDS;
    const auto lit = (int)std::round(displayLevel * (float)NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; ++i)
    {
        const auto x = b.getX() + (float)i * ledW + 1.0f;
        const auto w = ledW - 2.0f;
        const auto on = i < lit;
        const auto c = on ? OddEbbLAF::teal() : juce::Colour(0xff333333);
        g.setColour(c);
        g.fillRoundedRectangle(x, b.getY() + 2.0f, w, b.getHeight() - 4.0f, 1.5f);
    }
}

void SectionPanel::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(OddEbbLAF::panel());
    g.fillRoundedRectangle(b, 6.0f);
    g.setColour(juce::Colour(0xff808080));
    g.drawRoundedRectangle(b.reduced(0.5f), 6.0f, 1.0f);
    g.setColour(OddEbbLAF::text());
    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.drawText(title, getLocalBounds().removeFromTop(20).withTrimmedLeft(8), juce::Justification::left, false);
}

OddEbbChorusEditor::OddEbbChorusEditor(OddEbbChorusProcessor& p)
    : AudioProcessorEditor(&p), proc(p),
    secInput("INPUT / SIDECHAIN"), secProc("PROCESSING MODE"), secLFO("LFO / MODULATION"),
    secTopo("TOPOLOGY"), secSpace("SPACE / WIDTH"), secOut("OUTPUT"),
    knobInput("INPUT", laf), knobRate("RATE", laf), knobDepth("DEPTH", laf), knobDetune("DETUNE", laf),
    knobDelay("DELAY", laf), knobWidth("WIDTH", laf), knobFeedback("FEEDBACK / RES", laf),
    knobMix("MIX", laf), knobOut("OUTPUT", laf),
    switchSidechain("EXT", "INT"), switchLink("ON", "OFF"), switchMonoSafe("ON", "OFF"),
    switchLimit("ON", "OFF"), swMixMod("ON", "OFF"), swDelayMod("ON", "OFF"), swWidthMod("ON", "OFF"), swFbMod("ON", "OFF"), swDetuneMod("ON","OFF")
{
    setLookAndFeel(&laf);
    setSize(1540, 460);

    addAndMakeVisible(secInput);
    addAndMakeVisible(secProc);
    addAndMakeVisible(secLFO);
    addAndMakeVisible(secTopo);
    addAndMakeVisible(secSpace);
    addAndMakeVisible(secOut);

    addAndMakeVisible(knobInput);
    addAndMakeVisible(knobRate);
    addAndMakeVisible(knobDepth);
    addAndMakeVisible(knobDetune);
    addAndMakeVisible(knobDelay);
    addAndMakeVisible(knobWidth);
    addAndMakeVisible(knobFeedback);
    addAndMakeVisible(knobMix);
    addAndMakeVisible(knobOut);

    addAndMakeVisible(switchSidechain);
    addAndMakeVisible(switchLink);
    addAndMakeVisible(swDetuneMod);
    addAndMakeVisible(switchMonoSafe);
    addAndMakeVisible(switchLimit);
    addAndMakeVisible(swMixMod);
    addAndMakeVisible(swRateMod);
    addAndMakeVisible(swDepthMod);
    addAndMakeVisible(swLfoSidechain);
    addAndMakeVisible(swLfoSidechain);
    addAndMakeVisible(swDelayMod);
    addAndMakeVisible(swWidthMod);
    addAndMakeVisible(swFbMod);

    addAndMakeVisible(modeCombo);
    addAndMakeVisible(shapeCombo);
    addAndMakeVisible(macroBar);
    addAndMakeVisible(scMacroBar);
    addAndMakeVisible(scLevelLabel);
    addAndMakeVisible(topoWetDry);
    addAndMakeVisible(topoPseudo);
    addAndMakeVisible(topoTrue);

    addAndMakeVisible(titleLabel);
    addAndMakeVisible(modeLabel);
    addAndMakeVisible(footerTitleLabel);
    addAndMakeVisible(footerModelLabel);
    addAndMakeVisible(footerBrandLabel);
    addAndMakeVisible(rateModLabel);
    addAndMakeVisible(depthModLabel);
    addAndMakeVisible(lfoSidechainLabel);
    addAndMakeVisible(lfoSidechainLabel);
    addAndMakeVisible(scLabel);
    addAndMakeVisible(scIntLabel);
    addAndMakeVisible(scExtLabel);
    addAndMakeVisible(sidechainControlsLabel);
    sidechainControlsLabel.setText("Sidechain Controls:", juce::dontSendNotification);
    sidechainControlsLabel.setColour(juce::Label::textColourId, OddEbbLAF::text());
    sidechainControlsLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    addAndMakeVisible(modeGroupLabel);
    addAndMakeVisible(linkLabel);
    addAndMakeVisible(linkOffLabel);
    addAndMakeVisible(linkOnLabel);
    addAndMakeVisible(detuneLabel);
    addAndMakeVisible(shapeLabel);
    addAndMakeVisible(macroLabel);
    addAndMakeVisible(wetDryLabel);
    addAndMakeVisible(pseudoLabel);
    addAndMakeVisible(trueLabel);
    addAndMakeVisible(mixModLabel);
    addAndMakeVisible(delayModLabel);
    addAndMakeVisible(widthModLabel);
    addAndMakeVisible(fbModLabel);
    addAndMakeVisible(swRateMod);
    addAndMakeVisible(swDepthMod);
    addAndMakeVisible(swLfoSidechain);
    addAndMakeVisible(rateModLabel);
    addAndMakeVisible(depthModLabel);
    addAndMakeVisible(monoSafeLabel);
    addAndMakeVisible(msOffLabel);
    addAndMakeVisible(msOnLabel);
    addAndMakeVisible(limitLabel);
    addAndMakeVisible(limOffLabel);
    addAndMakeVisible(limOnLabel);

    // top-bar utility buttons removed

    titleLabel.setText("OddEbb", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, OddEbbLAF::teal());
    titleLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    modeLabel.setText("SIDECHAIN CHORUS", juce::dontSendNotification);
    modeLabel.setColour(juce::Label::textColourId, OddEbbLAF::teal());
    modeLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    footerTitleLabel.setText("Lazirko Records", juce::dontSendNotification);
    footerTitleLabel.setJustificationType(juce::Justification::centred);
    footerTitleLabel.setColour(juce::Label::textColourId, OddEbbLAF::text());
    footerTitleLabel.setFont(juce::FontOptions(10.5f, juce::Font::plain));
    // move the model text into the input/sidechain section bottom-left
    footerModelLabel.setText("GRADUAL CHORUS MODEL CH301", juce::dontSendNotification);
    footerModelLabel.setJustificationType(juce::Justification::left);
    footerModelLabel.setColour(juce::Label::textColourId, OddEbbLAF::text());
    footerModelLabel.setFont(juce::FontOptions(9.0f, juce::Font::plain));

    detuneLabel.setText("DETUNE", juce::dontSendNotification);
    detuneLabel.setColour(juce::Label::textColourId, OddEbbLAF::text());
    detuneLabel.setFont(juce::FontOptions(8.0f, juce::Font::plain));
    knobDetune.label.setFont(juce::FontOptions(9.0f, juce::Font::bold));

    for (auto* b : { &topoWetDry, &topoPseudo, &topoTrue })
        styleTextButton(*b);

    // Use ASCII fallback labels; glyphs are drawn manually in paint()
    topoWetDry.setButtonText(u8"~_");      // ~_
    topoPseudo.setButtonText(juce::String::fromUTF8("\x7E\xE2\x88\x85"));  // ~∅ (U+007E U+2205)
    topoTrue.setButtonText(juce::String::fromUTF8("\xE2\x89\x88"));         // ≈ (U+2248)

    scLabel.setText("SIDECHAIN", juce::dontSendNotification);
    scIntLabel.setText("INT", juce::dontSendNotification); scExtLabel.setText("EXT", juce::dontSendNotification);
    modeGroupLabel.setText("INPUT MODE", juce::dontSendNotification);
    linkLabel.setText("LINK LFO", juce::dontSendNotification); linkOffLabel.setText("OFF", juce::dontSendNotification); linkOnLabel.setText("ON", juce::dontSendNotification);
    shapeLabel.setText("SHAPE", juce::dontSendNotification); macroLabel.setText("MACRO EFFECT", juce::dontSendNotification);
    wetDryLabel.setText("WET/DRY", juce::dontSendNotification); pseudoLabel.setText("PSEUDO-ST", juce::dontSendNotification); trueLabel.setText("TRUE STEREO", juce::dontSendNotification);
    mixModLabel.setText("MIX", juce::dontSendNotification); delayModLabel.setText("DELAY", juce::dontSendNotification); widthModLabel.setText("WIDTH", juce::dontSendNotification); fbModLabel.setText("FEEDBACK", juce::dontSendNotification);
    rateModLabel.setText("RATE", juce::dontSendNotification); depthModLabel.setText("DEPTH", juce::dontSendNotification);
    lfoSidechainLabel.setText("LFO SC", juce::dontSendNotification);
    monoSafeLabel.setText("MONO SAFE OUTPUT", juce::dontSendNotification); msOffLabel.setText("OFF", juce::dontSendNotification); msOnLabel.setText("ON", juce::dontSendNotification);
    limitLabel.setText("LIMIT", juce::dontSendNotification); limOffLabel.setText("OFF", juce::dontSendNotification); limOnLabel.setText("ON", juce::dontSendNotification);

    for (auto* l : { &scLabel, &modeGroupLabel, &linkLabel, &shapeLabel, &macroLabel, &monoSafeLabel, &limitLabel, &mixModLabel, &delayModLabel, &widthModLabel, &fbModLabel })
    {
        l->setColour(juce::Label::textColourId, OddEbbLAF::text());
        l->setFont(juce::FontOptions(9.0f, juce::Font::bold));
    }
    for (auto* l : { &scIntLabel, &scExtLabel, &linkOffLabel, &linkOnLabel, &wetDryLabel, &pseudoLabel, &trueLabel, &msOffLabel, &msOnLabel, &limOffLabel, &limOnLabel, &rateModLabel, &depthModLabel, &lfoSidechainLabel })
    {
        l->setColour(juce::Label::textColourId, OddEbbLAF::text());
        l->setFont(juce::FontOptions(8.0f, juce::Font::plain));
    }

    modeCombo.addItem("MONO", 1); modeCombo.addItem("L/R", 2); modeCombo.addItem("M/S", 3); modeCombo.addItem("T/S", 4);
    shapeCombo.addItem("SINE", 1); shapeCombo.addItem("TRI", 2); shapeCombo.addItem("SAW", 3); shapeCombo.addItem("SQUARE", 4); shapeCombo.addItem("REV SAW", 5);

    topoWetDry.onClick = [this] { proc.apvts.getParameterAsValue(ParamID::TOPOLOGY) = 0; updateTopologyButtons(); };
    topoPseudo.onClick = [this] { proc.apvts.getParameterAsValue(ParamID::TOPOLOGY) = 1; updateTopologyButtons(); };
    topoTrue.onClick = [this] { proc.apvts.getParameterAsValue(ParamID::TOPOLOGY) = 2; updateTopologyButtons(); };

    attInput = std::make_unique<SliderAtt>(proc.apvts, ParamID::INPUT_GAIN, knobInput.knob);
    attRate = std::make_unique<SliderAtt>(proc.apvts, ParamID::LFO_RATE, knobRate.knob);
    attDepth = std::make_unique<SliderAtt>(proc.apvts, ParamID::LFO_DEPTH, knobDepth.knob);
    attDelay = std::make_unique<SliderAtt>(proc.apvts, ParamID::DELAY_TIME, knobDelay.knob);
    attWidth = std::make_unique<SliderAtt>(proc.apvts, ParamID::WIDTH, knobWidth.knob);
    attFeedback = std::make_unique<SliderAtt>(proc.apvts, ParamID::FEEDBACK, knobFeedback.knob);
    attDetune = std::make_unique<SliderAtt>(proc.apvts, ParamID::DETUNE, knobDetune.knob);
    attMix = std::make_unique<SliderAtt>(proc.apvts, ParamID::MIX, knobMix.knob);
    attOut = std::make_unique<SliderAtt>(proc.apvts, ParamID::OUTPUT_GAIN, knobOut.knob);
    attSidechain = std::make_unique<ButtonAtt>(proc.apvts, ParamID::SIDECHAIN_EXT, switchSidechain);
    attLink = std::make_unique<ButtonAtt>(proc.apvts, ParamID::LINK, switchLink);
    attMonoSafe = std::make_unique<ButtonAtt>(proc.apvts, ParamID::MONO_SAFE, switchMonoSafe);
    attLimit = std::make_unique<ButtonAtt>(proc.apvts, ParamID::LIMIT, switchLimit);
    attMixMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_MIX_MOD, swMixMod);
    attDelayMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_DELAY_MOD, swDelayMod);
    attWidthMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_WIDTH_MOD, swWidthMod);
    attFbMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_FB_MOD, swFbMod);
    attLfoSidechain = std::make_unique<ButtonAtt>(proc.apvts, ParamID::LFO_SIDECHAIN, swLfoSidechain);
    attRateMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_RATE_MOD, swRateMod);
    attDepthMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_DEPTH_MOD, swDepthMod);
    attMode = std::make_unique<ComboAtt>(proc.apvts, ParamID::PROC_MODE, modeCombo);
    attShape = std::make_unique<ComboAtt>(proc.apvts, ParamID::LFO_SHAPE, shapeCombo);
    attDetuneMod = std::make_unique<ButtonAtt>(proc.apvts, ParamID::TOPO_DETUNE_MOD, swDetuneMod);

    startTimerHz(30);
    updateTopologyButtons();
}

OddEbbChorusEditor::~OddEbbChorusEditor()
{
    setLookAndFeel(nullptr);
}

void OddEbbChorusEditor::styleTextButton(juce::TextButton& b)
{
    b.setColour(juce::TextButton::buttonColourId, OddEbbLAF::dark());
    b.setColour(juce::TextButton::buttonOnColourId, OddEbbLAF::teal());
    b.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    b.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
}

void OddEbbChorusEditor::updateTopologyButtons()
{
    const auto topo = (int)proc.apvts.getRawParameterValue(ParamID::TOPOLOGY)->load();
    topoWetDry.setToggleState(topo == 0, juce::dontSendNotification);
    topoPseudo.setToggleState(topo == 1, juce::dontSendNotification);
    topoTrue.setToggleState(topo == 2, juce::dontSendNotification);
}

void OddEbbChorusEditor::timerCallback()
{
    macroBar.setLevel(proc.sidechainLevel.get());
    scMacroBar.setLevel(proc.sidechainLevel.get());
    int busIndex = proc.sidechainBusIndex.load();
    if (busIndex >= 0)
        scLevelLabel.setText("Sidechain Level: BUS " + juce::String(busIndex), juce::dontSendNotification);
    else
        scLevelLabel.setText("Sidechain Level: INPUT", juce::dontSendNotification);
    updateTopologyButtons();
}

void OddEbbChorusEditor::paint(juce::Graphics& g)
{
    g.fillAll(OddEbbLAF::bg());
    auto top = getLocalBounds().removeFromTop(42).toFloat();
    g.setColour(juce::Colour(0xff111111));
    g.fillRoundedRectangle(top.reduced(4.0f, 3.0f), 4.0f);

    // Draw sidechain indicator dots to the right of switches
    g.setColour(juce::Colour(0xff222222));
    // Draw topology glyphs over the three topology buttons
    drawTopologyGlyph(g, topoWetDry.getBounds(), 0);
    drawTopologyGlyph(g, topoPseudo.getBounds(), 1);
    drawTopologyGlyph(g, topoTrue.getBounds(), 2);

    // Draw indicator dots aligned to the switches in topo section area
    const auto topoArea = secTopo.getBounds();
    int firstY = topoArea.getY() + 30 + 18 + 6; // aligned with first switch row
    g.setColour(juce::Colour(0xff222222));
    for (size_t i = 0; i < dots.size(); ++i)
        g.fillEllipse((float)dots[i].getX(), (float)dots[i].getY(), (float)dots[i].getWidth(), (float)dots[i].getHeight());
}

void OddEbbChorusEditor::resized()
{
    auto area = getLocalBounds().reduced(8);
    layoutTopBar(area.removeFromTop(40));
    area.removeFromTop(8);

    const int gap = 6;
    auto input = area.removeFromLeft(220);
    area.removeFromLeft(gap);
    auto procArea = area.removeFromLeft(250);
    area.removeFromLeft(gap);
    auto lfo = area.removeFromLeft(300);
    area.removeFromLeft(gap);
    auto topo = area.removeFromLeft(245);
    area.removeFromLeft(gap);
    auto space = area.removeFromLeft(245);
    area.removeFromLeft(gap);
    auto out = area;

    auto footerArea = area.removeFromBottom(34);

    layoutInputSection(input);
    layoutProcSection(procArea);
    layoutLFOSection(lfo);
    layoutTopoSection(topo);
    layoutSpaceSection(space);
    layoutOutputSection(out);

    // Center footer title horizontally and keep brand on the right
    const int centerX = footerArea.getCentreX();
    footerTitleLabel.setBounds(centerX - 220, footerArea.getY() + 2, 440, 12);
    footerBrandLabel.setBounds(footerArea.getRight() - 160, footerArea.getY() + 4, 150, 14);

    // Place model text in bottom-left of the input/sidechain section
    // find the input section bounds (layoutInputSection used 'input' region earlier)
    // we approximate by using left-most 220px area from overall content
    auto leftArea = getLocalBounds().reduced(8);
    leftArea.removeFromTop(40);
    auto inputArea = leftArea.removeFromLeft(220);
    footerModelLabel.setBounds(inputArea.getX() + 8, footerArea.getY() + 12, 200, 12);
}

void OddEbbChorusEditor::layoutTopBar(juce::Rectangle<int> area)
{
    titleLabel.setBounds(area.removeFromLeft(160));
    prevBtn.setBounds(area.removeFromLeft(36).reduced(2));
    nextBtn.setBounds(area.removeFromLeft(36).reduced(2));
    area.removeFromLeft(12);
    loadBtn.setBounds(area.removeFromLeft(64).reduced(2));
    saveBtn.setBounds(area.removeFromLeft(64).reduced(2));
    compareBtn.setBounds(area.removeFromLeft(88).reduced(2));
    infoBtn.setBounds(area.removeFromLeft(36).reduced(2));
    settingsBtn.setBounds(area.removeFromLeft(36).reduced(2));
    modeLabel.setBounds(getWidth() - 250, 8, 220, 24);
}

void OddEbbChorusEditor::layoutInputSection(juce::Rectangle<int> a)
{
    secInput.setBounds(a);
    auto inner = a.reduced(8, 24);
    knobInput.setBounds(inner.removeFromTop(130).reduced(22, 0));
    inner.removeFromTop(30);
    scLabel.setBounds(inner.removeFromTop(14));
    auto row = inner.removeFromTop(34);
    scIntLabel.setBounds(row.removeFromLeft(26));
    switchSidechain.setBounds(row.removeFromLeft(56).reduced(4, 4));
    scExtLabel.setBounds(row.removeFromLeft(26));
}

void OddEbbChorusEditor::layoutProcSection(juce::Rectangle<int> a)
{
    secProc.setBounds(a);
    auto inner = a.reduced(8, 24);
    modeGroupLabel.setBounds(inner.removeFromTop(14));
    inner.removeFromTop(4);
    modeCombo.setBounds(inner.removeFromTop(24));
    inner.removeFromTop(8);
    knobDetune.setBounds(inner.removeFromTop(84).reduced(8, 0));
    inner.removeFromTop(20);
    linkLabel.setBounds(inner.removeFromTop(14));
    auto row = inner.removeFromTop(34);
    linkOffLabel.setBounds(row.removeFromLeft(26));
    switchLink.setBounds(row.removeFromLeft(56).reduced(4, 4));
    linkOnLabel.setBounds(row.removeFromLeft(26));
}

void OddEbbChorusEditor::layoutLFOSection(juce::Rectangle<int> a)
{
    secLFO.setBounds(a);
    auto inner = a.reduced(8, 24);
    knobRate.setBounds(inner.removeFromTop(125).reduced(44, 0));
    auto row = inner.removeFromTop(110);
    knobDepth.setBounds(row.removeFromLeft(row.getWidth() / 2).reduced(8, 0));
    auto shapeArea = row.reduced(8, 0);
    shapeLabel.setBounds(shapeArea.removeFromBottom(14));
    shapeCombo.setBounds(shapeArea.removeFromBottom(24));
    inner.removeFromTop(8);
    macroLabel.setBounds(inner.removeFromTop(14));
    macroBar.setBounds(inner.removeFromTop(28).reduced(6, 4));
    scMacroBar.setBounds(inner.removeFromTop(20).reduced(6, 4));
    scLevelLabel.setBounds(inner.removeFromTop(16));
}

void OddEbbChorusEditor::layoutTopoSection(juce::Rectangle<int> a)
{
    secTopo.setBounds(a);
    auto inner = a.reduced(8, 24);
    auto row = inner.removeFromTop(30);
    auto oneThird = row.getWidth() / 3;
    topoWetDry.setBounds(row.removeFromLeft(oneThird).reduced(2, 0));
    topoPseudo.setBounds(row.removeFromLeft(oneThird).reduced(2, 0));
    topoTrue.setBounds(row.reduced(2, 0));
    auto labels = inner.removeFromTop(18);
    auto third = labels.getWidth() / 3;
    wetDryLabel.setBounds(labels.removeFromLeft(third));
    pseudoLabel.setBounds(labels.removeFromLeft(third));
    trueLabel.setBounds(labels);
    inner.removeFromTop(24);
    // Sidechain controls header
    sidechainControlsLabel.setBounds(inner.removeFromTop(18));
    inner.removeFromTop(6);
    auto m1 = inner.removeFromTop(28); mixModLabel.setBounds(m1.removeFromLeft(80)); swMixMod.setBounds(m1.removeFromLeft(50).reduced(0, 3)); // dot handled by separate decoration
    auto dot1 = juce::Rectangle<int>(m1.getRight() - 10, m1.getY() + 6, 8, 8);
    auto m2 = inner.removeFromTop(28); delayModLabel.setBounds(m2.removeFromLeft(80)); swDelayMod.setBounds(m2.removeFromLeft(50).reduced(0, 3)); auto dot2 = juce::Rectangle<int>(m2.getRight() - 10, m2.getY() + 6, 8, 8);
    auto m3 = inner.removeFromTop(28); widthModLabel.setBounds(m3.removeFromLeft(80)); swWidthMod.setBounds(m3.removeFromLeft(50).reduced(0, 3)); auto dot3 = juce::Rectangle<int>(m3.getRight() - 10, m3.getY() + 6, 8, 8);
    auto m4 = inner.removeFromTop(28); fbModLabel.setBounds(m4.removeFromLeft(80)); swFbMod.setBounds(m4.removeFromLeft(50).reduced(0, 3)); auto dot4 = juce::Rectangle<int>(m4.getRight() - 10, m4.getY() + 6, 8, 8);

    // Store dots for painting by setting label positions for small circles - we will draw them in paint()
    // Keep dot positions in a small local vector stored in the component for painting
    dots.clear();
    dots.push_back(dot1); dots.push_back(dot2); dots.push_back(dot3); dots.push_back(dot4);

    // add rows for RATE, DEPTH, DETUNE, and LFO-SC sidechain row
    auto m5 = inner.removeFromTop(28); rateModLabel.setBounds(m5.removeFromLeft(80)); swRateMod.setBounds(m5.removeFromLeft(50).reduced(0, 3)); auto dot5 = juce::Rectangle<int>(m5.getRight() - 10, m5.getY() + 6, 8, 8);
    auto m6 = inner.removeFromTop(28); depthModLabel.setBounds(m6.removeFromLeft(80)); swDepthMod.setBounds(m6.removeFromLeft(50).reduced(0, 3)); auto dot6 = juce::Rectangle<int>(m6.getRight() - 10, m6.getY() + 6, 8, 8);
    auto m7 = inner.removeFromTop(28); detuneLabel.setBounds(m7.removeFromLeft(80)); swDetuneMod.setBounds(m7.removeFromLeft(50).reduced(0, 3)); auto dot7 = juce::Rectangle<int>(m7.getRight() - 10, m7.getY() + 6, 8, 8);
    auto m8 = inner.removeFromTop(28); lfoSidechainLabel.setBounds(m8.removeFromLeft(80)); swLfoSidechain.setBounds(m8.removeFromLeft(50).reduced(0, 3)); auto dot8 = juce::Rectangle<int>(m8.getRight() - 10, m8.getY() + 6, 8, 8);
    dots.push_back(dot5); dots.push_back(dot6); dots.push_back(dot7); dots.push_back(dot8);
}



void OddEbbChorusEditor::layoutSpaceSection(juce::Rectangle<int> a)
{
    secSpace.setBounds(a);
    auto inner = a.reduced(8, 24);
    auto row = inner.removeFromTop(120);
    knobDelay.setBounds(row.removeFromLeft(row.getWidth() / 2).reduced(8, 0));
    knobWidth.setBounds(row.reduced(8, 0));
    knobFeedback.setBounds(inner.removeFromTop(120).reduced(34, 0));
    monoSafeLabel.setBounds(inner.removeFromTop(14));
    auto sw = inner.removeFromTop(34);
    msOffLabel.setBounds(sw.removeFromLeft(26));
    switchMonoSafe.setBounds(sw.removeFromLeft(56).reduced(4, 4));
    msOnLabel.setBounds(sw.removeFromLeft(26));
}

void OddEbbChorusEditor::layoutOutputSection(juce::Rectangle<int> a)
{
    secOut.setBounds(a);
    auto inner = a.reduced(8, 24);
    knobMix.setBounds(inner.removeFromTop(120).reduced(16, 0));
    knobOut.setBounds(inner.removeFromTop(120).reduced(16, 0));
    limitLabel.setBounds(inner.removeFromTop(14));
    auto sw = inner.removeFromTop(34);
    limOffLabel.setBounds(sw.removeFromLeft(26));
    switchLimit.setBounds(sw.removeFromLeft(56).reduced(4, 4));
    limOnLabel.setBounds(sw.removeFromLeft(26));
}