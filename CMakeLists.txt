cmake_minimum_required(VERSION 3.25)

project(t20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

file(GLOB SOURCES "src/*.c")

if(DEBUG)
  add_compile_definitions(DEBUG=${DEBUG})
endif()

# Packages
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
include_directories(${LIBNOTIFY_INCLUDE_DIRS})

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})

add_executable(t20 ${SOURCES})
install(TARGETS t20 DESTINATION bin)
target_link_libraries(t20 PRIVATE ${LIBNOTIFY_LIBRARIES})
target_link_libraries(t20 PRIVATE ${GTK3_LIBRARIES})