cmake_minimum_required(VERSION 3.11.2 FATAL_ERROR)
project(BAND LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Werror -Wextra)
add_definitions(-include src/util/autoinclude.h)

find_package(Boost 1.67.0 COMPONENTS system REQUIRED)
find_package(Protobuf 3.5.0 REQUIRED)
find_package(Sodium REQUIRED)
find_package(CxxTest REQUIRED)

include_directories(. src test)

file(GLOB source_files "src/**/*.h" "src/**/*.cc")
add_library(bandprotocol STATIC
            abci/abci.pb.h
            abci/abci.pb.cc
            ${source_files})

enable_testing()
file(GLOB test_files "test/*_test.h")
CXXTEST_ADD_TEST(band_test_runner foo_test.cc "${test_files}")
target_link_libraries(band_test_runner bandprotocol)

target_link_libraries(bandprotocol
                      ${Boost_LIBRARIES}
                      ${Protobuf_LIBRARIES}
                      ${sodium_LIBRARY_RELEASE})

file(GLOB main_files "src/main/*.cxx")
foreach(main_file ${main_files})
  get_filename_component(executable "${main_file}" NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" executable ${executable})
  add_executable("${executable}" ${main_file})
  target_link_libraries("${executable}" bandprotocol)
endforeach()
