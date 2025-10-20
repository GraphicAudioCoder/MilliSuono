
cmake_minimum_required(VERSION 3.10)
project(MilliSuono)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Genera compile_commands.json per LSP/Intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Target per pulizia completa
add_custom_target(clean-all
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
  COMMENT "Removing all build files"
)

# Libreria statica (fix nome file)
add_library(MilliSuonoLib STATIC
  src/miniaudio_impl.cpp
)
target_include_directories(MilliSuonoLib PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

# Eseguibile principale
add_executable(MilliSuono src/main.cpp)
target_include_directories(MilliSuono PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(MilliSuono MilliSuonoLib)
