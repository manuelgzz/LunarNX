// Exercise the actual native media backends without credentials or network.
#include "app/stream_controller.h"
#include "stream/stream_backend_provider.h"
#include "stream/video_decoder.h"
#include "stream/video_renderer.h"
#include "stream/audio_decoder.h"
#include "stream/audio_player.h"

extern "C" {
#include <libavformat/avformat.h>
#include <opus/opus.h>
}
#include <SDL2/SDL.h>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void checkVideo(lunar::stream::VideoPipelinePath path,
                lunar::stream::VideoCodec codec, const char* fixture) {
    using namespace lunar::stream;
    auto provider = StreamBackendProvider::createDefault();
    auto decoder = provider->createVideoDecoder(path);
    auto renderer = provider->createVideoRenderer();
    decoder->setVideoBackend(VideoBackend::Software);
    decoder->setVideoCodec(codec);
    require(decoder->initialize(320, 180), "video decoder initialization failed");
    require(renderer->initialize("LunarNX native media smoke", 320, 180),
            "SDL video renderer initialization failed");

    unsigned frames = 0;
    bool valid = true;
    decoder->setCallback([&](const VideoFrame& frame) {
        valid = valid && frame.width == 320 && frame.height == 180 &&
                frame.data[0] && renderer->render(frame);
        ++frames;
    });

    AVFormatContext* raw = nullptr;
    require(avformat_open_input(&raw, fixture, nullptr, nullptr) == 0,
            "cannot open generated media fixture");
    auto closeInput = [](AVFormatContext* input) { avformat_close_input(&input); };
    std::unique_ptr<AVFormatContext, decltype(closeInput)> input(raw, closeInput);
    require(avformat_find_stream_info(raw, nullptr) >= 0, "cannot inspect fixture");
    const int video = av_find_best_stream(raw, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    require(video >= 0, "fixture contains no video");
    auto freePacket = [](AVPacket* packet) { av_packet_free(&packet); };
    std::unique_ptr<AVPacket, decltype(freePacket)> packet(av_packet_alloc(), freePacket);
    require(bool(packet), "packet allocation failed");
    uint64_t timestamp = 1;
    while (av_read_frame(raw, packet.get()) >= 0) {
        if (packet->stream_index == video) {
            require(decoder->decode(packet->data, packet->size, timestamp),
                    "video access unit was rejected");
            timestamp += 16'666'667;
        }
        av_packet_unref(packet.get());
    }
    decoder->flush();
    require(valid && frames >= 2, "expected decoded and presented fixture frames");
    require(renderer->successfulPresentCount() == frames, "SDL presentation failed");
    require(renderer->consumeRenderFault() == RenderFault::None, "renderer fault");
    decoder->shutdown();
    renderer->shutdown();
    std::printf("PASS %s %s: %u frames decoded and presented\n",
                path == VideoPipelinePath::Xbox ? "Xbox" : "PlayStation",
                videoCodecName(codec), frames);
}

void checkAudio() {
    using namespace lunar::stream;
    auto provider = StreamBackendProvider::createDefault();
    auto decoder = provider->createAudioDecoder();
    auto player = provider->createAudioPlayer();
    require(decoder->initialize(), "Opus decoder initialization failed");
    require(player->initialize(), "SDL audio initialization failed");
    int error = OPUS_OK;
    OpusEncoder* raw = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
    require(raw && error == OPUS_OK, "test Opus encoder initialization failed");
    std::unique_ptr<OpusEncoder, decltype(&opus_encoder_destroy)> encoder(raw, opus_encoder_destroy);
    std::array<opus_int16, 960 * 2> pcm{};
    std::array<unsigned char, 4096> packet{};
    const int bytes = opus_encode(encoder.get(), pcm.data(), 960, packet.data(), packet.size());
    require(bytes > 0, "test Opus packet encoding failed");
    unsigned frames = 0;
    bool valid = true;
    decoder->setCallback([&](const AudioFrame& frame) {
        valid = valid && frame.sample_rate == 48000 && frame.channels == 2 &&
                frame.sample_count == 960 && frame.pcm_data.size() == 3840 &&
                player->play(frame);
        ++frames;
    });
    require(decoder->decode(packet.data(), bytes, 1), "Opus packet decode failed");
    require(decoder->decodeMissing(20'000'001), "Opus loss concealment failed");
    require(valid && frames == 2, "audio callbacks or output failed");
    player->flush();
    player->shutdown();
    decoder->shutdown();
    std::puts("PASS Opus: 48 kHz stereo output and packet-loss concealment");
}
} // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s fixture.h264 fixture.hevc\n", argv[0]);
        return 2;
    }
    try {
        // Construction must not start authentication or network streaming.
        lunar::app::StreamController controller;
        require(controller.getState() == lunar::app::StreamState::Idle,
                "controller must start idle");
        checkVideo(lunar::stream::VideoPipelinePath::Xbox,
                   lunar::stream::VideoCodec::H264, argv[1]);
        checkVideo(lunar::stream::VideoPipelinePath::PlayStation,
                   lunar::stream::VideoCodec::H264, argv[1]);
        checkVideo(lunar::stream::VideoPipelinePath::PlayStation,
                   lunar::stream::VideoCodec::HEVC, argv[2]);
        checkAudio();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL native media: %s\n", error.what());
        SDL_Quit();
        return 1;
    }
    SDL_Quit();
    return 0;
}
