cmake_minimum_required(VERSION 3.10)
project(MilliSuono)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for LSP/Intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Target for complete cleanup
add_custom_target(clean-all
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
  COMMENT "Removing all build files"
)

# Static library (fix file name)
add_library(MilliSuonoLib STATIC
  src/external/miniaudio_impl.cpp
  src/core/Node.cpp
)
target_include_directories(MilliSuonoLib PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

# Main executable
add_executable(MilliSuono src/main.cpp)
target_include_directories(MilliSuono PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(MilliSuono MilliSuonoLib)
