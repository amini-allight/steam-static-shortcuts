cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(steam-static-shortcuts)

add_definitions("-std=c++20")

add_executable(steam-static-shortcuts "${PROJECT_SOURCE_DIR}/src/main.cpp")
set_target_properties(steam-static-shortcuts PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_custom_target(manual COMMAND mkdir -p "${PROJECT_SOURCE_DIR}/sys/share/man/man1" && gzip -c "${PROJECT_SOURCE_DIR}/sys/steam-static-shortcuts.1" > "${PROJECT_SOURCE_DIR}/sys/share/man/man1/steam-static-shortcuts.1.gz")
add_dependencies(steam-static-shortcuts manual)

target_link_libraries(steam-static-shortcuts yaml-cpp)

install(TARGETS steam-static-shortcuts RUNTIME DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/sys/share/" DESTINATION share)
