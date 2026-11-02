#pragma once

#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)

#include "ps_input_mapper.h"
#ifdef __SWITCH__
#include <switch.h>
#else
#include "../platform/native_gamepad.h"
#include "native_touchpad.h"
#endif
#include <array>
#include <chrono>
#include <cstdint>

namespace lunar::ps {

enum class PsTouchpadGesture : uint8_t {
    None,
    Touch,
    Tap,
    Pan,
    LongPress,
};

struct PsTouchpadFeedbackPoint {
    bool active = false;
    uint16_t screen_x = 0;
    uint16_t screen_y = 0;
};

struct PsTouchpadFeedback {
    PsTouchpadGesture gesture = PsTouchpadGesture::None;
    std::array<PsTouchpadFeedbackPoint, CHIAKI_CONTROLLER_TOUCHES_MAX> points{};
};

class PsTouchpadReader {
public:
    explicit PsTouchpadReader(bool ps5);

    PsTouchpadState read(bool suppressed);
    PsTouchpadFeedback feedback() const;
    void reset();

private:
#ifndef __SWITCH__
    NativeTouchpad native_touchpad_;
    PsTouchpadFeedback native_feedback_{};
#endif
    enum class GestureState : uint8_t {
        Idle,
        Pending,
        Pan,
        LongPress,
        ReleaseHold,
    };

    PsTouchpadState currentState(bool pressed) const;
#ifdef __SWITCH__
    using TouchSample = HidTouchState;
    using TouchSnapshot = HidTouchScreenState;
#else
    using TouchSample = platform::NativeTouch;
    struct TouchSnapshot {
        size_t count = 0;
        std::array<TouchSample, CHIAKI_CONTROLLER_TOUCHES_MAX> touches{};
    };
#endif
    void updateTrackedTouches(const TouchSnapshot& state);
    size_t activeTouchCount() const;

    struct TrackedTouch {
        bool active = false;
        uint64_t finger_id = 0;
        uint32_t down_x = 0;
        uint32_t down_y = 0;
        uint16_t screen_x = 0;
        uint16_t screen_y = 0;
        uint16_t x = 0;
        uint16_t y = 0;
    };

    bool ps5_ = false;
    bool blocked_until_release_ = false;
    uint64_t blocked_finger_id_ = 0;
    GestureState gesture_ = GestureState::Idle;
    std::array<TrackedTouch, CHIAKI_CONTROLLER_TOUCHES_MAX> touches_{};
    uint64_t primary_finger_id_ = 0;
    uint32_t max_distance_squared_ = 0;
    bool had_multiple_touches_ = false;
    bool release_was_long_press_ = false;
    std::chrono::steady_clock::time_point gesture_started_{};
    std::chrono::steady_clock::time_point release_hold_until_{};
};

} // namespace lunar::ps

#endif
