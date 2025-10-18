
cmake_minimum_required(VERSION 3.10)
project(MicroSuono)

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
add_library(MicroSuonoLib STATIC
  src/miniaudio_impl.cpp
)
target_include_directories(MicroSuonoLib PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

# Eseguibile principale
add_executable(MicroSuono src/main.cpp)
target_include_directories(MicroSuono PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(MicroSuono MicroSuonoLib)
