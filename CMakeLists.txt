cmake_minimum_required(VERSION 3.22.0)

project(cpp20 CXX)

include(CheckCXXSymbolExists)

add_compile_definitions($<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

function(add_demo)
    set(options)
    set(oneValueArgs ID NAME)
    set(multiValueArgs FILES LIBS)

    cmake_parse_arguments(TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message("${TARGET_ID}: ${TARGET_FILES}")
    add_executable(${TARGET_ID} ${TARGET_FILES})
    target_link_libraries(${TARGET_ID} ${TARGET_LIBS})
endfunction()

add_demo(
    ID split
    FILES
    ranges/split.cpp
)

add_demo(
    ID filter
    FILES
    ranges/filter.cpp
)

add_demo(
    ID view
    FILES
    ranges/view.cpp
)

add_demo(
    ID generator
    FILES
    coroutine/generator.cpp
)

add_demo(
    ID version
    FILES
    others/version.cpp
)