#include "app/stream_controller.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <cstdio>
#include <thread>

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    lunar::app::StreamController controller;
    controller.setDefaultVideoBackend(lunar::stream::VideoBackend::Software);
    controller.setStateCallback([](lunar::app::StreamState state, const std::string& info) {
        std::printf("mock state=%d %s\n", static_cast<int>(state), info.c_str());
        std::fflush(stdout);
    });
    if (!controller.bypassAuthForMock(argv[1])) return 1;
    lunar::stream::MediaPipelineOptions options;
    options.video_backend = lunar::stream::VideoBackend::Software;
    if (!controller.startStream("MOCKXBOX001", 1280, 720, options)) {
        std::fprintf(stderr, "mock startup failed: %s\n", controller.getLastStreamError().c_str());
        return 1;
    }
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(25);
    bool received = false;
    while (std::chrono::steady_clock::now() < deadline) {
        controller.update();
        controller.presentVideoFrame();
        const auto& stats = controller.getPerfStats();
        if (stats.video_frames.load() >= 10 && stats.audio_frames.load() >= 10) {
            std::printf("PASS mock WebRTC: video=%u audio=%u\n",
                        stats.video_frames.load(), stats.audio_frames.load());
            received = true;
            break;
        }
        if (controller.getState() == lunar::app::StreamState::Error ||
            controller.getState() == lunar::app::StreamState::Disconnected)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    if (!received) {
        const auto& stats = controller.getPerfStats();
        std::fprintf(stderr, "mock media missing: video=%u audio=%u error=%s\n",
                     stats.video_frames.load(), stats.audio_frames.load(),
                     controller.getLastStreamError().c_str());
    }
    controller.stopStream();
    // Controller/media destructors release SDL before process exit.
    return received ? 0 : 1;
}
