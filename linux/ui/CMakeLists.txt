cmake_minimum_required(VERSION 3.20)
project(LunarNXNativeUI LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(PEER "${ROOT}/lib/libpeer")
set(BOREALIS_LIBRARY "${ROOT}/lib/borealis/library")

if(ANDROID)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(PLATFORM_DESKTOP OFF CACHE BOOL "" FORCE)
    set(PLATFORM_ANDROID ON CACHE BOOL "" FORCE)
    set(USE_GLES3 ON CACHE BOOL "" FORCE)
    set(USE_LIBROMFS OFF CACHE BOOL "" FORCE)
    set(ANDROID_PREFIX "${ROOT}/build/android-prefix")
    list(APPEND CMAKE_FIND_ROOT_PATH "${ANDROID_PREFIX}")
    list(APPEND CMAKE_PREFIX_PATH "${ANDROID_PREFIX}")
    set(OPENSSL_ROOT_DIR "${ANDROID_PREFIX}")
    set(BRLS_PLATFORM_RESOURCES_PATH "\"./resources/\"")
    add_compile_definitions(USE_GLES3 __SDL2__ ANDROID PLATFORM_ANDROID BOREALIS_USE_STD_THREAD)
    add_library(SDL2 SHARED IMPORTED GLOBAL)
    set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${ANDROID_PREFIX}/lib/libSDL2.so"
        INTERFACE_INCLUDE_DIRECTORIES "${ANDROID_PREFIX}/include")
    # SDLActivity provides the Android entry point; no SDL2main archive is needed.
    add_library(SDL2main INTERFACE)
else()
    set(PLATFORM_DESKTOP ON CACHE BOOL "" FORCE)
endif()
set(PLATFORM_SWITCH OFF CACHE BOOL "" FORCE)
set(USE_SDL2 ON CACHE BOOL "" FORCE)
set(USE_SYSTEM_SDL2 ON CACHE BOOL "" FORCE)
set(USE_GLFW OFF CACHE BOOL "" FORCE)
set(USE_STD_THREAD ON CACHE BOOL "" FORCE)
set(USE_LIBROMFS OFF CACHE BOOL "" FORCE)
set(BRLS_RESOURCES_DIR ".")
include("${BOREALIS_LIBRARY}/cmake/commonOption.cmake")
if(NOT ANDROID)
    include("${BOREALIS_LIBRARY}/cmake/toolchain.cmake")
endif()
add_subdirectory("${BOREALIS_LIBRARY}" borealis)

# Build the matching Chiaki SDK in this graph so its headers, ABI and curl
# implementation cannot drift from the library linked into the interface.
foreach(feature IN ITEMS TESTS CLI GUI BOREALIS FFMPEG_DECODER PI_DECODER
        SETSU SPEEX STEAMDECK_NATIVE STEAM_SHORTCUT)
    set(CHIAKI_ENABLE_${feature} OFF CACHE BOOL "" FORCE)
endforeach()
if(ANDROID)
    set(CHIAKI_ENABLE_ANDROID ON CACHE BOOL "" FORCE)
    set(LUNARNX_CHIAKI_SDK_ONLY ON)
    set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
endif()
set(CHIAKI_ENABLE_RUDP ON CACHE BOOL "" FORCE)
set(CHIAKI_LIB_ENABLE_OPUS ON CACHE BOOL "" FORCE)
set(CHIAKI_LIB_ENABLE_LIBNX_CRYPTO OFF CACHE BOOL "" FORCE)
set(CHIAKI_USE_SYSTEM_CURL OFF CACHE BOOL "" FORCE)
set(CHIAKI_USE_SYSTEM_NANOPB OFF CACHE BOOL "" FORCE)
set(CHIAKI_USE_SYSTEM_JERASURE OFF CACHE BOOL "" FORCE)
set(CHIAKI_LIB_JSONC_EXTERNAL_PROJECT OFF CACHE BOOL "" FORCE)
set(CHIAKI_LIB_MINIUPNPC_EXTERNAL_PROJECT OFF CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
# Let OpenSSL use the host's trust store (or SSL_CERT_FILE from AppRun).
set(CURL_CA_BUNDLE none CACHE STRING "" FORCE)
set(CURL_CA_PATH none CACHE STRING "" FORCE)
set(CURL_CA_FALLBACK ON CACHE BOOL "" FORCE)
add_subdirectory("${ROOT}/lib/chiaki-native" chiaki)
target_compile_definitions(chiaki-lib PRIVATE
    LUNARNX_CHIAKI_RECV_OPT=1 LUNARNX_CHIAKI_TRANSPORT_DIAG=0)

find_package(PkgConfig REQUIRED)
if(ANDROID)
    set(NATIVE_GL_LIBS GLESv3 EGL android log m)
else()
    find_package(OpenGL REQUIRED)
    set(NATIVE_GL_LIBS OpenGL::GL)
endif()
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
pkg_check_modules(NATIVE REQUIRED IMPORTED_TARGET
    sdl2 libavcodec libavutil libavformat libswresample libswscale opus openssl)
if(ANDROID)
    set(MBED_LIBRARY_PATH "${ANDROID_PREFIX}/lib")
    set(SRTP_LIBRARY_PATH "${ANDROID_PREFIX}/lib")
    set(SCTP_LIBRARY_PATH "${ANDROID_PREFIX}/lib")
else()
    set(MBED_LIBRARY_PATH "${PEER}/third_party/mbedtls/build_host/library")
    set(SRTP_LIBRARY_PATH "${PEER}/third_party/libsrtp/build_host")
    set(SCTP_LIBRARY_PATH "${PEER}/build/dist/lib")
endif()
foreach(name IN ITEMS mbedtls mbedx509 mbedcrypto)
    find_library(${name}_LIB NAMES ${name} PATHS "${MBED_LIBRARY_PATH}" NO_DEFAULT_PATH REQUIRED)
endforeach()
find_library(SRTP_LIB srtp2 PATHS "${SRTP_LIBRARY_PATH}" NO_DEFAULT_PATH REQUIRED)
find_library(SCTP_LIB usrsctp PATHS "${SCTP_LIBRARY_PATH}" NO_DEFAULT_PATH REQUIRED)

# Reuse the maintained desktop source lists, rejecting dynamic Make syntax.
file(READ "${ROOT}/Makefile.desktop" DESKTOP_MAKE)
foreach(language IN ITEMS C CXX)
    string(REGEX MATCH "${language}_SRCS[ \t]*=[^\n]*(\n[ \t]+[^\n]+)*" block "${DESKTOP_MAKE}")
    string(REGEX REPLACE "^[^=]*=" "" block "${block}")
    string(REPLACE "\\" " " block "${block}")
    string(REGEX MATCHALL "[^ \t\r\n]+" sources "${block}")
    foreach(source IN LISTS sources)
        if(NOT source MATCHES "^[a-zA-Z0-9_/.-]+\\.(c|cpp)$")
            message(FATAL_ERROR "Unsupported desktop source expression: ${source}")
        endif()
        if(NOT source MATCHES "^src/ps/" AND
           NOT source MATCHES "^src/platform/desktop_main.cpp$" AND
           NOT source MATCHES "^lib/stubs/")
            list(APPEND CORE_SOURCES "${ROOT}/${source}")
        endif()
    endforeach()
endforeach()
add_library(lunarnx_native_core STATIC ${CORE_SOURCES})
target_compile_definitions(lunarnx_native_core PUBLIC
    $<$<NOT:$<BOOL:${ANDROID}>>:PLATFORM_DESKTOP> LUNARNX_NATIVE_UI
    CONFIG_USE_LWIP=0 CONFIG_USE_USRSCTP=1 CONFIG_DTLS_USE_ECDSA=1
    CONFIG_IFACE_PREFIX="" CONFIG_IPV6=1
    LUNARNX_CURL_FORCE_MBEDTLS=0 LUNARNX_CURL_PROVIDER="OpenSSL")
target_include_directories(lunarnx_native_core PUBLIC
    "${ROOT}/src" "${ROOT}/lib"
    $<$<BOOL:${ANDROID}>:${ANDROID_PREFIX}/include>
    $<$<BOOL:${ANDROID}>:${ANDROID_PREFIX}/include/srtp2>
    "${PEER}/src"
    "${PEER}/third_party/mbedtls/include"
    "${PEER}/third_party/libsrtp/include"
    "${PEER}/third_party/libsrtp/crypto/include"
    "${PEER}/build/dist/include" "${PEER}/build/dist/include/srtp2"
    "${PEER}/third_party/coreHTTP/source/interface")
target_link_libraries(lunarnx_native_core PUBLIC
    PkgConfig::NATIVE CURL::libcurl Threads::Threads ${SCTP_LIB} ${SRTP_LIB}
    ${mbedtls_LIB} ${mbedx509_LIB} ${mbedcrypto_LIB} ${NATIVE_GL_LIBS}
    OpenSSL::SSL OpenSSL::Crypto)

set(UI_SOURCES
    src/ui/ps_activity.cpp src/ui/ps_registration_activity.cpp src/ui/psn_login_activity.cpp
    src/ps/chiaki_crypto_init.cpp
    src/ps/chiaki_log_adapter.cpp
    src/ps/ps_connection_plan.cpp
    src/ps/ps_connection_trace.cpp
    src/ps/ps_console_repository.cpp
    src/ps/ps_credentials.cpp
    src/ps/ps_discovery.cpp
    src/ps/ps_haptics_rumble.cpp
    src/ps/ps_input_mapper.cpp
    src/ps/ps_manager.cpp
    src/ps/ps_media_bridge.cpp
    src/ps/ps_mock_replay_session.cpp
    src/ps/ps_motion_reader.cpp
    src/ps/ps_pairing_account.cpp
    src/ps/ps_pairing_account_codec.cpp
    src/ps/ps_pairing_account_store.cpp
    src/ps/ps_phone_pairing_server.cpp
    src/ps/ps_registration.cpp
    src/ps/ps_registration_diagnostics.cpp
    src/ps/ps_remote_connector.cpp
    src/ps/ps_route_preferences.cpp
    src/ps/ps_stream_controller.cpp
    src/ps/ps_stream_session.cpp
    src/ps/ps_touchpad_reader.cpp
    src/ps/psn_auth_manager.cpp
    src/ps/psn_auth_utils.cpp
    src/ps/psn_callback_server.cpp
    src/platform/native_gamepad.cpp
    src/input/button_mapping.cpp src/auth/xbox_login_server.cpp
    src/ui/about_activity.cpp src/ui/auth_activity.cpp
    src/ui/button_mapping_activity.cpp src/ui/cloud_game_card.cpp
    src/ui/cloud_library_model.cpp src/ui/i18n.cpp src/ui/main_activity.cpp
    src/ui/perf_overlay.cpp src/ui/platform_activity.cpp src/ui/poster_loader.cpp
    src/ui/ps_settings_activity.cpp src/ui/qr_code.cpp src/ui/qr_code_view.cpp
    src/ui/recycling_list.cpp src/ui/stream_loading_activity.cpp
    src/ui/stream_overlay.cpp src/ui/stream_settings_activity.cpp
    src/ui/stream_view.cpp src/ui/ui_style.cpp)
list(TRANSFORM UI_SOURCES PREPEND "${ROOT}/")
add_library(lunarnx_native_views STATIC ${UI_SOURCES})
file(STRINGS "${ROOT}/version.txt" APP_VERSION LIMIT_COUNT 1)
target_compile_definitions(lunarnx_native_views PRIVATE LUNARNX_VERSION="${APP_VERSION}")
target_link_libraries(lunarnx_native_views PUBLIC lunarnx_native_core borealis chiaki-lib)
if(ANDROID)
    target_link_libraries(lunarnx_native_core PUBLIC SDL2)
    add_library(main SHARED "${ROOT}/src/platform/linux_main.cpp")
    target_link_libraries(main PRIVATE lunarnx_native_views)
    target_link_options(main PRIVATE "-Wl,-z,max-page-size=16384")
else()
    add_executable(LunarNX_Linux "${ROOT}/src/platform/linux_main.cpp")
    target_link_libraries(LunarNX_Linux PRIVATE lunarnx_native_views)
endif()
target_compile_options(lunarnx_native_core PUBLIC
    "-idirafter${PEER}/include" "-idirafter${PEER}/include/sys")
if(NOT ANDROID)
add_executable(native_http_probe "${ROOT}/tests/native_http_probe.cpp")
target_link_libraries(native_http_probe PRIVATE lunarnx_native_core)
add_executable(auth_ui_transport_test "${ROOT}/tests/auth_ui_test.cpp")
target_link_libraries(auth_ui_transport_test PRIVATE lunarnx_native_views)
add_executable(native_ui_settings "${ROOT}/tests/native_ui_settings.cpp")
target_link_libraries(native_ui_settings PRIVATE lunarnx_native_views)
add_executable(native_ps_services "${ROOT}/tests/native_ps_services.cpp")
target_link_libraries(native_ps_services PRIVATE lunarnx_native_views)
target_compile_options(native_ps_services PRIVATE -UNDEBUG)
enable_testing()
add_test(NAME auth_ui_transport COMMAND auth_ui_transport_test)
add_test(NAME native_ps_services COMMAND native_ps_services)
set_tests_properties(native_ps_services PROPERTIES TIMEOUT 30)
add_test(NAME native_ui_settings COMMAND native_ui_settings)
# Resources are installed beside the native executable; main selects that directory.
add_custom_target(native_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT}/lib/borealis/resources" "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT}/romfs" "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${Python3_EXECUTABLE} "${ROOT}/scripts/generate_changelog_resource.py"
            "${ROOT}/CHANGELOG.md" "${CMAKE_BINARY_DIR}/resources/changelog.json")
add_dependencies(LunarNX_Linux native_resources)

endif() # native Linux tests and resource staging
