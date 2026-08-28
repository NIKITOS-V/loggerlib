cmake_minimum_required(VERSION 3.26)
project(LoggerLib VERSION 1.0.0 LANGUAGES CXX)

# ============================================================
# НАСТРОЙКИ C++
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# ОПЦИИ СБОРКИ
# ============================================================

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

# ============================================================
# ОСНОВНАЯ БИБЛИОТЕКА
# ============================================================

add_subdirectory(src)
# Создаём ALIAS для удобства пользователей (LoggerLib::LoggerLib)
add_library(LoggerLib::LoggerLib ALIAS LoggerLib)

# ============================================================
# ТЕСТЫ
# ============================================================

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================
# УСТАНОВКА
# ============================================================

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Конфигурационный файл для find_package
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LoggerLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LoggerLibConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LoggerLib
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LoggerLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Устанавливаем библиотеку
install(
    TARGETS LoggerLib
    EXPORT LoggerLibTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Устанавливаем заголовки
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Устанавливаем CMake конфиги
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/LoggerLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LoggerLibConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LoggerLib
)

# Экспортируем цели
install(
    EXPORT LoggerLibTargets
    FILE LoggerLibTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LoggerLib
)

# ============================================================
# ВЫВОД ИНФОРМАЦИИ
# ============================================================

message(STATUS "LoggerLib version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Shared libs: ${BUILD_SHARED_LIBS}")