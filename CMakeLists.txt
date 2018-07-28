cmake_minimum_required(VERSION 3.11.2 FATAL_ERROR)
project(BAND LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Werror -Wextra)

include_directories(abci src)

find_package(Boost 1.67.0 COMPONENTS system REQUIRED)
find_package(Protobuf 3.5.0 REQUIRED)

add_library(types STATIC abci/types.pb.cc)

add_executable(hello src/main/hello.cc)

add_executable(pingpong src/main/pingpong.cc)
target_link_libraries(pingpong ${Boost_LIBRARIES})
