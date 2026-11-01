cmake_minimum_required(VERSION 3.20)
project(LunarNXNativeUI LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(PEER "${ROOT}/lib/libpeer")
set(BOREALIS_LIBRARY "${ROOT}/lib/borealis/library")

set(PLATFORM_DESKTOP ON CACHE BOOL "" FORCE)
set(PLATFORM_SWITCH OFF CACHE BOOL "" FORCE)
set(USE_SDL2 ON CACHE BOOL "" FORCE)
set(USE_SYSTEM_SDL2 ON CACHE BOOL "" FORCE)
set(USE_GLFW OFF CACHE BOOL "" FORCE)
set(USE_STD_THREAD ON CACHE BOOL "" FORCE)
set(USE_LIBROMFS OFF CACHE BOOL "" FORCE)
set(BRLS_RESOURCES_DIR ".")
include("${BOREALIS_LIBRARY}/cmake/commonOption.cmake")
include("${BOREALIS_LIBRARY}/cmake/toolchain.cmake")
add_subdirectory("${BOREALIS_LIBRARY}" borealis)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
pkg_check_modules(NATIVE REQUIRED IMPORTED_TARGET
    sdl2 libavcodec libavutil libswresample libswscale opus libcurl openssl)
foreach(name IN ITEMS mbedtls mbedx509 mbedcrypto)
    find_library(${name}_LIB NAMES ${name}
        PATHS "${PEER}/third_party/mbedtls/build_host/library" NO_DEFAULT_PATH REQUIRED)
endforeach()
find_library(SRTP_LIB srtp2 PATHS "${PEER}/third_party/libsrtp/build_host" NO_DEFAULT_PATH REQUIRED)
find_library(SCTP_LIB usrsctp PATHS "${PEER}/build/dist/lib" NO_DEFAULT_PATH REQUIRED)

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
    PLATFORM_DESKTOP LUNARNX_NATIVE_UI
    CONFIG_USE_LWIP=0 CONFIG_USE_USRSCTP=1 CONFIG_DTLS_USE_ECDSA=1
    CONFIG_IFACE_PREFIX="" CONFIG_IPV6=1)
target_include_directories(lunarnx_native_core PUBLIC
    "${ROOT}/src" "${ROOT}/lib"
    "${PEER}/src"
    "${PEER}/third_party/mbedtls/include"
    "${PEER}/third_party/libsrtp/include"
    "${PEER}/third_party/libsrtp/crypto/include"
    "${PEER}/build/dist/include" "${PEER}/build/dist/include/srtp2"
    "${PEER}/third_party/coreHTTP/source/interface")
target_link_libraries(lunarnx_native_core PUBLIC
    PkgConfig::NATIVE Threads::Threads ${SCTP_LIB} ${SRTP_LIB}
    ${mbedtls_LIB} ${mbedx509_LIB} ${mbedcrypto_LIB} OpenGL::GL
    OpenSSL::SSL OpenSSL::Crypto)

set(UI_SOURCES
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
target_link_libraries(lunarnx_native_views PUBLIC lunarnx_native_core borealis)
add_executable(LunarNX_Linux "${ROOT}/src/platform/linux_main.cpp")
target_link_libraries(LunarNX_Linux PRIVATE lunarnx_native_views)
add_executable(native_ui_settings "${ROOT}/tests/native_ui_settings.cpp")
target_link_libraries(native_ui_settings PRIVATE lunarnx_native_views)
enable_testing()
add_test(NAME native_ui_settings COMMAND native_ui_settings)
target_compile_options(lunarnx_native_core PUBLIC
    "-idirafter${PEER}/include" "-idirafter${PEER}/include/sys")
# Resources are installed beside the native executable; main selects that directory.
add_custom_target(native_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT}/lib/borealis/resources" "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT}/romfs" "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${Python3_EXECUTABLE} "${ROOT}/scripts/generate_changelog_resource.py"
            "${ROOT}/CHANGELOG.md" "${CMAKE_BINARY_DIR}/resources/changelog.json")
add_dependencies(LunarNX_Linux native_resources)
