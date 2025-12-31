# AddEngineModule.cmake
#
# engine_add_module(
#   NAME <short-name>                  # e.g. core, net-socket, net-memstream
#   TARGET <target-name>               # e.g. ${PROJECT_NAME}-core
#   INCLUDE_ROOT <path>                # usually ${PROJECT_SOURCE_DIR}/include
#   HEADER_DIR <subpath-under-namespace> # e.g. core or network/socket
#   SRC_DIR <path>                     # usually ${CMAKE_CURRENT_SOURCE_DIR}/...
#   [NAMESPACE <ns>]                   # public header namespace under include/, default: gamenet
#   [PUBLIC_DEPS ...]
#   [PRIVATE_DEPS ...]
# )
function(engine_add_module)
  set(options)
  set(oneValueArgs NAME TARGET INCLUDE_ROOT HEADER_DIR SRC_DIR NAMESPACE)
  set(multiValueArgs PUBLIC_DEPS PRIVATE_DEPS)
  cmake_parse_arguments(E "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT E_NAME OR NOT E_TARGET OR NOT E_INCLUDE_ROOT OR NOT E_HEADER_DIR OR NOT E_SRC_DIR)
    message(FATAL_ERROR "engine_add_module: missing required args (NAME TARGET INCLUDE_ROOT HEADER_DIR SRC_DIR)")
  endif()

  # HEADER_DIR must be relative (e.g., 'core' or 'network/socket')
  if(E_HEADER_DIR MATCHES "^/|^[A-Za-z]:[/\\]")
    message(FATAL_ERROR "engine_add_module: HEADER_DIR must be a relative path under the namespace (got: '${E_HEADER_DIR}')")
  endif()

  # Full public header subdir under include/
  if (E_NAMESPACE)
    set(_HDR_SUBDIR "${E_NAMESPACE}/${E_HEADER_DIR}")
  endif()

  # Collect sources / public headers
  file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${E_SRC_DIR}/*.c" "${E_SRC_DIR}/*.cc" "${E_SRC_DIR}/*.cpp" "${E_SRC_DIR}/*.m" "${E_SRC_DIR}/*.mm")

  file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.h"
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.hpp"
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.hh")

  # Define target
  add_library(${E_TARGET})

  target_sources(${E_TARGET}
    PRIVATE ${SRC_FILES}
    PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${E_INCLUDE_ROOT}
      FILES ${HDR_FILES}
  )

  # PUBLIC include paths are relocatable (build/install); PRIVATE are for short in-module includes
  target_include_directories(${E_TARGET}
    PUBLIC
      $<BUILD_INTERFACE:${E_INCLUDE_ROOT}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
      "${E_SRC_DIR}"                                  # private headers under src/...
      "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}"              # short path to this module's public headers
  )

  target_compile_features(${E_TARGET} PUBLIC cxx_std_20)

  if(E_PUBLIC_DEPS)
    target_link_libraries(${E_TARGET} PUBLIC ${E_PUBLIC_DEPS})
  endif()
  if(E_PRIVATE_DEPS)
    target_link_libraries(${E_TARGET} PRIVATE ${E_PRIVATE_DEPS})
  endif()

  # Install the target (+ file-set headers)
  install(TARGETS ${E_TARGET}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()
