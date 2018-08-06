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
find_package(Sodium 1.0.16 REQUIRED)

include_directories(. src)

file(GLOB source_files "src/**/*.h" "src/**/*.cc")

add_library(bandprotocol STATIC
            abci/abci.pb.h
            abci/abci.pb.cc
            ${source_files})

target_link_libraries(bandprotocol
                      ${Boost_LIBRARIES}
                      ${Protobuf_LIBRARIES}
                      ${sodium_LIBRARY_RELEASE})

add_executable(counter src/main/counter.cxx)
add_executable(cryptotest src/main/cryptotest.cxx)
add_executable(welcome src/main/welcome.cxx)
add_executable(pingpong src/main/pingpong.cxx)

target_link_libraries(cryptotest bandprotocol)
target_link_libraries(pingpong bandprotocol)
target_link_libraries(counter bandprotocol)
target_link_libraries(welcome bandprotocol)
