### SVG Maze Generator
cmake_minimum_required(VERSION 3.16)

project(svgmaze
  LANGUAGES C
  VERSION 0.0.1
)
set(CMAKE_C_STANDARD 11)
add_compile_options(-W -Wall -Wextra -Wpedantic -Werror)

## Capture Build Information:
# the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND
  git describe --match=NeVeRmAtCh --always --abbrev=8 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND
  git log -1 --format=%ad --date=iso8601-strict
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the subject of the commit
execute_process(COMMAND
  git log -1 --format=%s
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SUBJECT
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the build
execute_process(COMMAND
  date --utc +%Y-%m-%dT%H:%M:%S%:z
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE BUILD_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# Send the queried variables into a autogen header
configure_file(version.h.in ${CMAKE_BINARY_DIR}/include/version.h
  @ONLY
  NEWLINE_STYLE UNIX
)

# This makes sure the version.h is generated
add_custom_target(regenerate_version_header ALL
  DEPENDS ${CMAKE_SOURCE_DIR}/version.h.in
  COMMAND touch ${CMAKE_SOURCE_DIR}/version.h.in
  COMMENT "Touching version.h"
)

## Executable: svgmaze
set(SOURCES
  src/strings.c
  src/prng.c
  src/grid.c
  src/maze.c
  src/main.c
)

add_executable(svgmaze ${SOURCES})
add_dependencies(svgmaze regenerate_version_header)
target_include_directories(svgmaze PRIVATE "${PROJECT_BINARY_DIR}/include")
