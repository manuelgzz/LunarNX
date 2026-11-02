#pragma once

#include <string>

namespace lunar::auth {

inline std::string describeXboxNetworkFailure(const std::string& stage,
                                              const std::string& detail) {
    return stage + (detail.empty()
        ? " No transport details are available."
        : " " + detail);
}

std::string describeXstsFailure(int status, const std::string& body);

} // namespace lunar::auth
