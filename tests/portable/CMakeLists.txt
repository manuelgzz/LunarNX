cmake_minimum_required(VERSION 3.20)
project(LunarNXPortableTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()
find_package(Threads REQUIRED)
get_filename_component(LUNARNX_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

# These existing tests exercise shared policies without Switch/SDL/network state.
# Keep assertions enabled even when the runner selects a release configuration.
foreach(name IN ITEMS
    realtime_latency_policy
    video_codec
    adaptive_bitrate_controller
    video_watchdog_policy
    bounded_video_queue_policy
    nv12_texture_geometry
    video_resolution_transition
    operation_generation
    stream_input_router
    native_touchpad)
    add_executable(${name}_test "${LUNARNX_ROOT}/tests/${name}_test.cpp")
    target_link_libraries(${name}_test PRIVATE Threads::Threads)
    target_include_directories(${name}_test PRIVATE "${LUNARNX_ROOT}/src")
    if(MSVC)
        target_compile_options(${name}_test PRIVATE /UNDEBUG)
    else()
        target_compile_options(${name}_test PRIVATE -UNDEBUG)
    endif()
    add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
