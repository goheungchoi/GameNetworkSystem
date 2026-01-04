# cmake/AddEngineModule.cmake
#
# add_engine_module(
#   NAME <short-name>
#   TARGET <target-name>
#   INCLUDE_ROOT <path>
#   HEADER_DIR <subpath-under-namespace>
#   SRC_DIR <path>
#   [NAMESPACE <ns>]         # default: gamenet
#   [PUBLIC_DEPS ...]
#   [PRIVATE_DEPS ...]
# )
function(add_engine_module)
  set(options)
  set(oneValueArgs NAME TARGET INCLUDE_ROOT HEADER_DIR SRC_DIR NAMESPACE)
  set(multiValueArgs PUBLIC_DEPS PRIVATE_DEPS)
  cmake_parse_arguments(E "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT E_NAME OR NOT E_TARGET OR NOT E_INCLUDE_ROOT OR NOT E_HEADER_DIR OR NOT E_SRC_DIR)
    message(FATAL_ERROR "add_engine_module: missing required args (NAME TARGET INCLUDE_ROOT HEADER_DIR SRC_DIR)")
  endif()

  if(NOT E_NAMESPACE)
    set(E_NAMESPACE gamenet)
  endif()

  # HEADER_DIR must be relative (e.g., 'core' or 'network/transport')
  if(E_HEADER_DIR MATCHES "^/|^[A-Za-z]:[/\\]")
    message(FATAL_ERROR "add_engine_module: HEADER_DIR must be relative (got: '${E_HEADER_DIR}')")
  endif()

  set(_HDR_SUBDIR "${E_NAMESPACE}/${E_HEADER_DIR}")

  file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${E_SRC_DIR}/*.c" "${E_SRC_DIR}/*.cc" "${E_SRC_DIR}/*.cpp" "${E_SRC_DIR}/*.cxx"
    "${E_SRC_DIR}/*.m" "${E_SRC_DIR}/*.mm"
  )

  file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.h"
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.hpp"
    "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}/*.hh"
  )

  add_library(${E_TARGET})

  if(SRC_FILES)
    target_sources(${E_TARGET} PRIVATE ${SRC_FILES})
  endif()

  set(_HAS_HEADERSET FALSE)
  if(HDR_FILES)
    target_sources(${E_TARGET}
      PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${E_INCLUDE_ROOT}
        FILES ${HDR_FILES}
    )
    set(_HAS_HEADERSET TRUE)
  endif()

  target_include_directories(${E_TARGET}
    PUBLIC
      $<BUILD_INTERFACE:${E_INCLUDE_ROOT}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
      "${E_SRC_DIR}"
      "${E_INCLUDE_ROOT}/${_HDR_SUBDIR}"
  )

  target_compile_features(${E_TARGET} PUBLIC cxx_std_23)

  if(E_PUBLIC_DEPS)
    target_link_libraries(${E_TARGET} PUBLIC ${E_PUBLIC_DEPS})
  endif()
  if(E_PRIVATE_DEPS)
    target_link_libraries(${E_TARGET} PRIVATE ${E_PRIVATE_DEPS})
  endif()

  # Install target (+ headers if present)
  if(_HAS_HEADERSET)
    install(TARGETS ${E_TARGET}
      EXPORT ${PROJECT_NAME}Targets
      FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
  else()
    install(TARGETS ${E_TARGET}
      EXPORT ${PROJECT_NAME}Targets
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
  endif()
endfunction()
