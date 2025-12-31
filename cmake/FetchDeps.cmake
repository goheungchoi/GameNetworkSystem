include(FetchContent)

option(USE_SYSTEM_ASIO "Use system-installed Asio" OFF)
option(USE_SYSTEM_GLM  "Use system-installed glm"  OFF)
option(USE_SYSTEM_GLFW "Use system-installed glfw" OFF)

set(ASIO_TAG "asio-1-30-2" CACHE STRING "Asio tag")
set(GLM_TAG  "1.0.1"       CACHE STRING "glm tag")
set(GLFW_TAG "3.4"         CACHE STRING "glfw tag")

# -------------------------
# Asio (standalone header-only)
# -> populate the repo and make our own interface target.
# -------------------------
if(USE_SYSTEM_ASIO)
  find_path(ASIO_INCLUDE_DIR NAMES asio.hpp PATH_SUFFIXES asio include/asio)
  if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "USE_SYSTEM_ASIO=ON but Asio headers not found")
  endif()
  add_library(asio_headers INTERFACE)
  target_include_directories(asio_headers INTERFACE "${ASIO_INCLUDE_DIR}")
  target_compile_definitions(asio_headers INTERFACE ASIO_STANDALONE)
else()
  # Silence CMP0169 warning only for this block (Asio has no CMake project)
  cmake_policy(PUSH)
  if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
  endif()

  FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        ${ASIO_TAG}
    GIT_SHALLOW    TRUE
    UPDATE_DISCONNECTED TRUE)
  FetchContent_GetProperties(asio)
  if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
  endif()

  cmake_policy(POP)

  add_library(asio_headers INTERFACE)
  target_include_directories(asio_headers INTERFACE
    "${asio_SOURCE_DIR}/asio/include")
  target_compile_definitions(asio_headers INTERFACE ASIO_STANDALONE)
endif()
# Provide the conventional alias name
add_library(asio::asio ALIAS asio_headers)

# -------------------------
# glm (header-only with CMake)
# -------------------------
if(USE_SYSTEM_GLM)
  find_package(glm CONFIG REQUIRED)   # exports glm::glm
else()
  FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        ${GLM_TAG}
    GIT_SHALLOW    TRUE
    UPDATE_DISCONNECTED TRUE)
  FetchContent_MakeAvailable(glm)    # defines glm::glm
endif()

# -------------------------
# glfw (compiled lib with CMake)
# -------------------------
if(USE_SYSTEM_GLFW)
  find_package(glfw3 CONFIG REQUIRED) # usually exports glfw or glfw::glfw
  if(NOT TARGET glfw::glfw AND TARGET glfw)
    add_library(glfw::glfw ALIAS glfw)
  endif()
else()
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        ${GLFW_TAG}
    GIT_SHALLOW    TRUE
    UPDATE_DISCONNECTED TRUE)
  FetchContent_MakeAvailable(glfw)   # defines glfw
  if(NOT TARGET glfw::glfw AND TARGET glfw)
    add_library(glfw::glfw ALIAS glfw)
  endif()
endif()
