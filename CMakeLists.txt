cmake_minimum_required(VERSION 3.26)

project(GameNet
  VERSION 0.1.0
  LANGUAGES CXX)

# ----------------------------
# Options
# ----------------------------
option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)
option(BUILD_TESTING     "Build tests"                ON)
option(ENABLE_WARNINGS   "Enable compiler warnings"   ON)

# Per-module toggles (used by src/CMakeLists.txt)
option(ENGINE_BUILD_CORE    "Build core module"    ON)
option(ENGINE_BUILD_NETWORK "Build network modules" ON)

# ----------------------------
# C++ / global settings
# ----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32 AND BUILD_SHARED_LIBS)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # quick start; prefer proper export macros later
endif()

if(ENABLE_WARNINGS)
  if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()
endif()

# ----------------------------
# Modules / deps
# ----------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(AddEngineExecutable)     # defines add_engine_app(...)
include(AddEngineModule)  # defines add_engine_module(...)
include(FetchDeps)        # FetchContent for asio::asio, glm::glm, glfw::glfw

# ----------------------------
# Targets
# ----------------------------
add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tools)

# ----------------------------
# Tests
# ----------------------------
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# ----------------------------
# Install / package export
# ----------------------------

# Export the targets of this project
install(EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Generate a find_package() config
include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
