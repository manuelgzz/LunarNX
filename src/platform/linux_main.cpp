#include <borealis.hpp>
#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>

#include "native_gamepad.h"
#include "native_presentation.h"
#include "network_worker.h"
#include "../ui/platform_activity.h"
#include "../ui/stream_settings_activity.h"
#include "../ui/ps_settings_activity.h"
#include "../ui/button_mapping_activity.h"
#include "../ui/stream_loading_activity.h"
#include "../ui/i18n.h"
#include "../ui/ui_style.h"
#include "../ui/poster_loader.h"

namespace {
int windowCount() {
    int count = 0;
    for (Uint32 id = 1; id < 1024; ++id)
        if (SDL_GetWindowFromID(id)) ++count;
    return count;
}
}

int main(int argc, char** argv) {
    bool smoke = false;
    std::string mock;
    for (int i = 1; i < argc; ++i) {
        const std::string option = argv[i];
        if (option == "--ui-smoke") smoke = true;
        else if (option == "--mock-smoke" && i + 1 < argc) mock = argv[++i];
        else {
            std::fprintf(stderr, "usage: %s [--ui-smoke | --mock-smoke URL]\n", argv[0]);
            return 2;
        }
    }
    umask(0077);
    try {
        const auto executable = std::filesystem::canonical("/proc/self/exe");
        std::filesystem::current_path(executable.parent_path());
        lunar::ui::configureAppLocale();
        if (!brls::Application::init()) return 1;
        lunar::ui::installLunarTheme();
        brls::Application::createWindow("LunarNX");
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
            throw std::runtime_error("Cannot initialize SDL gamepad support");
        brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);
        brls::Application::setGlobalQuit(false);
        brls::Application::setFPSStatus(false);
        brls::Application::getExitEvent()->subscribe([]() {
            lunar::ui::PosterLoader::instance().shutdown();
            lunar::platform::shutdownNetworkWorkers();
            lunar::platform::shutdownNativeGamepad();
        });
        brls::Application::pushActivity(new lunar::ui::PlatformActivity());

        std::shared_ptr<lunar::app::StreamController> controller;
        if (!mock.empty()) {
            controller = std::make_shared<lunar::app::StreamController>();
            controller->setDefaultVideoBackend(lunar::stream::VideoBackend::Software);
            controller->bypassAuthForMock(mock);
            lunar::ui::StreamLaunchRequest request;
            request.target_id = "MOCKXBOX001";
            request.display_name = "Local mock Xbox";
            request.options.video_backend = lunar::stream::VideoBackend::Software;
            brls::Application::pushActivity(new lunar::ui::StreamLoadingActivity(
                controller, request, [](auto, const std::string&) {}));
        }

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(45);
        unsigned frames = 0;
        bool passed = false;
        while (brls::Application::mainLoop()) {
            ++frames;
            if (smoke) {
                // Exercise the real activities, not an alternative mock interface.
                if (frames == 20) brls::Application::pushActivity(
                    new lunar::ui::StreamSettingsActivity(nullptr,
                        lunar::ui::loadStreamSettings(), {}, lunar::ui::StreamSettingsScope::Global));
                if (frames == 45) brls::Application::popActivity(brls::TransitionAnimation::NONE);
                if (frames == 60) brls::Application::pushActivity(
                    new lunar::ui::PsSettingsActivity(lunar::ui::loadPsSettings(), {}));
                if (frames == 85) brls::Application::popActivity(brls::TransitionAnimation::NONE);
                if (frames == 100) brls::Application::pushActivity(
                    new lunar::ui::ButtonMappingActivity(lunar::input::ButtonMappingProfile::Xbox));
                if (frames == 125) brls::Application::popActivity(brls::TransitionAnimation::NONE);
                if (frames == 140) brls::Application::pushActivity(
                    new lunar::ui::StreamSettingsActivity(nullptr,
                        lunar::ui::loadStreamSettings(), {}, lunar::ui::StreamSettingsScope::Xbox));
                if (frames == 165) brls::Application::popActivity(brls::TransitionAnimation::NONE);
                if (frames == 180) {
                    passed = windowCount() == 1;
                    std::printf("%s native UI: home, global, Xbox, PlayStation and mapping settings; windows=%d\n",
                                passed ? "PASS" : "FAIL", windowCount());
                    brls::Application::quit();
                }
            }
            if (controller) {
                const auto& perf = controller->getPerfStats();
                if (lunar::platform::native_presented_frames.load() >= 10 &&
                    perf.audio_frames.load() >= 10) {
                    passed = windowCount() == 1;
                    std::printf("%s native UI WebRTC: presented=%llu audio=%u windows=%d\n",
                        passed ? "PASS" : "FAIL",
                        static_cast<unsigned long long>(lunar::platform::native_presented_frames.load()),
                        perf.audio_frames.load(), windowCount());
                    controller->requestStop();
                    brls::Application::quit();
                }
            }
            if ((smoke || controller) && std::chrono::steady_clock::now() > deadline) {
                std::fprintf(stderr, "Native UI smoke timed out\n");
                if (controller) controller->requestStop();
                brls::Application::quit();
            }
        }
        if (controller) controller->stopStream();
        return smoke || controller ? (passed ? 0 : 1) : 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "LunarNX initialization failed: %s\n", error.what());
        return 1;
    }
}
