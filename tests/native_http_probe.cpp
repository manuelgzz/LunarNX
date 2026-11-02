#include "../src/api/http_client.h"
#include <cstdio>
#include <string>

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    lunar::api::HttpClient http;
    const auto response = http.post(argv[1], "offline-probe",
        {{"Content-Type", "text/plain"}});
    const bool expect_success = std::string(argv[2]) == "success";
    if (expect_success) {
        if (!response.network_error && response.status_code == 200 && response.body == "ok") return 0;
    } else if (response.network_error && response.error_message.find("certificate") != std::string::npos) {
        return 0;
    }
    std::fprintf(stderr, "Unexpected HTTPS result: status=%d network=%d detail=%s\n",
        response.status_code, response.network_error, response.error_message.c_str());
    return 1;
}
