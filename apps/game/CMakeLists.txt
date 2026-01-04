engine_add_executable(${PROJECT_NAME}-game
  "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_NAME "game"
  FOLDER "apps/game"
  DEPS
    ${PROJECT_NAME}::app-common
    ${PROJECT_NAME}::core
    ${PROJECT_NAME}::runtime
    ${PROJECT_NAME}::network
)
