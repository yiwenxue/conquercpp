add_subdirectory(libunifex)
target_compile_features(unifex PUBLIC cxx_std_23)
# target_compile_options(unifex PUBLIC -fcoroutines-ts)

set (SOURCES 
    main.cpp
)

add_executable(fex main.cpp)

target_link_libraries(fex unifex)

# target fex compiles using c++23
target_compile_features(fex PUBLIC cxx_std_23)