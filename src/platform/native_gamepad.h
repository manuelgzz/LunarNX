#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include "../input/gamepad_reader.h"
#include "native_touch.h"

namespace lunar::platform {
// Stable masks used by LunarNX's existing per-platform configuration files.
namespace mapping_bits {
inline constexpr uint64_t HidNpadButton_A = 1ULL << 0;
inline constexpr uint64_t HidNpadButton_B = 1ULL << 1;
inline constexpr uint64_t HidNpadButton_X = 1ULL << 2;
inline constexpr uint64_t HidNpadButton_Y = 1ULL << 3;
inline constexpr uint64_t HidNpadButton_StickL = 1ULL << 4;
inline constexpr uint64_t HidNpadButton_StickR = 1ULL << 5;
inline constexpr uint64_t HidNpadButton_L = 1ULL << 6;
inline constexpr uint64_t HidNpadButton_R = 1ULL << 7;
inline constexpr uint64_t HidNpadButton_ZL = 1ULL << 8;
inline constexpr uint64_t HidNpadButton_ZR = 1ULL << 9;
inline constexpr uint64_t HidNpadButton_Plus = 1ULL << 10;
inline constexpr uint64_t HidNpadButton_Minus = 1ULL << 11;
inline constexpr uint64_t HidNpadButton_Left = 1ULL << 12;
inline constexpr uint64_t HidNpadButton_Up = 1ULL << 13;
inline constexpr uint64_t HidNpadButton_Right = 1ULL << 14;
inline constexpr uint64_t HidNpadButton_Down = 1ULL << 15;
}
struct NativeMotion {
    bool valid = false;
    std::array<float, 3> gyro{}, accel{};
};
std::vector<NativeTouch> nativeTouches();
NativeMotion nativeMotion();
uint64_t nativeButtons();
input::GamepadState nativeGamepad(const input::ButtonMapping& mapping);
void nativeRumble(float left, float right, float left_trigger, float right_trigger, uint32_t duration);
void shutdownNativeGamepad();
} // namespace lunar::platform
