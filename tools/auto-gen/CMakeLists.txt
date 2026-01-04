# tools/auto-gen/CMakeLists.txt

add_library(${PROJECT_NAME}-auto-gen-internal
  internal/auto_gen.cpp
  internal/auto_gen.h
)
target_compile_features(${PROJECT_NAME}-auto-gen-internal PUBLIC cxx_std_23)
target_include_directories(${PROJECT_NAME}-auto-gen-internal PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/internal"
)
target_link_libraries(${PROJECT_NAME}-auto-gen-internal PUBLIC
  ${PROJECT_NAME}::core
)
set_target_properties(${PROJECT_NAME}-auto-gen-internal PROPERTIES FOLDER "tools/auto-gen")

engine_add_executable(${PROJECT_NAME}-auto-gen
  "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_NAME "auto-gen"
  FOLDER "tools/auto-gen"
  DEPS
    ${PROJECT_NAME}-auto-gen-internal
)
