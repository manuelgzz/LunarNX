#pragma once

#include "../platform/native_touch.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <vector>

namespace lunar::ps {

// Shared by Linux and Android; independent of SDL and the protocol transport.
class NativeTouchpad {
public:
    using Clock = std::chrono::steady_clock;
    struct Point {
        bool active = false;
        uint64_t id = 0;
        uint32_t x = 0, y = 0, down_x = 0, down_y = 0;
    };
    enum class Gesture { None, Touch, Pan, Tap, LongPress };
    struct State {
        std::array<Point, 2> points{};
        bool pressed = false;
        Gesture gesture = Gesture::None;
    };

    State update(const std::vector<platform::NativeTouch>& input, bool suppressed,
                 Clock::time_point now) {
        if (suppressed) {
            reset();
            blocked_ = !input.empty();
            return {};
        }
        if (blocked_) {
            if (input.empty()) blocked_ = false;
            return {};
        }
        if (input.empty()) {
            if (active_) {
                const bool tap = !multiple_ && distance_ <= 100 &&
                    now - started_ <= std::chrono::milliseconds(200);
                active_ = false;
                points_ = {};
                if (tap) click_until_ = now + std::chrono::milliseconds(150);
            }
            // A click pulse never retains lifted contacts.
            State state;
            state.pressed = now < click_until_;
            state.gesture = state.pressed ? Gesture::Tap : Gesture::None;
            return state;
        }
        if (!active_) {
            // Reserve the same rightmost 96 UI pixels as the stream menu.
            // Physical controller touchpads have no menu edge reservation.
            if (input.front().touchscreen && input.front().x >= 1280 - 96) {
                blocked_ = true;
                return {};
            }
            active_ = true;
            started_ = now;
            distance_ = 0;
            multiple_ = false;
            long_press_ = false;
            click_until_ = {};
        }
        multiple_ |= input.size() > 1;
        // Release old slots before assigning new fingers, keeping the other
        // finger's protocol slot stable when one contact is replaced.
        std::array<bool, 2> released{};
        for (size_t i = 0; i < points_.size(); ++i) {
            auto& point = points_[i];
            if (point.active && std::none_of(input.begin(), input.end(),
                    [&](const auto& touch) { return touch.finger_id == point.id; })) {
                point = {};
                released[i] = true;
            }
        }
        for (const auto& touch : input) {
            auto slot = std::find_if(points_.begin(), points_.end(),
                [&](const auto& point) { return point.active && point.id == touch.finger_id; });
            if (slot == points_.end()) {
                // Publish a release before reusing a slot so Chiaki assigns
                // the replacement a fresh contact ID.
                slot = points_.end();
                for (size_t i = 0; i < points_.size(); ++i) {
                    if (!points_[i].active && !released[i]) {
                        slot = points_.begin() + i;
                        break;
                    }
                }
                if (slot == points_.end()) continue;
                *slot = {true, touch.finger_id, 0, 0,
                         std::min(touch.x, 1279u), std::min(touch.y, 719u)};
            }
            slot->x = std::min(touch.x, 1279u);
            slot->y = std::min(touch.y, 719u);
            const int64_t dx = int64_t(slot->x) - slot->down_x;
            const int64_t dy = int64_t(slot->y) - slot->down_y;
            distance_ = std::max(distance_, uint64_t(dx * dx + dy * dy));
        }
        if (!multiple_ && distance_ <= 100 &&
            now - started_ >= std::chrono::milliseconds(500)) long_press_ = true;
        State state;
        state.points = points_;
        state.pressed = long_press_ && !multiple_;
        state.gesture = state.pressed ? Gesture::LongPress :
            distance_ >= 900 ? Gesture::Pan : Gesture::Touch;
        return state;
    }

    void reset() { *this = NativeTouchpad{}; }

private:
    std::array<Point, 2> points_{};
    bool active_ = false, blocked_ = false, multiple_ = false, long_press_ = false;
    uint64_t distance_ = 0;
    Clock::time_point started_{}, click_until_{};
};
} // namespace lunar::ps
