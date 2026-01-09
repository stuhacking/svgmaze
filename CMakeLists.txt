## SVG Maze Generator
cmake_minimum_required(VERSION 3.16)

project(svgmaze
  LANGUAGES C
  VERSION 0.0.1
)
set(CMAKE_C_STANDARD 11)
add_compile_options(-W -Wall -Wextra -Wpedantic -Werror)

## Build: svgmaze
add_executable(svgmaze src/main.c)
