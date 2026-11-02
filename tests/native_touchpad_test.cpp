#include "../src/ps/native_touchpad.h"
#include <cassert>
#include <iostream>
using lunar::ps::NativeTouchpad;
using lunar::platform::NativeTouch;
using namespace std::chrono;
int main() {
    NativeTouchpad pad;
    const auto start = NativeTouchpad::Clock::time_point{} + seconds(1);
    auto update = [&](std::vector<NativeTouch> touches, int ms, bool suppressed = false) {
        return pad.update(touches, suppressed, start + milliseconds(ms));
    };
    auto state = update({{10, 100, 200, true}}, 0);
    assert(state.points[0].active && !state.pressed); // immediate stationary contact
    state = update({{10, 105, 205, true}}, 8);
    assert(state.points[0].x == 105 && state.points[0].y == 205); // sub-threshold motion
    state = update({{10, 105, 205, true}, {20, 600, 400, true}}, 16);
    assert(state.points[0].id == 10 && state.points[1].id == 20);
    state = update({{20, 610, 410, true}}, 24);
    assert(!state.points[0].active && state.points[1].active && state.points[1].id == 20);
    state = update({{20, 610, 410, true}, {30, 300, 300, true}}, 32);
    assert(state.points[0].id == 30 && state.points[1].id == 20);
    state = update({{20, 610, 410, true}, {35, 300, 300, true}}, 33);
    assert(!state.points[0].active && state.points[1].id == 20);
    state = update({{20, 610, 410, true}, {35, 300, 300, true}}, 34);
    assert(state.points[0].id == 35 && state.points[1].id == 20);
    state = update({}, 40);
    assert(!state.pressed && !state.points[0].active && !state.points[1].active);
    state = update({{40, 200, 200, true}}, 100);
    state = update({}, 150);
    assert(state.pressed && state.gesture == NativeTouchpad::Gesture::Tap);
    assert(!state.points[0].active && !state.points[1].active); // lifted contacts stay released
    assert(!update({}, 301).pressed);
    update({{50, 200, 200, true}}, 400);
    state = update({{50, 200, 200, true}}, 901);
    assert(state.pressed && state.gesture == NativeTouchpad::Gesture::LongPress);
    assert(!update({}, 910).pressed);
    update({{60, 200, 200, true}}, 1000);
    state = update({{60, 400, 200, true}}, 1008);
    assert(state.gesture == NativeTouchpad::Gesture::Pan && !state.pressed);
    assert(!update({}, 1016).pressed); // swipe does not click
    update({{70, 200, 200, true}}, 1100);
    assert(!update({{70, 200, 200, true}}, 1108, true).points[0].active);
    assert(!update({{70, 300, 200, true}}, 1116).points[0].active);
    update({}, 1124);
    assert(update({{80, 200, 200, true}}, 1132).points[0].active);
    pad.reset();
    assert(!update({{90, 1250, 200, true}}, 1200).points[0].active);
    assert(!update({{90, 900, 200, true}}, 1208).points[0].active); // menu swipe reserved
    update({}, 1216);
    assert(update({{100, 1250, 200, false}}, 1224).points[0].active); // controller has no edge reservation
    pad.reset();
    state = update({{110, 5000, 5000, false}}, 1300);
    assert(state.points[0].x == 1279 && state.points[0].y == 719);
    pad.reset();
    update({{120, 100, 100, true}, {130, 400, 400, true}}, 1400);
    assert(!update({{120, 100, 100, true}, {130, 400, 400, true}}, 2000).pressed);
    assert(!update({}, 2008).pressed);
    std::cout << "PASS native touchpad: contacts, two fingers, replacement, click, hold, swipe, menu, suppression\n";
}
