#pragma once
#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)

#include <borealis.hpp>
#include "../stream/video_codec.h"
#include "../stream/native_decode_mode.h"
#include <functional>

namespace lunar::ui {

struct PsSettingsSnapshot {
    bool touch_gamepad_enabled = false;
    int touch_gamepad_opacity = 50;
    stream::NativeDecodeMode native_decode_mode = stream::NativeDecodeMode::Automatic;
    int width = 1280;
    int height = 720;
    int bitrate_kbps = 10000;
    stream::VideoCodec video_codec = stream::VideoCodec::H264;
    bool vibration_enabled = true;
    int rumble_strength_percent = 50;
};

PsSettingsSnapshot loadPsSettings();
bool savePsSettings(const PsSettingsSnapshot& settings);

class PsSettingsActivity : public brls::Activity {
public:
    using CompletionCallback = std::function<void(const PsSettingsSnapshot&)>;
    PsSettingsActivity(PsSettingsSnapshot settings, CompletionCallback completion = {});
    brls::View* createContentView() override;

private:
    PsSettingsSnapshot settings_;
    CompletionCallback completion_;
    bool closed_ = false;
    void closeSettings();
};

} // namespace lunar::ui
#endif
