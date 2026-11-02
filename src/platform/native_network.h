#pragma once
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/random.h>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace lunar::platform {
struct NativeIPv4 {
    uint32_t address;
    uint32_t netmask;
};
inline std::vector<NativeIPv4> nativeIPv4Addresses() {
    std::vector<NativeIPv4> result;
    ifaddrs* interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0) return result;
    for (auto* item = interfaces; item; item = item->ifa_next) {
        if (!item->ifa_addr || item->ifa_addr->sa_family != AF_INET ||
            !(item->ifa_flags & IFF_UP) || (item->ifa_flags & IFF_LOOPBACK)) continue;
        const auto address = reinterpret_cast<sockaddr_in*>(item->ifa_addr)->sin_addr.s_addr;
        const auto netmask = item->ifa_netmask
            ? reinterpret_cast<sockaddr_in*>(item->ifa_netmask)->sin_addr.s_addr : 0;
        if (address) result.push_back({address, netmask});
    }
    freeifaddrs(interfaces);
    return result;
}
inline bool nativeRandom(void* output, size_t size) {
    auto* bytes = static_cast<unsigned char*>(output);
    size_t written = 0;
    while (written < size) {
        const auto count = getrandom(bytes + written, size - written, 0);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) return false;
        written += static_cast<size_t>(count);
    }
    return true;
}
} // namespace lunar::platform
