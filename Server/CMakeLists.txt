cmake_minimum_required(VERSION 2.8)

project(Server)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_EXE main.cpp)
set(SOURCE_LIB ../Socket/Socket.cpp)
add_library(Socket STATIC ${SOURCE_LIB})
#add_executable(${PROJECT_NAME} "main.cpp")
add_executable(main ${SOURCE_EXE})
target_link_libraries(main Socket)

