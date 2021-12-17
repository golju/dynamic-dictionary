cmake_minimum_required (VERSION 3.20)
project(dynamic_dictionary_examples)

add_executable(example0 example0.cpp)
target_include_directories(example0 PUBLIC
        ../include
        ../third-party/doctest
        ../third-party/nlohmann)

set(TEST_DATA_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
set_source_files_properties(example0.cpp PROPERTIES COMPILE_DEFINITIONS "TEST_DATA_FOLDER=\"${TEST_DATA_FOLDER}\"")