#include "input/touch_gamepad.h"
#include "ps/native_touchpad.h"
#include <cassert>
#include <iostream>
using Pad = lunar::input::TouchGamepad;
using C = Pad::Control;
using Touch = lunar::platform::NativeTouch;
Touch at(C control, uint64_t id = 1) {
    const auto& r = Pad::regionFor(control);
    return {id, uint32_t(r.x + r.width / 2), uint32_t(r.y + r.height / 2), true};
}
int main() {
    Pad pad;
    pad.configure(true, true, false); pad.update({});
    for (auto pair : {std::pair{C::A, &lunar::input::GamepadState::a},
            {C::B, &lunar::input::GamepadState::b}, {C::X, &lunar::input::GamepadState::x},
            {C::Y, &lunar::input::GamepadState::y}, {C::Lb, &lunar::input::GamepadState::lb},
            {C::Rb, &lunar::input::GamepadState::rb}, {C::L3, &lunar::input::GamepadState::l3},
            {C::R3, &lunar::input::GamepadState::r3}, {C::View, &lunar::input::GamepadState::view},
            {C::Menu, &lunar::input::GamepadState::menu}, {C::Guide, &lunar::input::GamepadState::guide},
            {C::TouchClick, &lunar::input::GamepadState::touchpad}}) {
        auto state = pad.update({at(pair.first)});
        assert(state.gamepad.*(pair.second)); assert(state.touchpad.empty());
        assert(!(pad.update({}).gamepad.*(pair.second)));
    }
    for (auto control : {C::Up, C::Down, C::Left, C::Right}) {
        const auto state = pad.update({at(control)}).gamepad;
        assert(control != C::Up || state.dpad_up); assert(control != C::Down || state.dpad_down);
        assert(control != C::Left || state.dpad_left); assert(control != C::Right || state.dpad_right);
        pad.update({});
    }
    pad.update({at(C::Right)});
    auto escaped = pad.update({at(C::Touchpad)});
    assert(!escaped.gamepad.dpad_right && escaped.touchpad.empty()); pad.update({});
    auto left = at(C::LeftStick, 10); left.x += 65;
    auto right = at(C::RightStick, 20); right.y -= 65;
    auto pad1 = at(C::Touchpad, 50), pad2 = at(C::Touchpad, 60); pad2.x += 100;
    auto state = pad.update({left, right, at(C::Rt, 30), at(C::A, 40), pad1, pad2});
    assert(state.gamepad.left_stick_x == 32767 && state.gamepad.right_stick_y == 32767);
    assert(state.gamepad.a && state.gamepad.rt && state.gamepad.right_trigger == 65535);
    assert(state.touchpad.size() == 2 && !state.touchpad[0].touchscreen);
    // Sticks and buttons never become touchpad contacts, even when dragged into it.
    left.x = pad1.x; left.y = pad1.y;
    state = pad.update({left, pad1});
    assert(state.touchpad.size() == 1 && state.touchpad[0].finger_id == pad1.finger_id);
    assert(!state.gamepad.a && !state.gamepad.rt);
    pad.update({});
    auto finger = at(C::A);
    pad.update({finger}); finger = at(C::Touchpad);
    state = pad.update({finger}); assert(state.touchpad.empty() && !state.gamepad.a);
    pad.update({});
    // A finger starting on video remains video-owned until release.
    pad.update({{1, 640, 600, true}});
    assert(pad.update({at(C::Touchpad)}).touchpad.empty()); pad.update({});
    // Captured pad movement clamps to its perimeter instead of activating buttons.
    pad.update({at(C::Touchpad)});
    state = pad.update({{1, 1279, 719, true}});
    assert(state.touchpad.size() == 1 && state.touchpad[0].x == 1279 && state.touchpad[0].y == 719);
    assert(!state.gamepad.b && !state.gamepad.rt);
    assert(state.active[static_cast<size_t>(C::Touchpad)] &&
           !state.active[static_cast<size_t>(C::B)]);
    // A dedicated pad can start on its right edge, bypassing the fullscreen menu reservation.
    pad.update({}); finger = at(C::Touchpad); finger.x = 838;
    state = pad.update({finger});
    lunar::ps::NativeTouchpad gestures;
    const auto time = lunar::ps::NativeTouchpad::Clock::now();
    auto gesture = gestures.update(state.touchpad, false, time);
    assert(gesture.points[0].active && gesture.points[0].x > 1184);
    // Reducing the pad must not magnify normal finger jitter into a swipe.
    {
        Pad small; small.configure(true, true, false); small.update({});
        lunar::ps::NativeTouchpad tap;
        auto contact = at(C::Touchpad);
        tap.update(small.update({contact}).touchpad, false, time);
        contact.x += 4; contact.y += 4;
        auto held = tap.update(small.update({contact}).touchpad, false,
                              time + std::chrono::milliseconds(600));
        assert(held.pressed && held.gesture == lunar::ps::NativeTouchpad::Gesture::LongPress);
    }
    // Opening UI, changing modes, or losing focus releases everything and fences held fingers.
    pad.configure(true, true, true);
    assert(pad.update({finger}).touchpad.empty());
    pad.configure(true, true, false);
    assert(pad.update({finger}).touchpad.empty());
    pad.update({}); assert(pad.update({finger}).touchpad.size() == 1);
    pad.configure(false, true, false);
    assert(pad.update({finger}).touchpad.empty()); pad.update({});
    finger = {100, 200, 300, true};
    state = pad.update({finger});
    assert(!state.enabled && state.touchpad.size() == 1 && state.touchpad[0].x == 200 && state.touchpad[0].touchscreen);
    // The app menu remains accessible with virtual controls hidden and never presses a game button.
    pad.update({}); state = pad.update({at(C::AppMenu)});
    assert(state.menu_requested && state.touchpad.empty() && !state.gamepad.menu);
    assert(!pad.update({at(C::AppMenu)}).menu_requested);
    pad.update({});
    pad.configure(true, false, false); pad.update({});
    assert(pad.update({at(C::Touchpad)}).touchpad.empty());
    pad.update({}); state = pad.update({at(C::Lt), at(C::Rt, 2), at(C::Guide, 3)});
    assert(state.gamepad.left_trigger == 65535 && state.gamepad.right_trigger == 65535 && state.gamepad.guide);
    auto physical = lunar::input::GamepadState{}; physical.b = true; physical.left_stick_y = 123;
    auto merged = Pad::merge(physical, state.gamepad);
    assert(merged.b && merged.guide && merged.left_stick_y == 123 && merged.lt && merged.rt);
    assert(pad.update({}).gamepad.left_trigger == 0);
    std::cout << "PASS touch gamepad: full controls, multitouch, ownership, pad routing, menu/focus/mode release\n";
}
