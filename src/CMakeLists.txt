# src/CMakeLists.txt

option(ENGINE_BUILD_RUNTIME "Build runtime module" ON)
option(ENGINE_BUILD_SERVER  "Build server framework module" OFF)

function(gamenet_add_module short)
  cmake_parse_arguments(M "" "HEADER_DIR;SRC_SUBDIR" "PUBLIC_DEPS;PRIVATE_DEPS" ${ARGN})

  if(NOT M_HEADER_DIR)
    message(FATAL_ERROR "gamenet_add_module(${short}): HEADER_DIR is required")
  endif()

  if(NOT M_SRC_SUBDIR)
    set(M_SRC_SUBDIR "${M_HEADER_DIR}")
  endif()

  set(_target "${PROJECT_NAME}-${short}")

  add_engine_module(
    NAME ${short}
    TARGET ${_target}
    INCLUDE_ROOT ${PROJECT_SOURCE_DIR}/include
    NAMESPACE gamenet
    HEADER_DIR ${M_HEADER_DIR}
    SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${M_SRC_SUBDIR}
    PUBLIC_DEPS ${M_PUBLIC_DEPS}
    PRIVATE_DEPS ${M_PRIVATE_DEPS}
  )

  add_library(${PROJECT_NAME}::${short} ALIAS ${_target})
endfunction()

# ---- Core ---------------------------------------------------------
if(ENGINE_BUILD_CORE)
  gamenet_add_module(core
    HEADER_DIR core
    SRC_SUBDIR core
  )
endif()

# ---- Runtime ------------------------------------------------------
if(ENGINE_BUILD_RUNTIME)
  gamenet_add_module(runtime
    HEADER_DIR runtime
    SRC_SUBDIR runtime
    PUBLIC_DEPS ${PROJECT_NAME}::core
  )
endif()

# ---- Network ------------------------------------------------------
if(ENGINE_BUILD_NETWORK)

  gamenet_add_module(net-transport
    HEADER_DIR network/transport
    SRC_SUBDIR network/transport
    PUBLIC_DEPS ${PROJECT_NAME}::core
  )

  if(WIN32)
    target_compile_definitions(${PROJECT_NAME}-net-transport
      PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0601
    )
    target_link_libraries(${PROJECT_NAME}-net-transport PRIVATE ws2_32)
  else()
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}-net-transport PRIVATE Threads::Threads)
  endif()

  gamenet_add_module(net-protocol
    HEADER_DIR network/protocol
    SRC_SUBDIR network/protocol
    PUBLIC_DEPS ${PROJECT_NAME}::core
  )

  gamenet_add_module(net-replication
    HEADER_DIR network/replication
    SRC_SUBDIR network/replication
    PUBLIC_DEPS
      ${PROJECT_NAME}::core
      ${PROJECT_NAME}::runtime
      ${PROJECT_NAME}::net-protocol
  )

  gamenet_add_module(net-endpoint
    HEADER_DIR network/endpoint
    SRC_SUBDIR network/endpoint
    PUBLIC_DEPS
      ${PROJECT_NAME}::core
      ${PROJECT_NAME}::runtime
      ${PROJECT_NAME}::net-transport
      ${PROJECT_NAME}::net-protocol
      ${PROJECT_NAME}::net-replication
  )

  add_library(${PROJECT_NAME}-network INTERFACE)
  target_link_libraries(${PROJECT_NAME}-network INTERFACE
    ${PROJECT_NAME}::net-transport
    ${PROJECT_NAME}::net-protocol
    ${PROJECT_NAME}::net-replication
    ${PROJECT_NAME}::net-endpoint
  )
  add_library(${PROJECT_NAME}::network ALIAS ${PROJECT_NAME}-network)

endif()

# ---- Server (optional) -------------------------------------------
if(ENGINE_BUILD_SERVER)
  file(GLOB_RECURSE _srv_src CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/server/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/*.cxx"
  )
  file(GLOB_RECURSE _srv_hdr CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/gamenet/server/*.h"
    "${PROJECT_SOURCE_DIR}/include/gamenet/server/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/gamenet/server/*.hh"
  )

  if(_srv_src OR _srv_hdr)
    gamenet_add_module(server
      HEADER_DIR server
      SRC_SUBDIR server
      PUBLIC_DEPS
        ${PROJECT_NAME}::core
        ${PROJECT_NAME}::runtime
        ${PROJECT_NAME}::network
    )
  else()
    message(STATUS "ENGINE_BUILD_SERVER=ON but server module has no files; skipping.")
  endif()
endif()
