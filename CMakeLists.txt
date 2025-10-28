cmake_minimum_required(VERSION 3.10)
project(MilliSuono)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/external
)

add_library(MilliSuonoLib STATIC
  src/external/miniaudio_impl.cpp
  src/core/Node.cpp
)

add_executable(MilliSuono src/main.cpp)
target_link_libraries(MilliSuono MilliSuonoLib)
