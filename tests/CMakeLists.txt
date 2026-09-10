add_executable(LoggerLibTests
    test_main.cpp
    test_loggerlib.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(LoggerLibTests PRIVATE LOGGING_MODE=0)
    message(STATUS "Building in DEBUG mode - full logging enabled")

else()
    target_compile_definitions(LoggerLibTests PRIVATE LOGGING_MODE=1)
    message(STATUS "Building with release with debug info - INFO logging")

endif()

target_link_libraries(LoggerLibTests
    PRIVATE
        LoggerLib::LoggerLib
)