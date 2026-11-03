#include "ui/stream_settings_activity.h"
#include "ui/ps_settings_activity.h"
#include "input/button_mapping.h"
#include "input/rumble_controller.h"
#include "platform/native_gamepad.h"
#include "common.h"
#include <SDL2/SDL.h>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
struct IsolatedState {
    std::string directory;
    IsolatedState() {
        auto pattern = (std::filesystem::temp_directory_path() / "lunarnx-settings-XXXXXX").string();
        char* result = mkdtemp(pattern.data());
        if (!result) throw std::runtime_error("Cannot create isolated settings directory");
        directory = result;
        setenv("XDG_CONFIG_HOME", directory.c_str(), 1);
        setenv("XDG_CACHE_HOME", directory.c_str(), 1);
    }
    ~IsolatedState() {
        std::error_code ignored;
        std::filesystem::remove_all(directory, ignored);
    }
};

void checkSettings() {
    using namespace lunar;
    std::ofstream(get_config_path()) << R"({"future_option":"preserve"})";
    auto xbox = ui::loadStreamSettings();
    xbox.width = 1920; xbox.height = 1080; xbox.bitrate_kbps = 30000;
    require(xbox.native_decode_mode == stream::NativeDecodeMode::Automatic,
            "New Xbox installations must retain automatic acceleration");
    xbox.native_decode_mode = stream::NativeDecodeMode::Software;
    xbox.show_decoder_overlay = true;
    xbox.preferred_game_language = "es-MX";
    xbox.vibration_enabled = false; xbox.rumble_strength_percent = 25;
    require(ui::saveStreamSettings(xbox), "Cannot save Xbox settings");
    auto ps = ui::loadPsSettings();
    ps.width = 1280; ps.height = 720; ps.bitrate_kbps = 20000;
    require(ps.native_decode_mode == stream::NativeDecodeMode::Automatic,
            "Xbox selection must not change PlayStation defaults");
    ps.video_codec = stream::VideoCodec::HEVC;
    ps.vibration_enabled = true; ps.rumble_strength_percent = 75;
    require(ui::savePsSettings(ps), "Cannot save PlayStation settings");
    auto restoredXbox = ui::loadStreamSettings();
    require(restoredXbox.height == 1080 && restoredXbox.bitrate_kbps == 30000 &&
            restoredXbox.preferred_game_language == "es-MX" &&
            restoredXbox.native_decode_mode == stream::NativeDecodeMode::Software &&
            restoredXbox.show_decoder_overlay &&
            !restoredXbox.vibration_enabled && restoredXbox.rumble_strength_percent == 25,
            "PlayStation save changed independent Xbox settings");
    ps.native_decode_mode = stream::NativeDecodeMode::Software;
    require(ui::savePsSettings(ps), "Cannot save PlayStation CPU preference");
    xbox.native_decode_mode = stream::NativeDecodeMode::Automatic;
    xbox.show_decoder_overlay = false;
    xbox.rumble_strength_percent = 50;
    require(ui::saveStreamSettings(xbox), "Cannot update Xbox settings");
    auto restoredPs = ui::loadPsSettings();
    require(restoredPs.height == 720 && restoredPs.bitrate_kbps == 20000 &&
            restoredPs.video_codec == stream::VideoCodec::HEVC &&
            restoredPs.native_decode_mode == stream::NativeDecodeMode::Software &&
            restoredPs.vibration_enabled && restoredPs.rumble_strength_percent == 75,
            "Xbox save changed independent PlayStation settings");
    require(ui::loadStreamSettings().native_decode_mode == stream::NativeDecodeMode::Automatic &&
            !ui::loadStreamSettings().show_decoder_overlay,
            "Automatic mode and hidden overlay did not survive reloading");
    auto xboxMapping = input::defaultButtonMapping(input::ButtonMappingProfile::Xbox);
    auto psMapping = input::defaultButtonMapping(input::ButtonMappingProfile::PlayStation);
    xboxMapping[0] = platform::mapping_bits::HidNpadButton_L;
    psMapping[0] = platform::mapping_bits::HidNpadButton_R;
    require(input::saveButtonMapping(input::ButtonMappingProfile::Xbox, xboxMapping),
            "Cannot save Xbox mapping");
    require(input::saveButtonMapping(input::ButtonMappingProfile::PlayStation, psMapping),
            "Cannot save PlayStation mapping");
    require(input::loadButtonMapping(input::ButtonMappingProfile::Xbox) == xboxMapping &&
            input::loadButtonMapping(input::ButtonMappingProfile::PlayStation) == psMapping,
            "Controller mappings are not independent");
    std::ifstream file(get_config_path());
    const std::string content((std::istreambuf_iterator<char>(file)), {});
    require(content.find("future_option") != std::string::npos,
            "Unrecognized settings must survive updates");
    std::puts("PASS native settings: Xbox/PS quality, language, codec, rumble and mappings stay independent");
}

void checkController() {
    using namespace lunar;
    require(SDL_Init(SDL_INIT_GAMECONTROLLER) == 0, "SDL input initialization failed");
    struct Vibration { Uint16 left = 0, right = 0; } vibration;
    SDL_VirtualJoystickDesc description{};
    description.version = SDL_VIRTUAL_JOYSTICK_DESC_VERSION;
    description.type = SDL_JOYSTICK_TYPE_GAMECONTROLLER;
    description.naxes = SDL_CONTROLLER_AXIS_MAX;
    description.nbuttons = SDL_CONTROLLER_BUTTON_MAX;
    description.button_mask = (1u << SDL_CONTROLLER_BUTTON_MAX) - 1;
    description.axis_mask = (1u << SDL_CONTROLLER_AXIS_MAX) - 1;
    description.userdata = &vibration;
    description.Rumble = [](void* data, Uint16 left, Uint16 right) {
        auto* output = static_cast<Vibration*>(data);
        output->left = left; output->right = right;
        return 0;
    };
    const int index = SDL_JoystickAttachVirtualEx(&description);
    require(index >= 0, "Virtual gamepad creation failed");
    SDL_Joystick* joystick = SDL_JoystickOpen(index);
    require(joystick != nullptr, "Virtual gamepad open failed");
    SDL_JoystickSetVirtualButton(joystick, SDL_CONTROLLER_BUTTON_A, 1);
    SDL_JoystickSetVirtualAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX, 16000);
    SDL_JoystickUpdate();
    const auto defaults = input::defaultButtonMapping(input::ButtonMappingProfile::Xbox);
    const auto state = platform::nativeGamepad(defaults);
    require(state.a && !state.b && state.left_stick_x > 0, "SDL gamepad input was not routed");
    auto remapped = defaults;
    remapped[static_cast<size_t>(input::RemoteButton::A)] = 0;
    remapped[static_cast<size_t>(input::RemoteButton::B)] = platform::mapping_bits::HidNpadButton_B;
    const auto changed = platform::nativeGamepad(remapped);
    require(!changed.a && changed.b, "Per-client button mapping was not applied");
    input::RumbleController feedback;
    require(feedback.initialize(), "Rumble initialization failed");
    feedback.setStrengthPercent(50);
    feedback.setRumble(0, 1.0f, 0.5f, 0, 0, 500, 0, 0);
    feedback.update();
    require(vibration.left > vibration.right && vibration.right > 0,
            "SDL rumble output was not routed");
    const auto previous = vibration.left;
    feedback.setStrengthPercent(100);
    feedback.update();
    require(vibration.left > previous, "Rumble strength setting was not applied");
    feedback.setEnabled(false);
    require(vibration.left == 0 && vibration.right == 0, "Disabling rumble must stop output");
    feedback.stop();
    platform::shutdownNativeGamepad();
    SDL_JoystickClose(joystick);
    SDL_JoystickDetachVirtual(index);
    SDL_Quit();
    std::puts("PASS native controller: SDL virtual gamepad buttons, stick, remapping and rumble");
}
} // namespace

int main() {
    try {
        IsolatedState isolated;
        checkSettings();
        checkController();
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL native settings/input: %s\n", error.what());
        lunar::platform::shutdownNativeGamepad();
        SDL_Quit();
        return 1;
    }
}
