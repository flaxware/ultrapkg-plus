cmake_minimum_required(VERSION 3.21)
project(ultrapkg-plus VERSION 0.1.0 LANGUAGES CXX)

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

target_compile_features(ultrapkg-plus PRIVATE cxx_std_20)

install(TARGETS ultrapkg-plus DESTINATION bin)
install(FILES data/ultrapkg-plus.desktop DESTINATION share/applications)
install(FILES data/ultrapkg-plus.svg DESTINATION share/icons/hicolor/scalable/apps)
install(FILES data/thunar-actions.xml DESTINATION share/Thunar/actions)
