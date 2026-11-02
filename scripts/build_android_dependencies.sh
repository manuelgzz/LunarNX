#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
: "${ANDROID_NDK_ROOT:?Set ANDROID_NDK_ROOT to NDK 28.2.13676358}"
native_tools="$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin"
export PATH="$native_tools:$PATH"
prefix="$project_root/build/android-prefix"
sources="$project_root/lib/android-sources"
peer="$project_root/lib/libpeer"
mkdir -p "$prefix"
if [ -f "$prefix/.lunarnx-complete" ]; then
    echo "Reusing completed ARM64 dependencies from the matching cache"
    exit 0
fi
export PKG_CONFIG_LIBDIR="$prefix/lib/pkgconfig"
export PKG_CONFIG_PATH=""
toolchain="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake"
build_cmake() {
    local source="$1"
    local name="$2"
    shift 2
    cmake -S "$source" -B "$project_root/build/android-deps/$name" \
        -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DANDROID_ABI=arm64-v8a \
        -DANDROID_PLATFORM=android-29 -DANDROID_STL=c++_shared \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_INSTALL_PREFIX="$prefix" -DCMAKE_PREFIX_PATH="$prefix" \
        -DCMAKE_FIND_ROOT_PATH="$prefix" -DBUILD_SHARED_LIBS=OFF \
        -DBUILD_TESTING=OFF "$@"
    cmake --build "$project_root/build/android-deps/$name" --parallel 2
    cmake --install "$project_root/build/android-deps/$name"
}
pushd "$sources/openssl"
export ANDROID_NDK_HOME="$ANDROID_NDK_ROOT"
./Configure android-arm64 -D__ANDROID_API__=29 --prefix="$prefix" --libdir=lib no-shared no-tests -fPIC
make -j2 build_libs
make install_dev
popd
build_cmake "$sources/opus" opus -DOPUS_BUILD_TESTING=OFF -DOPUS_BUILD_PROGRAMS=OFF
build_cmake "$sources/json-c" json-c -DDISABLE_WERROR=ON -DENABLE_THREADING=ON
build_cmake "$sources/miniupnp/miniupnpc" miniupnpc -DUPNPC_BUILD_STATIC=ON -DUPNPC_BUILD_SHARED=OFF -DUPNPC_BUILD_TESTS=OFF -DUPNPC_BUILD_SAMPLE=OFF
build_cmake "$peer/third_party/mbedtls" mbedtls -DENABLE_PROGRAMS=OFF -DENABLE_TESTING=OFF -DMBEDTLS_FATAL_WARNINGS=OFF
build_cmake "$peer/third_party/libsrtp" srtp -DENABLE_OPENSSL=ON -DOPENSSL_ROOT_DIR="$prefix" -DTEST_APPS=OFF
build_cmake "$peer/third_party/usrsctp" sctp -Dsctp_build_programs=OFF -Dsctp_build_shared_lib=OFF -Dsctp_inet=ON -Dsctp_inet6=ON -Dsctp_werror=OFF
build_cmake "$sources/SDL" SDL -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF -DSDL_TESTS=OFF
pushd "$sources/ffmpeg"
./configure --prefix="$prefix" --target-os=android --arch=aarch64 \
    --enable-cross-compile --cc="$native_tools/aarch64-linux-android29-clang" \
    --cxx="$native_tools/aarch64-linux-android29-clang++" --ar="$native_tools/llvm-ar" \
    --ranlib="$native_tools/llvm-ranlib" --nm="$native_tools/llvm-nm" \
    --strip="$native_tools/llvm-strip" --sysroot="$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/sysroot" \
    --enable-static --disable-shared --enable-pic --disable-programs --disable-doc \
    --disable-debug --disable-everything --enable-decoder=h264,hevc \
    --enable-parser=h264,hevc --enable-demuxer=mov --enable-protocol=file \
    --enable-swscale --enable-swresample --enable-avformat --disable-network \
    --disable-avdevice --disable-avfilter --disable-vulkan --disable-zlib \
    --extra-cflags="-fPIC" --extra-ldflags="-Wl,-z,max-page-size=16384"
make -j2
make install
popd
# Normalize install libdir differences for pkg-config's cross-only search path.
mkdir -p "$prefix/lib/pkgconfig"
if [ -d "$prefix/lib64/pkgconfig" ]; then
    cp "$prefix/lib64/pkgconfig/"*.pc "$prefix/lib/pkgconfig/"
fi

touch "$prefix/.lunarnx-complete"
