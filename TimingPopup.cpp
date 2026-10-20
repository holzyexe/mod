#include "TimingPopup.hpp"
#include "Timing.hpp"
#include <fmt/format.h>

using namespace geode::prelude;

TimingPopup* TimingPopup::create() {
    auto ret = new TimingPopup();
    if (ret && ret->initAnchored(320.f, 240.f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool TimingPopup::setup() {
    this->setTitle("Timing Tweaker");
    auto size = m_mainLayer->getContentSize();
    float cx = size.width / 2;

    // Enable toggle
    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(TimingPopup::onToggle), 0.7f);
    toggler->toggle(timing::enabled());
    toggler->setPosition({ 40.f, size.height - 50.f });
    m_buttonMenu->addChild(toggler);
    auto enLabel = CCLabelBMFont::create("Enabled", "bigFont.fnt");
    enLabel->setScale(0.45f);
    enLabel->setAnchorPoint({ 0.f, 0.5f });
    enLabel->setPosition({ 62.f, size.height - 50.f });
    m_mainLayer->addChild(enLabel);

    // Big TPS readout
    m_tpsLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_tpsLabel->setScale(0.9f);
    m_tpsLabel->setPosition({ cx, size.height - 85.f });
    m_mainLayer->addChild(m_tpsLabel);

    m_msLabel = CCLabelBMFont::create("", "chatFont.fnt");
    m_msLabel->setScale(0.7f);
    m_msLabel->setOpacity(180);
    m_msLabel->setPosition({ cx, size.height - 108.f });
    m_mainLayer->addChild(m_msLabel);

    // Slider
    m_slider = Slider::create(this, menu_selector(TimingPopup::onSlider), 0.8f);
    m_slider->setPosition({ cx, size.height - 135.f });
    m_mainLayer->addChild(m_slider);

    // Presets
    auto presetMenu = CCMenu::create();
    presetMenu->setPosition({ cx, size.height - 172.f });
    presetMenu->setContentSize({ 280.f, 30.f });
    presetMenu->setLayout(RowLayout::create()->setGap(8.f));
    for (int p : { 60, 120, 240, 480 }) {
        auto spr = ButtonSprite::create(std::to_string(p).c_str(), 40, true, "bigFont.fnt", "GJ_button_04.png", 28.f, 0.6f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(TimingPopup::onPreset));
        btn->setTag(p);
        presetMenu->addChild(btn);
    }
    presetMenu->updateLayout();
    m_mainLayer->addChild(presetMenu);

    // Exact value input + apply
    m_input = TextInput::create(110.f, "TPS", "bigFont.fnt");
    m_input->setFilter("0123456789.");
    m_input->setMaxCharCount(7);
    m_input->setPosition({ cx - 40.f, 30.f });
    m_mainLayer->addChild(m_input);

    auto applySpr = ButtonSprite::create("Apply", 60, true, "goldFont.fnt", "GJ_button_01.png", 28.f, 0.7f);
    auto applyBtn = CCMenuItemSpriteExtra::create(applySpr, this, menu_selector(TimingPopup::onApply));
    m_buttonMenu->addChild(applyBtn);
    // m_buttonMenu is anchored at the layer origin, so position in layer coordinates
    applyBtn->setPosition({ cx + 55.f, 30.f });

    this->refresh(true);
    return true;
}

void TimingPopup::refresh(bool updateSlider) {
    float tps = timing::tps();
    m_tpsLabel->setString(fmt::format("{:.0f} TPS", tps).c_str());
    m_msLabel->setString(fmt::format("frame time: {:.3f} ms  (240 TPS = 4.167 ms)", 1000.f / tps).c_str());
    if (updateSlider) {
        m_slider->setValue((tps - timing::MIN_TPS) / (timing::MAX_TPS - timing::MIN_TPS));
    }
}

void TimingPopup::onSlider(CCObject* sender) {
    float v = static_cast<SliderThumb*>(sender)->getValue();
    timing::setTps(std::round(timing::MIN_TPS + v * (timing::MAX_TPS - timing::MIN_TPS)));
    this->refresh(false);
}

void TimingPopup::onPreset(CCObject* sender) {
    timing::setTps(static_cast<float>(static_cast<CCNode*>(sender)->getTag()));
    this->refresh(true);
}

void TimingPopup::onApply(CCObject*) {
    auto parsed = geode::utils::numFromString<float>(m_input->getString());
    if (parsed.isOk() && parsed.unwrap() > 0.f) {
        timing::setTps(parsed.unwrap());
        this->refresh(true);
    } else {
        FLAlertLayer::create("Invalid", "Enter a number between 30 and 960.", "OK")->show();
    }
}

void TimingPopup::onToggle(CCObject*) {
    // toggler state flips after the callback, so invert the stored value
    timing::setEnabled(!timing::enabled());
}
