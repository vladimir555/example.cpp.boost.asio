cmake_minimum_required(VERSION 3.5)


project(example.cpp.boost.asio LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_BUILD_TYPE "Debug")


find_package(Boost COMPONENTS system log log_setup)
add_compile_definitions(
    BOOST_ALL_DYN_LINK
    BOOST_LOG_USE_NATIVE_SYSLOG
)


include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/source/server
)
file(GLOB_RECURSE SOURCE_SERVER
    ${CMAKE_SOURCE_DIR}/source/server/*.h
    ${CMAKE_SOURCE_DIR}/source/server/*.cpp
)
add_executable(${PROJECT_NAME}.server ${SOURCE_SERVER})
target_link_libraries(
    ${PROJECT_NAME}.server
    ${Boost_LIBRARIES}
)


include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/source/client
)
file(GLOB_RECURSE SOURCE_CLIENT
    ${CMAKE_SOURCE_DIR}/source/client/*.h
    ${CMAKE_SOURCE_DIR}/source/client/*.cpp
)
add_executable(${PROJECT_NAME}.client ${SOURCE_CLIENT})
target_link_libraries(
    ${PROJECT_NAME}.client
    ${Boost_LIBRARIES}
)

