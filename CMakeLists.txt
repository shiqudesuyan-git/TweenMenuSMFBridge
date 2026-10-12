cmake_minimum_required(VERSION 3.24)
project(TweenMenuSMFBridge VERSION 1.0.0 LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This plugin requires a Windows x64 toolchain.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(COMMONLIBSSE_SOURCE_DIR "" CACHE PATH "Path to a CommonLibSSE-NG source checkout")
option(BUILD_BRIDGE_TESTS "Build the headless bridge integration checks" ON)

if(COMMONLIBSSE_SOURCE_DIR)
    set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
    set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
    set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory("${COMMONLIBSSE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/commonlib" EXCLUDE_FROM_ALL)
    include("${COMMONLIBSSE_SOURCE_DIR}/cmake/CommonLibSSE.cmake")
else()
    find_package(CommonLibSSE CONFIG REQUIRED)
endif()

add_library(FrameworkClient STATIC src/FrameworkClient.cpp)
target_include_directories(FrameworkClient PUBLIC src PRIVATE vendor/smf)
target_compile_definitions(FrameworkClient PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

add_commonlibsse_plugin(TweenMenuSMFBridge
    AUTHOR "TweenMenuSMFBridge contributors"
    SOURCES src/Plugin.cpp)
target_link_libraries(TweenMenuSMFBridge PRIVATE FrameworkClient)
target_include_directories(TweenMenuSMFBridge PRIVATE src)
target_compile_definitions(TweenMenuSMFBridge PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_precompile_headers(TweenMenuSMFBridge PRIVATE src/PCH.h)
if(MSVC)
    target_compile_options(TweenMenuSMFBridge PRIVATE /permissive- /Zc:preprocessor /EHsc /W4)
endif()

install(FILES "$<TARGET_FILE:TweenMenuSMFBridge>" DESTINATION SKSE/Plugins COMPONENT Mod)
install(DIRECTORY package/ DESTINATION . COMPONENT Mod)
install(FILES README.zh-CN.md README.md LICENSE THIRD_PARTY_NOTICES.md VALIDATION.md DESTINATION . COMPONENT Mod)
install(DIRECTORY licenses/ DESTINATION licenses COMPONENT Mod)

if(BUILD_BRIDGE_TESTS)
    enable_testing()
    add_library(FakeFramework SHARED tests/FakeFramework.cpp)
    target_include_directories(FakeFramework PRIVATE vendor/smf)
    target_compile_definitions(FakeFramework PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    set_target_properties(FakeFramework PROPERTIES OUTPUT_NAME SKSEMenuFramework
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/fixtures/current")
    add_library(FakeLegacyFramework SHARED tests/FakeLegacyFramework.cpp)
    set_target_properties(FakeLegacyFramework PROPERTIES OUTPUT_NAME SKSEMenuFramework
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/fixtures/legacy")
    add_executable(BridgeChecks tests/BridgeChecks.cpp)
    target_link_libraries(BridgeChecks PRIVATE FrameworkClient)
    target_compile_definitions(BridgeChecks PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    add_test(NAME bridge_integration COMMAND BridgeChecks
        "$<TARGET_FILE:FakeFramework>" "$<TARGET_FILE:FakeLegacyFramework>")
endif()

