#include "native_gamepad.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <mutex>

namespace lunar::platform {
namespace {
std::mutex mutex;
SDL_GameController* controller = nullptr;
input::TouchGamepad touch_gamepad;
bool touch_menu_requested = false;

std::vector<NativeTouch> screenTouches() {
    std::vector<NativeTouch> touches;
    for (int device = 0; device < SDL_GetNumTouchDevices(); ++device) {
        const auto id = SDL_GetTouchDevice(device);
        if (id == SDL_MOUSE_TOUCHID) continue;
        for (int index = 0; index < SDL_GetNumTouchFingers(id); ++index) {
            const auto* finger = SDL_GetTouchFinger(id, index);
            if (finger) touches.push_back({static_cast<uint64_t>(finger->id),
                static_cast<uint32_t>(std::clamp(finger->x, 0.0f, 1.0f) * 1279),
                static_cast<uint32_t>(std::clamp(finger->y, 0.0f, 1.0f) * 719), true});
        }
        if (!touches.empty()) break;
    }
    return touches;
}

input::TouchGamepad::Snapshot sampleTouchGamepad() {
    auto snapshot = touch_gamepad.update(screenTouches());
    touch_menu_requested |= snapshot.menu_requested;
    return snapshot;
}

input::GamepadState rawState() {
    input::GamepadState state;
    if (controller && !SDL_GameControllerGetAttached(controller)) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
    if (!controller) {
        for (int index = 0; index < SDL_NumJoysticks(); ++index)
            if (SDL_IsGameController(index)) {
                controller = SDL_GameControllerOpen(index);
                if (controller) break;
            }
    }
    if (controller) {
        auto button = [](SDL_GameControllerButton value) {
            return SDL_GameControllerGetButton(controller, value) != 0;
        };
        state.a = button(SDL_CONTROLLER_BUTTON_A);
        state.b = button(SDL_CONTROLLER_BUTTON_B);
        state.x = button(SDL_CONTROLLER_BUTTON_X);
        state.y = button(SDL_CONTROLLER_BUTTON_Y);
        state.lb = button(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        state.rb = button(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        state.l3 = button(SDL_CONTROLLER_BUTTON_LEFTSTICK);
        state.r3 = button(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        state.view = button(SDL_CONTROLLER_BUTTON_BACK);
        state.menu = button(SDL_CONTROLLER_BUTTON_START);
        state.guide = button(SDL_CONTROLLER_BUTTON_GUIDE);
        state.touchpad = button(SDL_CONTROLLER_BUTTON_TOUCHPAD);
        state.dpad_up = button(SDL_CONTROLLER_BUTTON_DPAD_UP);
        state.dpad_down = button(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        state.dpad_left = button(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        state.dpad_right = button(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        auto axis = [](SDL_GameControllerAxis value) {
            int input = SDL_GameControllerGetAxis(controller, value);
            return static_cast<int16_t>(input > -2000 && input < 2000 ? 0 : input);
        };
        state.left_stick_x = axis(SDL_CONTROLLER_AXIS_LEFTX);
        state.left_stick_y = static_cast<int16_t>(-std::clamp<int>(axis(SDL_CONTROLLER_AXIS_LEFTY), -32767, 32767));
        state.right_stick_x = axis(SDL_CONTROLLER_AXIS_RIGHTX);
        state.right_stick_y = static_cast<int16_t>(-std::clamp<int>(axis(SDL_CONTROLLER_AXIS_RIGHTY), -32767, 32767));
        state.left_trigger = static_cast<uint16_t>(std::max<int>(0, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) * 2);
        state.right_trigger = static_cast<uint16_t>(std::max<int>(0, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) * 2);
    }
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (keys) {
        state.a |= keys[SDL_SCANCODE_RETURN]; state.b |= keys[SDL_SCANCODE_SPACE];
        state.x |= keys[SDL_SCANCODE_X]; state.y |= keys[SDL_SCANCODE_Y];
        state.lb |= keys[SDL_SCANCODE_LEFTBRACKET]; state.rb |= keys[SDL_SCANCODE_RIGHTBRACKET];
        state.view |= keys[SDL_SCANCODE_TAB]; state.menu |= keys[SDL_SCANCODE_ESCAPE];
        state.l3 |= keys[SDL_SCANCODE_Z]; state.r3 |= keys[SDL_SCANCODE_C];
        state.dpad_up |= keys[SDL_SCANCODE_UP]; state.dpad_down |= keys[SDL_SCANCODE_DOWN];
        state.dpad_left |= keys[SDL_SCANCODE_LEFT]; state.dpad_right |= keys[SDL_SCANCODE_RIGHT];
        if (keys[SDL_SCANCODE_Q]) state.left_trigger = 65535;
        if (keys[SDL_SCANCODE_E]) state.right_trigger = 65535;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D])
            state.left_stick_x = (keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A]) * 32767;
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S])
            state.left_stick_y = (keys[SDL_SCANCODE_W] - keys[SDL_SCANCODE_S]) * 32767;
        if (keys[SDL_SCANCODE_I] || keys[SDL_SCANCODE_K])
            state.right_stick_y = (keys[SDL_SCANCODE_I] - keys[SDL_SCANCODE_K]) * 32767;
        if (keys[SDL_SCANCODE_J] || keys[SDL_SCANCODE_L])
            state.right_stick_x = (keys[SDL_SCANCODE_L] - keys[SDL_SCANCODE_J]) * 32767;
    }
    state.lt = state.left_trigger > 16000;
    state.rt = state.right_trigger > 16000;
    return state;
}

uint64_t mask(const input::GamepadState& s) {
    using namespace mapping_bits;
    uint64_t value = 0;
    // Switch-shaped masks keep the original default mappings and disk schema.
    if (s.a) value |= HidNpadButton_B; if (s.b) value |= HidNpadButton_A;
    if (s.x) value |= HidNpadButton_Y; if (s.y) value |= HidNpadButton_X;
    if (s.lb) value |= HidNpadButton_L; if (s.rb) value |= HidNpadButton_R;
    if (s.lt) value |= HidNpadButton_ZL; if (s.rt) value |= HidNpadButton_ZR;
    if (s.l3) value |= HidNpadButton_StickL; if (s.r3) value |= HidNpadButton_StickR;
    if (s.view) value |= HidNpadButton_Minus; if (s.menu) value |= HidNpadButton_Plus;
    if (s.dpad_up) value |= HidNpadButton_Up; if (s.dpad_down) value |= HidNpadButton_Down;
    if (s.dpad_left) value |= HidNpadButton_Left; if (s.dpad_right) value |= HidNpadButton_Right;
    if (controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_MISC1))
        value |= input::kButtonMappingCapture;
    return value;
}
} // namespace

uint64_t nativeButtons() {
    std::lock_guard<std::mutex> lock(mutex);
    return mask(rawState());
}

input::GamepadState nativeGamepad(const input::ButtonMapping& mapping) {
    std::lock_guard<std::mutex> lock(mutex);
    auto state = rawState();
    uint64_t buttons = mask(state);
    using namespace mapping_bits;
    if ((buttons & HidNpadButton_Minus) && (buttons & HidNpadButton_Plus))
        buttons &= ~(HidNpadButton_Minus | HidNpadButton_Plus);
    uint64_t consumed = 0;
    for (auto chord : mapping)
        if (chord && (chord & (chord - 1)) && (buttons & chord) == chord) consumed |= chord;
    auto mapped = [&](input::RemoteButton button) {
        auto chord = mapping[static_cast<size_t>(button)];
        return chord && (buttons & chord) == chord &&
               ((chord & (chord - 1)) || !(consumed & chord));
    };
    using input::RemoteButton;
    state.a = mapped(RemoteButton::A); state.b = mapped(RemoteButton::B);
    state.x = mapped(RemoteButton::X); state.y = mapped(RemoteButton::Y);
    state.dpad_up = mapped(RemoteButton::DpadUp); state.dpad_down = mapped(RemoteButton::DpadDown);
    state.dpad_left = mapped(RemoteButton::DpadLeft); state.dpad_right = mapped(RemoteButton::DpadRight);
    state.lb = mapped(RemoteButton::Lb); state.rb = mapped(RemoteButton::Rb);
    state.lt = mapped(RemoteButton::Lt); state.rt = mapped(RemoteButton::Rt);
    state.l3 = mapped(RemoteButton::L3); state.r3 = mapped(RemoteButton::R3);
    state.view = mapped(RemoteButton::View); state.menu = mapped(RemoteButton::Menu);
    state.guide = state.guide || mapped(RemoteButton::Guide);
    state.touchpad = state.touchpad || mapped(RemoteButton::Touchpad);
    if (!state.lt) state.left_trigger = 0;
    else if (!state.left_trigger) state.left_trigger = 65535;
    if (!state.rt) state.right_trigger = 0;
    else if (!state.right_trigger) state.right_trigger = 65535;
    // Touch controls use remote button names directly, independent of physical remapping.
    return input::TouchGamepad::merge(state, sampleTouchGamepad().gamepad);
}

void configureNativeTouchGamepad(bool enabled, bool playstation, bool suppressed) {
    std::lock_guard<std::mutex> lock(mutex);
    touch_gamepad.configure(enabled, playstation, suppressed);
    if (suppressed) touch_menu_requested = false;
}

input::TouchGamepad::Snapshot nativeTouchGamepadSnapshot() {
    std::lock_guard<std::mutex> lock(mutex);
    return sampleTouchGamepad();
}

bool consumeNativeTouchMenuRequest() {
    std::lock_guard<std::mutex> lock(mutex);
    const bool requested = touch_menu_requested;
    touch_menu_requested = false;
    return requested;
}

bool nativeTouchGamepadEnabled() {
    std::lock_guard<std::mutex> lock(mutex);
    return touch_gamepad.enabled();
}

void nativeRumble(float left, float right, float left_trigger, float right_trigger, uint32_t duration) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!controller || !SDL_GameControllerGetAttached(controller)) return;
    auto amplitude = [](float value) {
        return static_cast<Uint16>(std::clamp(value, 0.0f, 1.0f) * 65535.0f);
    };
    SDL_GameControllerRumble(controller, amplitude(left), amplitude(right), duration);
    SDL_GameControllerRumbleTriggers(controller, amplitude(left_trigger), amplitude(right_trigger), duration);
}

std::vector<NativeTouch> nativeTouches() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<NativeTouch> result;
    rawState(); // Lazily opens or refreshes the current controller.
    auto coordinate = [](float value, uint32_t maximum) {
        return static_cast<uint32_t>(std::clamp(value, 0.0f, 1.0f) * maximum);
    };
    if (controller && SDL_GameControllerGetNumTouchpads(controller) > 0) {
        for (int finger = 0; finger < SDL_GameControllerGetNumTouchpadFingers(controller, 0); ++finger) {
            Uint8 active = 0;
            float x = 0, y = 0, pressure = 0;
            if (SDL_GameControllerGetTouchpadFinger(controller, 0, finger, &active, &x, &y, &pressure) == 0 && active)
                result.push_back({static_cast<uint64_t>(finger), coordinate(x, 1279), coordinate(y, 719)});
        }
        // Prefer the physical controller touchpad when it is in use.
        if (!result.empty()) return result;
    }
    // The virtual pad owns its fingers from down to up. Only the dedicated
    // PS region reaches the touchpad when the gamepad is visible.
    return sampleTouchGamepad().touchpad;
}

NativeMotion nativeMotion() {
    std::lock_guard<std::mutex> lock(mutex);
    NativeMotion result;
    rawState();
    if (!controller ||
        !SDL_GameControllerHasSensor(controller, SDL_SENSOR_GYRO) ||
        !SDL_GameControllerHasSensor(controller, SDL_SENSOR_ACCEL)) return result;
    for (auto sensor : {SDL_SENSOR_GYRO, SDL_SENSOR_ACCEL})
        if (!SDL_GameControllerIsSensorEnabled(controller, sensor) &&
            SDL_GameControllerSetSensorEnabled(controller, sensor, SDL_TRUE) < 0) return result;
    result.valid = SDL_GameControllerGetSensorData(controller, SDL_SENSOR_GYRO, result.gyro.data(), 3) == 0 &&
                   SDL_GameControllerGetSensorData(controller, SDL_SENSOR_ACCEL, result.accel.data(), 3) == 0;
    return result;
}

void shutdownNativeGamepad() {
    std::lock_guard<std::mutex> lock(mutex);
    if (controller) SDL_GameControllerClose(controller);
    controller = nullptr;
    touch_gamepad = {};
    touch_menu_requested = false;
}
} // namespace lunar::platform
