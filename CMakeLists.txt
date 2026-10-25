cmake_minimum_required(VERSION 3.21)
project(ultrapkg-plus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

add_executable(ultrapkg-plus
    src/main.cpp
)

target_include_directories(ultrapkg-plus
    PRIVATE
        ${GTKMM_INCLUDE_DIRS}
)

target_link_libraries(ultrapkg-plus
    PRIVATE
        ${GTKMM_LIBRARIES}
)

target_compile_options(ultrapkg-plus
    PRIVATE
        ${GTKMM_CFLAGS_OTHER}
)

install(TARGETS ultrapkg-plus DESTINATION bin)
