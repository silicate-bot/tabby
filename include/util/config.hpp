#pragma once
#ifndef TABBY_UTIL_CONFIG
#define TABBY_UTIL_CONFIG

#include "util/defines.hpp"

TABBY_NS_BEGIN

class TabbyGlobalCfg {
public:
    static TabbyGlobalCfg& get() {
        static TabbyGlobalCfg instance;
        return instance;
    }

    double animationSpeed = 1.0;
    bool playAnimations = true;
    float uiScale = 1.0f;

    double widgetWidth = 0.0;

    TabbyGlobalCfg(const TabbyGlobalCfg&) = delete;
    TabbyGlobalCfg& operator=(const TabbyGlobalCfg&) = delete;

private:
    TabbyGlobalCfg() = default;
    ~TabbyGlobalCfg() = default;
};


TABBY_NS_END

#endif