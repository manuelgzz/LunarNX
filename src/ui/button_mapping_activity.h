#pragma once
#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)

#include <borealis.hpp>
#ifdef __SWITCH__
#include <switch.h>
#else
#include "../platform/native_gamepad.h"
#endif

#include "../input/button_mapping.h"

#include <array>
#include <cstddef>
#include <vector>

namespace lunar::ui {

class ButtonMappingActivity : public brls::Activity {
public:
    explicit ButtonMappingActivity(input::ButtonMappingProfile profile);
    ~ButtonMappingActivity() override;

    brls::View* createContentView() override;
    void pollCaptureInput();

private:
    void enterCapture(size_t index);
    void finishCapture();
    void refreshRows();
    bool hasConflict(size_t index) const;

    input::ButtonMapping mapping_;
    input::ButtonMappingProfile profile_;
    std::vector<brls::DetailCell*> rows_;
    brls::Box* mapping_content_ = nullptr;
    brls::Box* capture_content_ = nullptr;
    brls::Label* capture_status_ = nullptr;
#ifdef __SWITCH__
    PadState capture_pad_{};
#endif
    size_t capture_index_ = 0;
    uint64_t peak_buttons_ = 0;
    bool capturing_ = false;
    bool waiting_for_release_ = false;
    bool saw_button_ = false;
    int release_frames_ = 0;
};

} // namespace lunar::ui
#endif
