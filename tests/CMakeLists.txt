cmake_minimum_required (VERSION 3.20)
project(dynamic_dictionary_tests)

add_executable(tests tests.cpp doctest_main.cpp)
target_include_directories(tests PUBLIC
                            ../include
                            ../third-party/doctest
                            ../third-party/nlohmann)