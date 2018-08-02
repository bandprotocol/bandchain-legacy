cmake_minimum_required(VERSION 3.11.2 FATAL_ERROR)
project(BAND LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Werror -Wextra)

include_directories(abci src)

find_package(Boost 1.67.0 COMPONENTS system REQUIRED)
find_package(Protobuf 3.5.0 REQUIRED)

add_library(abci STATIC
            abci/types.pb.h abci/types.pb.cc)

add_library(server STATIC
            src/net/netapp.h
            src/net/server.h src/net/server.cc
            src/net/session.h src/net/session.cc)

add_library(tendermint STATIC
            src/tendermint/tmapp.h src/tendermint/tmapp.cc)

add_library(util STATIC
            src/util/buffer.h src/util/buffer.cc)

add_executable(welcome src/main/welcome.cc)

add_executable(pingpong src/main/pingpong.cc)
target_link_libraries(pingpong
                      ${Boost_LIBRARIES}
                      util
                      server)
