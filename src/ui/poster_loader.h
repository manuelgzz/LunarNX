#pragma once

#include <cstdint>
#include <string>

#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)
#include <borealis.hpp>
#endif

namespace lunar::ui {

// Serial poster loader: one download at a time, next starts after previous finishes.
class PosterLoader {
public:
    using BatchId = uint32_t;

    static PosterLoader& instance();

#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)
    BatchId beginBatch();
    void load(brls::Image* view, const std::string& url, BatchId batch);
    void clear(brls::Image* view);
#endif
    void shutdown();
};

} // namespace lunar::ui
