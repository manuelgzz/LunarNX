#pragma once
#include <cstdlib>
#include <filesystem>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>

namespace lunar::platform {
inline std::string nativeLocale() {
    const char* value = std::getenv("LC_ALL");
    if (!value || !*value) value = std::getenv("LC_MESSAGES");
    if (!value || !*value) value = std::getenv("LANG");
    std::string locale = value ? value : "en-US";
    locale = locale.substr(0, locale.find_first_of(".@"));
    for (char& c : locale) if (c == '_') c = '-';
    if (locale == "C" || locale == "POSIX") return "en-US";
    if (locale == "zh-CN" || locale == "zh-SG") return "zh-Hans";
    if (locale == "zh-TW" || locale == "zh-HK") return "zh-Hant";
    return locale;
}

inline const char* nativePath(const char* name, bool cache = false) {
    static std::mutex mutex;
    static std::map<std::string, std::string> paths;
    std::lock_guard<std::mutex> lock(mutex);
    const std::string key = std::string(cache ? "cache/" : "config/") + name;
    auto it = paths.find(key);
    if (it != paths.end()) return it->second.c_str();
    const char* configured = std::getenv(cache ? "XDG_CACHE_HOME" : "XDG_CONFIG_HOME");
    std::filesystem::path base;
    if (configured && std::filesystem::path(configured).is_absolute()) base = configured;
    else {
        const char* home = std::getenv("HOME");
        if (!home || !std::filesystem::path(home).is_absolute())
            throw std::runtime_error("A valid HOME or XDG directory is required");
        base = std::filesystem::path(home) / (cache ? ".cache" : ".config");
    }
    base /= "lunarnx";
    std::filesystem::create_directories(base);
    std::filesystem::permissions(base, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    auto result = paths.emplace(key, (base / name).string());
    return result.first->second.c_str();
}
} // namespace lunar::platform
