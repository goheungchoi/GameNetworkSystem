# cmake/AddEngineExecutable.cmake

function(engine_add_executable TARGET_NAME ROOT_DIR)
  set(options INSTALL)
  set(oneValueArgs OUTPUT_NAME FOLDER MAIN)
  set(multiValueArgs DEPS OPTIONAL_DEPS DEFINITIONS INCLUDE_DIRS)
  cmake_parse_arguments(A "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # Validate inputs (CMake-safe)
  if("${TARGET_NAME}" STREQUAL "" OR "${ROOT_DIR}" STREQUAL "")
    message(FATAL_ERROR "engine_add_executable: usage: engine_add_executable(<target> <root_dir> ...)")
  endif()

  # Normalize ROOT_DIR to an absolute path (helps with EXISTS + globbing)
  if(NOT IS_ABSOLUTE "${ROOT_DIR}")
    set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/${ROOT_DIR}")
  endif()

  # ---- resolve main.cpp ----
  set(_main "")
  if(A_MAIN)
    set(_main "${A_MAIN}")
  else()
    if(EXISTS "${ROOT_DIR}/src/main.cpp")
      set(_main "${ROOT_DIR}/src/main.cpp")
    elseif(EXISTS "${ROOT_DIR}/main.cpp")
      set(_main "${ROOT_DIR}/main.cpp")
    else()
      message(FATAL_ERROR
        "engine_add_executable(${TARGET_NAME}): cannot find main.cpp at "
        "'${ROOT_DIR}/src/main.cpp' or '${ROOT_DIR}/main.cpp'. "
        "Provide MAIN <path>.")
    endif()
  endif()

  # ---- gather non-main sources under root/src ----
  set(_extra_sources "")
  if(EXISTS "${ROOT_DIR}/src")
    file(GLOB_RECURSE _all_cpp CONFIGURE_DEPENDS
      "${ROOT_DIR}/src/*.c"
      "${ROOT_DIR}/src/*.cc"
      "${ROOT_DIR}/src/*.cpp"
      "${ROOT_DIR}/src/*.cxx"
    )
    foreach(f IN LISTS _all_cpp)
      if(NOT f STREQUAL _main)
        list(APPEND _extra_sources "${f}")
      endif()
    endforeach()
  endif()

  # ---- create optional app library if we have extra sources ----
  set(_lib_target "")
  if(_extra_sources)
    set(_lib_target "${TARGET_NAME}-lib")
    add_library(${_lib_target} ${_extra_sources})
    target_compile_features(${_lib_target} PUBLIC cxx_std_23)
  endif()

  # ---- executable ----
  add_executable(${TARGET_NAME} "${_main}")
  target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)

  # ---- include dirs ----
  if(EXISTS "${ROOT_DIR}/include")
    if(_lib_target)
      target_include_directories(${_lib_target} PUBLIC "${ROOT_DIR}/include")
    endif()
    target_include_directories(${TARGET_NAME} PRIVATE "${ROOT_DIR}/include")
  endif()

  if(A_INCLUDE_DIRS)
    if(_lib_target)
      target_include_directories(${_lib_target} PUBLIC ${A_INCLUDE_DIRS})
    endif()
    target_include_directories(${TARGET_NAME} PRIVATE ${A_INCLUDE_DIRS})
  endif()

  # ---- compile definitions ----
  if(A_DEFINITIONS)
    if(_lib_target)
      target_compile_definitions(${_lib_target} PUBLIC ${A_DEFINITIONS})
    endif()
    target_compile_definitions(${TARGET_NAME} PRIVATE ${A_DEFINITIONS})
  endif()

  # ---- link library part (if any) ----
  if(_lib_target)
    target_link_libraries(${TARGET_NAME} PRIVATE ${_lib_target})
  endif()

  # ---- required deps (fail if missing) ----
  foreach(dep IN LISTS A_DEPS)
    if(NOT TARGET ${dep})
      message(FATAL_ERROR
        "engine_add_executable(${TARGET_NAME}): required dependency target '${dep}' does not exist.")
    endif()
  endforeach()

  if(A_DEPS)
    if(_lib_target)
      target_link_libraries(${_lib_target} PUBLIC ${A_DEPS})
    else()
      target_link_libraries(${TARGET_NAME} PRIVATE ${A_DEPS})
    endif()
  endif()

  # ---- optional deps ----
  foreach(dep IN LISTS A_OPTIONAL_DEPS)
    if(TARGET ${dep})
      if(_lib_target)
        target_link_libraries(${_lib_target} PUBLIC ${dep})
      else()
        target_link_libraries(${TARGET_NAME} PRIVATE ${dep})
      endif()
    endif()
  endforeach()

  # ---- properties ----
  if(A_OUTPUT_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${A_OUTPUT_NAME}")
  endif()

  if(A_FOLDER)
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${A_FOLDER}")
    if(_lib_target)
      set_target_properties(${_lib_target} PROPERTIES FOLDER "${A_FOLDER}")
    endif()
  endif()

  # ---- install ----
  if(A_INSTALL)
    install(TARGETS ${TARGET_NAME}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif()
endfunction()
