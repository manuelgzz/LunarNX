#include "stream/native_decode_mode.h"
#include <cassert>
#include <initializer_list>
#include <cstring>
int main() {
    using namespace lunar::stream;
    for (const char* value : {static_cast<const char*>(nullptr), "", "auto", "unknown"})
        assert(parseNativeDecodeMode(value) == NativeDecodeMode::Automatic);
    for (auto mode : {NativeDecodeMode::Automatic, NativeDecodeMode::Software})
        assert(parseNativeDecodeMode(nativeDecodeModeName(mode)) == mode);
    assert(!nativeSoftwareRequested(NativeDecodeMode::Automatic, nullptr));
    assert(nativeSoftwareRequested(NativeDecodeMode::Automatic, "software"));
    assert(nativeSoftwareRequested(NativeDecodeMode::Software, nullptr));
    assert(nativeSoftwareRequested(NativeDecodeMode::Software, "auto"));
}
