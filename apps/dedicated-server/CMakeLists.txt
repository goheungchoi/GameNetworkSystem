engine_add_executable(${PROJECT_NAME}-dedicated-server
  "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_NAME "dedicated-server"
  FOLDER "apps/dedicated-server"
  DEPS
    ${PROJECT_NAME}::app-common
    ${PROJECT_NAME}::core
    ${PROJECT_NAME}::runtime
    ${PROJECT_NAME}::network
  OPTIONAL_DEPS
    ${PROJECT_NAME}::server
)
