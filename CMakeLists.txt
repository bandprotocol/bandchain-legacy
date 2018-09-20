cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(BAND LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall")

find_package(Boost 1.65.1 COMPONENTS system serialization REQUIRED)
find_package(Protobuf 3.0.0 REQUIRED)
find_package(Sodium REQUIRED)
find_package(CxxTest REQUIRED)
find_package(RocksDB REQUIRED)

include_directories(3pc/spdlog-0.17.0/include)
include_directories(3pc/gsl-0cebbd7/include)
include_directories(3pc/json-3.1.2/include)
include_directories(3pc/better-enum-0.11.1/include)
include_directories(. src test)


file(GLOB source_files
     "src/band/*.h" "src/band/*.cc"
     "src/contract/*.h" "src/contract/*.cc"
     "src/crypto/*.h" "src/crypto/*.cc"
     "src/inc/*.h" "src/inc/*.cc"
     "src/msg/*.h" "src/msg/*.cc"
     "src/net/*.h" "src/net/*.cc"
     "src/state/*.h" "src/state/*.cc"
     "src/store/*.h" "src/store/*.cc"
     "src/util/*.h" "src/util/*.cc")

add_library(bandprotocol STATIC
            abci/abci.pb.h
            abci/abci.pb.cc
            ${source_files})

target_link_libraries(bandprotocol
                      ${Boost_LIBRARIES}
                      ${Protobuf_LIBRARIES}
                      ${sodium_LIBRARY_RELEASE}
                      ${ROCKSDB_LIBRARIES})

enable_testing()
file(GLOB test_files "test/*_test.h")
CXXTEST_ADD_TEST(band_test_runner band_test_runner.cc "${test_files}")
target_link_libraries(band_test_runner bandprotocol)
#[[
foreach(test_file ${test_files})
  get_filename_component(filename "${test_file}" NAME)
  string(REGEX REPLACE "\\.[^.]*$" "_runner" executable ${filename})
  string(REGEX REPLACE "\\.[^.]*$" "_runner.cc" test_source ${filename})
  CXXTEST_ADD_TEST(${executable} ${test_source} "${test_file}")
  target_link_libraries(${executable} bandprotocol)
endforeach()
]]

file(GLOB main_files "src/main/*.cc")
foreach(main_file ${main_files})
  get_filename_component(executable "${main_file}" NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" executable ${executable})
  add_executable("${executable}" ${main_file})
  target_link_libraries("${executable}" bandprotocol)
endforeach()
