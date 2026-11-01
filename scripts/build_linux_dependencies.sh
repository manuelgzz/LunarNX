#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
peer_dir="$project_root/lib/libpeer"
mbedtls_dir="$peer_dir/third_party/mbedtls"
libsrtp_dir="$peer_dir/third_party/libsrtp"
usrsctp_dir="$peer_dir/third_party/usrsctp"
stage_dir="$peer_dir/build/dist"
build_jobs="${BUILD_JOBS:-2}"

test -f "$peer_dir/LINUX_PROVENANCE.json"
cmake -S "$mbedtls_dir" -B "$mbedtls_dir/build_host" \
    -DCMAKE_BUILD_TYPE=Release -DENABLE_PROGRAMS=OFF -DENABLE_TESTING=OFF \
    -DMBEDTLS_FATAL_WARNINGS=OFF -DUSE_SHARED_MBEDTLS_LIBRARY=OFF \
    -DUSE_STATIC_MBEDTLS_LIBRARY=ON
cmake --build "$mbedtls_dir/build_host" --parallel "$build_jobs"

# The pinned libsrtp revision provides its OpenSSL backend on Linux.
cmake -S "$libsrtp_dir" -B "$libsrtp_dir/build_host" \
    -DCMAKE_BUILD_TYPE=Release -DTEST_APPS=OFF -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_OPENSSL=ON -DCMAKE_INSTALL_PREFIX="$stage_dir"
cmake --build "$libsrtp_dir/build_host" --parallel "$build_jobs"
cmake --install "$libsrtp_dir/build_host"

cmake -S "$usrsctp_dir" -B "$usrsctp_dir/build_host" \
    -DCMAKE_BUILD_TYPE=Release -Dsctp_werror=OFF -Dsctp_build_programs=OFF \
    -Dsctp_build_shared_lib=OFF -Dsctp_debug=OFF \
    -DCMAKE_INSTALL_PREFIX="$stage_dir"
cmake --build "$usrsctp_dir/build_host" --parallel "$build_jobs"
cmake --install "$usrsctp_dir/build_host"
