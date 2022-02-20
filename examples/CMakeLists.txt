# This file has to be included by node parent, this is not standalone
cmake_minimum_required(VERSION 3.12)
project(micro-containers-examples)
message(\n===========\n${PROJECT_NAME} \n===========\n)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS_RELEASE " -fno-rtti -fno-exceptions -fno-unwind-tables \
                            -fstrict-aliasing -O3  -flto \
                            -Wstrict-aliasing -fno-math-errno \
                            -ffast-math -fno-threadsafe-statics \
                            -fdata-sections -ffunction-sections \
                            " CACHE INTERNAL "" FORCE)

set(libs micro-containers)

set(SOURCES
        test_linked_list.cpp
        test_forward_list.cpp
        test_avl_tree.cpp
        test_dictionary.cpp
        test_ordered_set.cpp
        test_hash_map.cpp
        test_array_map_robin.cpp
        test_array_map_probing.cpp
        test_array_set_robin.cpp
        test_array_set_probing.cpp
        test_hash_set.cpp
        test_array.cpp
        test_static_array.cpp
        test_circular_array.cpp
        test_dynamic_array.cpp
        test_stack.cpp
        test_queue.cpp
        test_heap.cpp
        test_priority_queue.cpp
        test_buffer.cpp
        test_string.cpp
        )

set(SOURCES_SHARED "" src/test_utils.h)

foreach( testsourcefile ${SOURCES} )
    # I used node simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    include_directories("${PROJECT_BINARY_DIR}")

    add_executable( ${testname} ${testsourcefile} ${SOURCES_SHARED} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${testname} ${libs} )
endforeach( testsourcefile ${SOURCES} )

