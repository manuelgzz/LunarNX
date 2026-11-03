#pragma once

#include "gamepad_reader.h"
#include "../platform/native_touch.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace lunar::input {

// Geometry and finger ownership shared by the renderer and native input path.
// Coordinates use the same 1280x720 space as Borealis and the PS touchpad.
class TouchGamepad {
public:
    enum class Control {
        A, B, X, Y, Up, Down, Left, Right, Lb, Rb, Lt, Rt,
        L3, R3, View, Menu, Guide, TouchClick, LeftStick, RightStick,
        Touchpad, AppMenu, Ignore,
    };
    struct Region {
        Control control;
        float x, y, width, height;
        bool contains(float px, float py) const {
            return px >= x && px < x + width && py >= y && py < y + height;
        }
    };
    struct Snapshot {
        bool enabled = false, playstation = false, suppressed = false;
        GamepadState gamepad{};
        std::vector<platform::NativeTouch> touchpad;
        std::vector<platform::NativeTouch> touchpad_screen;
        std::array<bool, 22> active{};
        bool menu_requested = false;
    };

    static const std::array<Region, 22>& regions() {
        static const std::array<Region, 22> value{{
            {Control::A, 1080, 385, 62, 62}, {Control::B, 1148, 317, 62, 62},
            {Control::X, 1012, 317, 62, 62}, {Control::Y, 1080, 249, 62, 62},
            {Control::Up, 145, 240, 58, 58}, {Control::Down, 145, 356, 58, 58},
            {Control::Left, 87, 298, 58, 58}, {Control::Right, 203, 298, 58, 58},
            {Control::Lb, 40, 158, 112, 54}, {Control::Rb, 1128, 158, 112, 54},
            {Control::Lt, 40, 92, 112, 54}, {Control::Rt, 1128, 92, 112, 54},
            {Control::L3, 294, 579, 70, 54}, {Control::R3, 1054, 579, 70, 54},
            {Control::View, 452, 330, 112, 52}, {Control::Menu, 716, 330, 112, 52},
            {Control::Guide, 594, 330, 92, 52}, {Control::TouchClick, 570, 275, 140, 44},
            {Control::LeftStick, 86, 450, 180, 180},
            {Control::RightStick, 850, 450, 180, 180},
            {Control::Touchpad, 440, 100, 400, 160},
            {Control::AppMenu, 1194, 18, 66, 46},
        }};
        return value;
    }

    void configure(bool enabled, bool playstation, bool suppressed) {
        if (enabled_ == enabled && ps_ == playstation && suppressed_ == suppressed) return;
        enabled_ = enabled; ps_ = playstation; suppressed_ = suppressed;
        captures_.clear();
        // Never reinterpret a finger held across a menu, focus or mode change.
        wait_for_release_ = true;
        snapshot_ = {};
    }

    Snapshot update(const std::vector<platform::NativeTouch>& contacts) {
        Snapshot out;
        out.enabled = enabled_; out.playstation = ps_; out.suppressed = suppressed_;
        if (contacts.empty()) wait_for_release_ = false;
        if (suppressed_ || wait_for_release_) {
            captures_.clear(); snapshot_ = out; return out;
        }
        captures_.erase(std::remove_if(captures_.begin(), captures_.end(), [&](const Capture& capture) {
            return std::none_of(contacts.begin(), contacts.end(), [&](const auto& touch) {
                return touch.finger_id == capture.id;
            });
        }), captures_.end());
        for (const auto& touch : contacts) {
            auto captured = std::find_if(captures_.begin(), captures_.end(), [&](const Capture& c) {
                return c.id == touch.finger_id;
            });
            if (captured == captures_.end()) {
                const auto control = hit(touch.x, touch.y);
                captures_.push_back({touch.finger_id, control});
                captured = captures_.end() - 1;
                if (control == Control::AppMenu) out.menu_requested = true;
            }
            const auto control = captured->control;
            if (control == Control::AppMenu && regionFor(control).contains(touch.x, touch.y))
                out.active[static_cast<size_t>(control)] = true;
            if (!enabled_) {
                if (control != Control::AppMenu) out.touchpad.push_back(touch);
                continue;
            }
            if (control == Control::Ignore || control == Control::AppMenu) continue;
            const auto& region = regionFor(control);
            if (control == Control::Touchpad) {
                out.active[static_cast<size_t>(control)] = true;
                out.touchpad_screen.push_back(touch);
                // Already routed to the dedicated pad: bypass the fullscreen
                // menu-edge reservation after converting into PS coordinates.
                out.touchpad.push_back({touch.finger_id,
                    scale(touch.x, region.x, region.width, 1279),
                    scale(touch.y, region.y, region.height, 719), false});
            } else if (control == Control::LeftStick || control == Control::RightStick) {
                out.active[static_cast<size_t>(control)] = true;
                float dx = (float(touch.x) - region.x - region.width / 2) / 65;
                float dy = (region.y + region.height / 2 - float(touch.y)) / 65;
                const float distance = std::sqrt(dx * dx + dy * dy);
                if (distance > 1) { dx /= distance; dy /= distance; }
                if (distance < 0.12f) dx = dy = 0;
                auto& x = control == Control::LeftStick ? out.gamepad.left_stick_x : out.gamepad.right_stick_x;
                auto& y = control == Control::LeftStick ? out.gamepad.left_stick_y : out.gamepad.right_stick_y;
                x = static_cast<int16_t>(std::clamp(dx, -1.0f, 1.0f) * 32767);
                y = static_cast<int16_t>(std::clamp(dy, -1.0f, 1.0f) * 32767);
            } else if (isDpad(control)) {
                // D-pad capture permits sliding to a neighbour or diagonal,
                // while retaining ownership so it cannot enter the PS pad.
                if (touch.x < 67 || touch.x >= 281 || touch.y < 220 || touch.y >= 434) continue;
                const float dx = float(touch.x) - 174, dy = float(touch.y) - 327;
                out.gamepad.dpad_left |= dx < -20;
                out.gamepad.dpad_right |= dx > 20;
                out.gamepad.dpad_up |= dy < -20;
                out.gamepad.dpad_down |= dy > 20;
            } else if (region.contains(touch.x, touch.y)) {
                out.active[static_cast<size_t>(control)] = true;
                press(out.gamepad, control);
            }
        }
        out.active[static_cast<size_t>(Control::Up)] = out.gamepad.dpad_up;
        out.active[static_cast<size_t>(Control::Down)] = out.gamepad.dpad_down;
        out.active[static_cast<size_t>(Control::Left)] = out.gamepad.dpad_left;
        out.active[static_cast<size_t>(Control::Right)] = out.gamepad.dpad_right;
        snapshot_ = out;
        return out;
    }

    const Snapshot& snapshot() const { return snapshot_; }
    bool enabled() const { return enabled_; }
    static const Region& regionFor(Control control) {
        const auto& values = regions();
        const auto it = std::find_if(values.begin(), values.end(), [&](const Region& r) {
            return r.control == control;
        });
        return it == values.end() ? values.back() : *it;
    }
    static GamepadState merge(GamepadState physical, const GamepadState& touch) {
#define MERGE_BUTTON(name) physical.name = physical.name || touch.name
        MERGE_BUTTON(a); MERGE_BUTTON(b); MERGE_BUTTON(x); MERGE_BUTTON(y);
        MERGE_BUTTON(dpad_up); MERGE_BUTTON(dpad_down); MERGE_BUTTON(dpad_left); MERGE_BUTTON(dpad_right);
        MERGE_BUTTON(lb); MERGE_BUTTON(rb); MERGE_BUTTON(lt); MERGE_BUTTON(rt);
        MERGE_BUTTON(l3); MERGE_BUTTON(r3); MERGE_BUTTON(view); MERGE_BUTTON(menu);
        MERGE_BUTTON(guide); MERGE_BUTTON(touchpad);
#undef MERGE_BUTTON
        if (touch.left_stick_x || touch.left_stick_y) {
            physical.left_stick_x = touch.left_stick_x; physical.left_stick_y = touch.left_stick_y;
        }
        if (touch.right_stick_x || touch.right_stick_y) {
            physical.right_stick_x = touch.right_stick_x; physical.right_stick_y = touch.right_stick_y;
        }
        physical.left_trigger = std::max(physical.left_trigger, touch.left_trigger);
        physical.right_trigger = std::max(physical.right_trigger, touch.right_trigger);
        return physical;
    }

private:
    struct Capture { uint64_t id; Control control; };
    std::vector<Capture> captures_;
    bool enabled_ = false, ps_ = false, suppressed_ = false, wait_for_release_ = false;
    Snapshot snapshot_{};
    static bool isDpad(Control c) {
        return c == Control::Up || c == Control::Down || c == Control::Left || c == Control::Right;
    }
    Control hit(uint32_t x, uint32_t y) const {
        for (const auto& r : regions()) {
            if (r.control != Control::AppMenu && !enabled_) continue;
            if (!ps_ && (r.control == Control::Touchpad || r.control == Control::TouchClick)) continue;
            // Only one finger owns a stick; a second must not overwrite its axis.
            if ((r.control == Control::LeftStick || r.control == Control::RightStick) &&
                std::any_of(captures_.begin(), captures_.end(), [&](const auto& c) { return c.control == r.control; }))
                continue;
            if (r.contains(x, y)) return r.control;
        }
        if (enabled_ && x >= 87 && x < 261 && y >= 240 && y < 414) return Control::Up;
        return Control::Ignore;
    }
    static uint32_t scale(uint32_t value, float start, float extent, uint32_t maximum) {
        return static_cast<uint32_t>(std::clamp((float(value) - start) / (extent - 1), 0.0f, 1.0f) * maximum);
    }
    static void press(GamepadState& state, Control c) {
        switch (c) {
            case Control::A: state.a = true; break; case Control::B: state.b = true; break;
            case Control::X: state.x = true; break; case Control::Y: state.y = true; break;
            case Control::Lb: state.lb = true; break; case Control::Rb: state.rb = true; break;
            case Control::Lt: state.lt = true; state.left_trigger = 65535; break;
            case Control::Rt: state.rt = true; state.right_trigger = 65535; break;
            case Control::L3: state.l3 = true; break; case Control::R3: state.r3 = true; break;
            case Control::View: state.view = true; break; case Control::Menu: state.menu = true; break;
            case Control::Guide: state.guide = true; break;
            case Control::TouchClick: state.touchpad = true; break;
            default: break;
        }
    }
};
} // namespace lunar::input
