#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "Timing.hpp"
#include "TimingPopup.hpp"

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// Physics step hook.
// GD 2.2 runs physics in fixed substeps of 1/240s. getModifiedDelta() decides
// how much simulated time to advance this frame. We replace the 1/240 step
// with 1/tps and keep the leftover time in m_extraDelta.
//
// NOTE: written against my best understanding of the 2.2 bindings. If your
// Geode version has a different member name/signature, adjust here.
// ---------------------------------------------------------------------------
class $modify(TTBaseGameLayer, GJBaseGameLayer) {
    float getModifiedDelta(float dt) {
        if (!timing::enabled()) {
            return GJBaseGameLayer::getModifiedDelta(dt);
        }

        float warp = std::max(m_gameState.m_timeWarp, 0.f);
        float step = (1.f / timing::tps()) * warp;
        if (step <= 0.f) return 0.f;

        float total = dt + m_extraDelta;
        int steps = static_cast<int>(std::floor(total / step));
        m_extraDelta = total - steps * step;
        return steps * step;
    }
};

// ---------------------------------------------------------------------------
// "TPS" button on the pause menu opens the GUI.
// ---------------------------------------------------------------------------
class $modify(TTPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto spr = ButtonSprite::create("TPS", 40, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(TTPauseLayer::onTiming));

        if (auto menu = this->getChildByID("right-button-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        } else {
            auto menu = CCMenu::create();
            menu->addChild(btn);
            menu->setPosition({ CCDirector::get()->getWinSize().width - 40.f, 40.f });
            this->addChild(menu);
        }
    }

    void onTiming(CCObject*) {
        TimingPopup::create()->show();
    }
};
