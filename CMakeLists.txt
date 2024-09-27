cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(steam-static-shortcuts)

add_definitions("-std=c++17")

add_executable(steam-static-shortcuts "${PROJECT_SOURCE_DIR}/src/main.cpp")
set_target_properties(steam-static-shortcuts PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

target_link_libraries(steam-static-shortcuts yaml-cpp)

install(TARGETS steam-static-shortcuts RUNTIME DESTINATION bin)
