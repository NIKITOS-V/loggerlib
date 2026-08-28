include(FetchContent)

# fmt (приватная зависимость для реализации логирования)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)


add_library(LoggerLib
    ts_file_handler.cpp
    "status.cpp"
    ostream_handler.cpp
    logger.cpp
    loggerlib_funcs.cpp
)

target_sources(LoggerLib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/LoggerLib/Logger.hpp
)

# Настраиваем include директории
target_include_directories(LoggerLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

# fmt подключаем строго как приватную зависимость
target_link_libraries(LoggerLib
    PRIVATE
        $<BUILD_LOCAL_INTERFACE:fmt::fmt>
)

set_target_properties(LoggerLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)