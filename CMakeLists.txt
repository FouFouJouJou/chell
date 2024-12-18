cmake_minimum_required(VERSION 3.25)
project(schell VERSION 1.0 LANGUAGES C)
set(SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(EXECUTABLE main) 
add_executable(${EXECUTABLE})

target_include_directories(${EXECUTABLE} PUBLIC ${INCLUDE_DIR})
target_sources(${EXECUTABLE} PUBLIC ${SOURCE_DIRECTORY}/${EXECUTABLE}.c ${SOURCE_DIRECTORY}/lexer.c ${SOURCE_DIRECTORY}/parser.c ${SOURCE_DIRECTORY}/exec.c)
