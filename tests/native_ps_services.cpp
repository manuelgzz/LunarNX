#include "../src/ps/ps_phone_pairing_server.h"
#include "../src/ps/psn_callback_server.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <string>

namespace {
struct Address { std::string host, path; int port; };
Address parse(const std::string& url) {
    assert(url.starts_with("http://"));
    const auto colon = url.find(':', 7), slash = url.find('/', colon);
    assert(colon != std::string::npos && slash != std::string::npos);
    return {url.substr(7, colon - 7), url.substr(slash),
            std::stoi(url.substr(colon + 1, slash - colon - 1))};
}
std::string request(const Address& address, const std::string& method,
                    const std::string& path, const std::string& body = "") {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    timeval timeout{3, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    sockaddr_in peer{};
    peer.sin_family = AF_INET;
    peer.sin_port = htons(address.port);
    assert(inet_pton(AF_INET, address.host.c_str(), &peer.sin_addr) == 1);
    assert(connect(fd, reinterpret_cast<sockaddr*>(&peer), sizeof(peer)) == 0);
    const auto wire = method + " " + path + " HTTP/1.1\r\nHost: " +
        address.host + "\r\nContent-Length: " + std::to_string(body.size()) +
        "\r\nConnection: close\r\n\r\n" + body;
    size_t sent = 0;
    while (sent < wire.size()) {
        const auto count = send(fd, wire.data() + sent, wire.size() - sent, MSG_NOSIGNAL);
        assert(count > 0);
        sent += static_cast<size_t>(count);
    }
    std::string response;
    char buffer[4096];
    for (;;) {
        const auto count = recv(fd, buffer, sizeof(buffer), 0);
        if (count == 0) break;
        assert(count > 0);
        response.append(buffer, static_cast<size_t>(count));
        assert(response.size() < 1024 * 1024);
    }
    close(fd);
    return response;
}
bool status(const std::string& response, int code) {
    return response.starts_with("HTTP/1.1 " + std::to_string(code));
}
std::string encode(const std::string& value) {
    std::string result;
    char encoded[4];
    for (unsigned char c : value) {
        std::snprintf(encoded, sizeof(encoded), "%%%02X", c);
        result += encoded;
    }
    return result;
}
}

int main() {
    std::atomic<int> pairing_calls{0}, login_calls{0};
    lunar::ps::PsPhonePairingServer pairing;
    assert(pairing.start("en-US", [&](lunar::ps::PsPhonePairingInput input) {
        assert(input.account_id == "AQIDBAUGBwg=" && input.pin == 12345678);
        ++pairing_calls;
    }));
    auto address = parse(pairing.getHelperUrl());
    assert(address.path.size() == std::string("/pair/").size() + 32);
    assert(status(request(address, "GET", address.path), 200));
    assert(status(request(address, "POST", "/pair/wrong/submit", ""), 404));
    assert(status(request(address, "POST", address.path + "/submit",
        "account_input=AQIDBAUGBwg%3D&pin=12"), 400));
    assert(pairing_calls == 0);
    assert(status(request(address, "POST", address.path + "/submit",
        "account_input=AQIDBAUGBwg%3D&pin=12345678"), 200));
    pairing.stop(); // Joins the callback before inspecting its result.
    assert(pairing_calls == 1);

    lunar::ps::PsnCallbackServer login;
    assert(login.start("https://example.invalid/login?client=offline-test", "en-US",
        [&](std::string input) { assert(input.find("code=offline-code") != std::string::npos); ++login_calls; }));
    address = parse(login.getHelperUrl());
    const auto state = address.path.substr(address.path.rfind('/') + 1);
    assert(state.size() == 32);
    assert(status(request(address, "GET", address.path), 200));
    const std::string redirect =
        "https://remoteplay.dl.playstation.net/remoteplay/redirect?code=offline-code&state=";
    assert(status(request(address, "POST", address.path + "/submit",
        "callback=" + encode(redirect + "wrong")), 400));
    assert(login_calls == 0);
    assert(status(request(address, "POST", address.path + "/submit",
        "callback=" + encode(redirect + state)), 200));
    login.stop();
    assert(login_calls == 1);
    std::puts("PASS native PlayStation services: pairing validation and PSN callback state");
}
