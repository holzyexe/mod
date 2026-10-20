#pragma once
#include <Geode/Geode.hpp>

class TimingPopup : public geode::Popup<> {
protected:
    cocos2d::CCLabelBMFont* m_tpsLabel = nullptr;
    cocos2d::CCLabelBMFont* m_msLabel = nullptr;
    Slider* m_slider = nullptr;
    geode::TextInput* m_input = nullptr;

    bool setup() override;
    void refresh(bool updateSlider);
    void onSlider(cocos2d::CCObject* sender);
    void onPreset(cocos2d::CCObject* sender);
    void onApply(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);

public:
    static TimingPopup* create();
};
