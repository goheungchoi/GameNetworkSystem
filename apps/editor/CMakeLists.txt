engine_add_executable(${PROJECT_NAME}-editor
  "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_NAME "editor"
  FOLDER "apps/editor"
  DEPS
    ${PROJECT_NAME}::app-common
    ${PROJECT_NAME}::core
    ${PROJECT_NAME}::runtime
)
