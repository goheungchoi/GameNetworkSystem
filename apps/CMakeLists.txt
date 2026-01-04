# apps/CMakeLists.txt
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

option(ENGINE_BUILD_APPS "Build applications" ON)
if(NOT ENGINE_BUILD_APPS)
  return()
endif()

add_subdirectory(common)
add_subdirectory(game)
add_subdirectory(editor)
add_subdirectory(dedicated-server)
