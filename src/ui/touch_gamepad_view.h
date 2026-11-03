#pragma once
#if defined(LUNARNX_NATIVE_UI) && !defined(__SWITCH__)
#include <borealis.hpp>
#include "../platform/native_gamepad.h"
#include <functional>
#include <utility>

namespace lunar::ui {

class TouchGamepadView : public brls::View {
public:
    explicit TouchGamepadView(std::function<void()> menu) : menu_(std::move(menu)) {}
    void setControlOpacity(int percent) { opacity_ = std::clamp(percent, 10, 100) / 100.0f; }

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style, brls::FrameContext*) override {
        using Pad = input::TouchGamepad;
        using C = Pad::Control;
        const auto state = platform::nativeTouchGamepadSnapshot();
        if (state.suppressed) return;
        if (platform::consumeNativeTouchMenuRequest()) brls::sync(menu_);
        nvgSave(vg);
        nvgTranslate(vg, x, y);
        nvgScale(vg, width / 1280, height / 720);
        for (const auto& r : Pad::regions()) {
            if (!state.enabled && r.control != C::AppMenu) continue;
            if (!state.playstation && (r.control == C::Touchpad || r.control == C::TouchClick)) continue;
            nvgGlobalAlpha(vg, r.control == C::AppMenu ? 0.85f : opacity_);
            const bool pressed = state.active[static_cast<size_t>(r.control)];
            nvgBeginPath(vg);
            const bool stick = r.control == C::LeftStick || r.control == C::RightStick;
            const bool face = r.control == C::A || r.control == C::B || r.control == C::X || r.control == C::Y;
            if (stick || face)
                nvgCircle(vg, r.x + r.width / 2, r.y + r.height / 2, r.width / 2 - 2);
            else nvgRoundedRect(vg, r.x, r.y, r.width, r.height, 10);
            nvgFillColor(vg, pressed ? nvgRGBA(100, 140, 185, 155) : nvgRGBA(20, 27, 38, 120));
            nvgFill(vg);
            nvgStrokeWidth(vg, pressed ? 3 : 2);
            nvgStrokeColor(vg, nvgRGBA(235, 240, 250, 200)); nvgStroke(vg);
            if (stick) {
                const float sx = (r.control == C::LeftStick ? state.gamepad.left_stick_x : state.gamepad.right_stick_x) / 32767.0f;
                const float sy = (r.control == C::LeftStick ? state.gamepad.left_stick_y : state.gamepad.right_stick_y) / 32767.0f;
                nvgBeginPath(vg);
                nvgCircle(vg, r.x + r.width / 2 + sx * 55, r.y + r.height / 2 - sy * 55, 33);
                nvgFillColor(vg, nvgRGBA(225, 235, 250, 140)); nvgFill(vg);
                continue;
            }
            if (r.control == C::Touchpad) {
                for (const auto& touch : state.touchpad_screen) {
                    nvgBeginPath(vg);
                    nvgCircle(vg, std::clamp(float(touch.x), r.x + 8, r.x + r.width - 8),
                        std::clamp(float(touch.y), r.y + 8, r.y + r.height - 8), 8);
                    nvgFillColor(vg, nvgRGBA(235, 255, 245, 230)); nvgFill(vg);
                }
            }
            if (face && state.playstation) {
                const float cx = r.x + r.width / 2, cy = r.y + r.height / 2;
                nvgBeginPath(vg); nvgStrokeWidth(vg, 3);
                if (r.control == C::A) {
                    nvgMoveTo(vg, cx - 10, cy - 10); nvgLineTo(vg, cx + 10, cy + 10);
                    nvgMoveTo(vg, cx - 10, cy + 10); nvgLineTo(vg, cx + 10, cy - 10);
                } else if (r.control == C::B) nvgCircle(vg, cx, cy, 13);
                else if (r.control == C::X) nvgRect(vg, cx - 12, cy - 12, 24, 24);
                else {
                    nvgMoveTo(vg, cx, cy - 14); nvgLineTo(vg, cx + 14, cy + 12);
                    nvgLineTo(vg, cx - 14, cy + 12); nvgClosePath(vg);
                }
                nvgStroke(vg);
                continue;
            }
            std::string label;
            switch (r.control) {
                case C::A: label = state.playstation ? "X" : "A"; break;
                case C::B: label = state.playstation ? "O" : "B"; break;
                case C::X: label = state.playstation ? "[]" : "X"; break;
                case C::Y: label = state.playstation ? "/\\" : "Y"; break;
                case C::Up: label = "^"; break; case C::Down: label = "v"; break;
                case C::Left: label = "<"; break; case C::Right: label = ">"; break;
                case C::Lb: label = state.playstation ? "L1" : "LB"; break;
                case C::Rb: label = state.playstation ? "R1" : "RB"; break;
                case C::Lt: label = state.playstation ? "L2" : "LT"; break;
                case C::Rt: label = state.playstation ? "R2" : "RT"; break;
                case C::L3: label = state.playstation ? "L3" : "LS"; break;
                case C::R3: label = state.playstation ? "R3" : "RS"; break;
                case C::View: label = state.playstation ? "Share" : "View"; break;
                case C::Menu: label = state.playstation ? "Options" : "Menu"; break;
                case C::Guide: label = state.playstation ? "PS" : "Xbox"; break;
                case C::TouchClick: label = brls::getStr("lunarnx/settings/touchpad_click"); break;
                case C::Touchpad: label = brls::getStr("lunarnx/settings/touchpad_area"); break;
                case C::AppMenu: label = "..."; break;
                default: break;
            }
            nvgFontFaceId(vg, brls::Application::getDefaultFont());
            nvgFontSize(vg, r.control == C::TouchClick ? 16 : 21);
            nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
            nvgText(vg, r.x + r.width / 2, r.y + r.height / 2, label.c_str(), nullptr);
        }
        nvgRestore(vg);
    }
private:
    float opacity_ = 0.5f;
    std::function<void()> menu_;
};
} // namespace lunar::ui
#endif
