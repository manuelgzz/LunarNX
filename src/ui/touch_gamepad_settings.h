#pragma once
#if defined(LUNARNX_NATIVE_UI) && !defined(__SWITCH__)
#include <borealis.hpp>
#include <algorithm>
#include <cstdlib>

namespace lunar::ui {
template<class Settings>
void addTouchGamepadSettings(brls::Box* card, Settings& settings) {
    auto* enabled = new brls::BooleanCell();
    enabled->init(brls::getStr("lunarnx/settings/touch_gamepad"),
        settings.touch_gamepad_enabled,
        [&settings](bool value) { settings.touch_gamepad_enabled = value; });
    card->addView(enabled);
    static constexpr int values[] = {10, 25, 50, 75, 100};
    int selected = 0;
    for (int i = 1; i < 5; ++i)
        if (std::abs(settings.touch_gamepad_opacity - values[i]) <
            std::abs(settings.touch_gamepad_opacity - values[selected])) selected = i;
    auto* opacity = new brls::SelectorCell();
    opacity->init(brls::getStr("lunarnx/settings/touch_gamepad_opacity"),
        {"10%", "25%", "50%", "75%", "100%"}, selected,
        [&settings](int selected) { settings.touch_gamepad_opacity = values[std::clamp(selected, 0, 4)]; });
    card->addView(opacity);
}
} // namespace lunar::ui
#endif
