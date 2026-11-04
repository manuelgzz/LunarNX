#include "../src/stream/software_video_frame.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

int main() {
    auto& sink = lunar::stream::SoftwareVideoFrameSink::instance();
    sink.clear();
    assert(!sink.acquire());
    std::vector<uint8_t> pixels(64 * 64 * 4, 7);
    assert(sink.publishRgba(pixels.data(), 64, 64, 1));
    auto first = sink.acquire();
    assert(first && first->rgba[0] == 7);
    for (int i = 0; i < 100; ++i) {
        auto repaint = sink.acquire();
        assert(repaint.get() == first.get());
        assert(repaint->rgba.data() == first->rgba.data());
    }
    pixels.assign(pixels.size(), 9);
    assert(first->rgba[0] == 7);
    assert(sink.publishRgba(pixels.data(), 64, 64, 2));
    assert(sink.acquire()->generation > first->generation);
    assert(first->rgba.back() == 7);
    sink.clear();
    assert(!sink.acquire());
    assert(first->rgba[0] == 7); // Upload ownership survives stop/reset.
    std::atomic<bool> finished{false};
    std::thread producer([&] {
        for (int i = 0; i < 500; ++i) {
            pixels.assign(pixels.size(), static_cast<uint8_t>(i));
            assert(sink.publishRgba(pixels.data(), 64, 64, i));
            if (i % 9 == 0) sink.clear();
        }
        finished = true;
    });
    do {
        auto frame = sink.acquire();
        if (frame) {
            assert(frame->rgba.size() == 64 * 64 * 4);
            for (auto value : frame->rgba) assert(value == frame->rgba[0]);
        }
    } while (!finished);
    producer.join();
    assert(!sink.publishRgba(nullptr, 64, 64, 0));
    assert(!sink.publishRgba(pixels.data(), 0, 64, 0));
}
