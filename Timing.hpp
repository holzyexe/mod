#pragma once
#include <Geode/Geode.hpp>
#include <algorithm>

namespace timing {
    constexpr float MIN_TPS = 30.f;
    constexpr float MAX_TPS = 960.f;

    inline bool enabled() {
        return geode::Mod::get()->getSavedValue<bool>("enabled", false);
    }
    inline void setEnabled(bool v) {
        geode::Mod::get()->setSavedValue<bool>("enabled", v);
    }
    inline float tps() {
        return std::clamp(geode::Mod::get()->getSavedValue<float>("tps", 240.f), MIN_TPS, MAX_TPS);
    }
    inline void setTps(float v) {
        geode::Mod::get()->setSavedValue<float>("tps", std::clamp(v, MIN_TPS, MAX_TPS));
    }
}
