# Find GoogleTest
enable_testing()

macro(add_gtest test_name)
    add_executable(${test_name} ${test_name}.cpp)
    
    if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
        target_link_libraries(${test_name} gtest gtest_main pthread)
        target_compile_options(${test_name} PUBLIC --save-temps)
    else()
        target_link_libraries(${test_name} gtest gtest_main)
    endif()
    
    target_include_directories(${test_name} PRIVATE ${GOOGLE_TEST_H} ${GTR_CONTAINERS})
    
    # Set output directories to bin/test
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test"
    )
    
    # Ensure the bin/test directory exists
    file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test")
    file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test")
    file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test")
    
    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endmacro()

add_gtest(static_string_test)
add_gtest(matrix_test)
add_gtest(hashmap_test)
add_gtest(linkedlist_test)
add_gtest(stack_test)
add_gtest(queue_test)
add_gtest(text_test)
add_gtest(vector_test)
add_gtest(hashset_test)
add_gtest(deque_test)
add_gtest(redblack_tree_test)
add_gtest(tracking_allocator_test)

if (UNIX)
macro(add_benchmark benchmark_name)
    add_executable(${benchmark_name} ${benchmark_name}.cpp)
    if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")) 
        target_compile_options(${benchmark_name} PUBLIC --save-temps)
    endif()
    target_link_libraries(${benchmark_name} benchmark::benchmark)
    target_include_directories(${benchmark_name} PRIVATE ${GOOGLE_BENCH_H} ${GTR_CONTAINERS})
endmacro()

add_benchmark(static_string_benchmark)
add_benchmark(hashmap_benchmark)
add_benchmark(matrix_bench)
add_benchmark(vector_benchmark)
add_benchmark(deque_benchmark)
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wpedantic")
endif()
endif()
