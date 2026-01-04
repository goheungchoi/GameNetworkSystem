# tools/CMakeLists.txt

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

option(ENGINE_BUILD_TOOLS "Build tools" ON)
if(NOT ENGINE_BUILD_TOOLS)
  return()
endif()

add_subdirectory(auto-gen)
