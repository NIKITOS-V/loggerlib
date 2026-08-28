#[[ В случае подключений "googletest"
include(FetchContent)

# Отключаем установку GTest (не нужно для тестов)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)
]]

add_executable(LoggerLibTests
    test_main.cpp
    test_loggerlib.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(LoggerLibTests PRIVATE LOGGING_MODE=0)
    message(STATUS "Building in DEBUG mode - full logging enabled")

elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(LoggerLibTests PRIVATE LOGGING_MODE=1)
    message(STATUS "Building with release with debug info - INFO logging")

endif()

target_link_libraries(LoggerLibTests
    PRIVATE
        LoggerLib::LoggerLib
        #GTest::gtest
        #GTest::gtest_main
)

#[[ В случае подключений "googletest"
# Автоматическое обнаружение тестов для интеграции с CTest
include(GoogleTest)
#gtest_discover_tests(LoggerLibTests)
]]