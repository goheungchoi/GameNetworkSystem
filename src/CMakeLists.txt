# Core
if(ENGINE_BUILD_CORE)
  engine_add_module(
    NAME core
    TARGET ${PROJECT_NAME}-core
    INCLUDE_ROOT ${PROJECT_SOURCE_DIR}/include
    NAMESPACE gamenet
    HEADER_DIR core
    SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core
  )
  add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}-core)
endif()

# Network: memory-stream
if(ENGINE_BUILD_NETWORK)
  engine_add_module(
    NAME net-memstream
    TARGET ${PROJECT_NAME}-net-memstream
    INCLUDE_ROOT ${PROJECT_SOURCE_DIR}/include
    NAMESPACE gamenet
    HEADER_DIR network/memory-stream
    SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/memory-stream
    PUBLIC_DEPS ${PROJECT_NAME}-core
  )
  add_library(${PROJECT_NAME}::net-memstream ALIAS ${PROJECT_NAME}-net-memstream)

  # Network: socket
  engine_add_module(
    NAME net-socket
    TARGET ${PROJECT_NAME}-net-socket
    INCLUDE_ROOT ${PROJECT_SOURCE_DIR}/include
    NAMESPACE gamenet
    HEADER_DIR network/socket
    SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/socket
    PUBLIC_DEPS ${PROJECT_NAME}-core
  )

  if(WIN32)
    target_compile_definitions(${PROJECT_NAME}-net-socket PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0601)
    target_link_libraries(${PROJECT_NAME}-net-socket PRIVATE ws2_32)
  else()
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}-net-socket PRIVATE Threads::Threads)
  endif()

  add_library(${PROJECT_NAME}::net-socket ALIAS ${PROJECT_NAME}-net-socket)

  # Umbrella "network" meta-target that pulls both
  add_library(${PROJECT_NAME}-network INTERFACE)
  target_link_libraries(${PROJECT_NAME}-network INTERFACE
    ${PROJECT_NAME}-net-memstream
    ${PROJECT_NAME}-net-socket)
  add_library(${PROJECT_NAME}::network ALIAS ${PROJECT_NAME}-network)

  install(TARGETS ${PROJECT_NAME}-network EXPORT ${PROJECT_NAME}Targets)
endif()
