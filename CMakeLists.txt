cmake_minimum_required(VERSION 3.11.2 FATAL_ERROR)
project(BAND LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Werror -Wextra)

find_package(Boost 1.67.0 COMPONENTS system REQUIRED)
find_package(Protobuf 3.5.0 REQUIRED)
find_package(Sodium REQUIRED)

include_directories(. src)

add_library(abci STATIC
            abci/abci.pb.h abci/abci.pb.cc)

add_library(net STATIC
            src/net/netapp.h
            src/net/server.h src/net/server.cc
            src/net/session.h src/net/session.cc
            src/net/tmapp.h src/net/tmapp.cc)

add_library(util STATIC
            src/util/enum.h
            src/util/buffer.h src/util/buffer.cc
            src/util/log.h src/util/log.cc)

add_executable(counter src/main/counter.cc)
add_executable(cryptotest src/main/cryptotest.cc)
add_executable(welcome src/main/welcome.cc)
add_executable(pingpong src/main/pingpong.cc)


target_link_libraries(cryptotest
                      ${sodium_LIBRARY_RELEASE})
target_link_libraries(pingpong
                      ${Boost_LIBRARIES}
                      net
                      util)
target_link_libraries(counter
                      ${Boost_LIBRARIES}
                      ${Protobuf_LIBRARIES}
                      abci
                      net
                      util)
