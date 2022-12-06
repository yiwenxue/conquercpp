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

# add_demo(
#     ID split
#     FILES
#     ranges/split.cpp
# )

# add_demo(
#     ID filter
#     FILES
#     ranges/filter.cpp
# )

# add_demo(
#     ID view
#     FILES
#     ranges/view.cpp
# )

add_demo(
    ID consumer
    FILES
    coroutine/consumer.cpp
)

add_demo(
    ID lazy_iterator
    FILES
    coroutine/lazy_iterator.cpp
)

add_demo(
    ID test
    FILES
    coroutine/test.cpp
)

add_demo(
    ID generator
    FILES
    coroutine/generator.cpp
)

# add_demo(
#     ID version
#     FILES
#     others/version.cpp
# )

# add_demo(
#     ID variant
#     FILES
#     others/variant.cpp
# )

# add_demo(
#     ID optional
#     FILES
#     others/optional.cpp
# )

# add_demo(
#     ID format
#     FILES
#     others/format.cpp
# )

# add_demo(
#     ID concept
#     FILES
#     concept/concept.cpp
# )

# add_demo(
#     ID bvh
#     FILES
#     tools/bvh.cpp
# )

# add_demo(
#     ID vector
#     FILES
#     tools/vector.cpp
# )

add_subdirectory(unifex)

add_subdirectory(graph)

add_demo(
    ID power_set
    FILES
    tools/power_set.cpp
)