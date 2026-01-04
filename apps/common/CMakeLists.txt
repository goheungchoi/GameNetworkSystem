# apps/common/CMakeLists.txt

add_library(${PROJECT_NAME}-app-common
  src/app_base.cpp
)

target_compile_features(${PROJECT_NAME}-app-common PUBLIC cxx_std_23)
target_include_directories(${PROJECT_NAME}-app-common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}-app-common PUBLIC
  ${PROJECT_NAME}::core
)

add_library(${PROJECT_NAME}::app-common ALIAS ${PROJECT_NAME}-app-common)
set_target_properties(${PROJECT_NAME}-app-common PROPERTIES FOLDER "apps/common")
