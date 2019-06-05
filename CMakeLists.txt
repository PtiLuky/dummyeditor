cmake_minimum_required(VERSION 3.10.0)
project(dummyeditor)
include_directories(
    include
    vendor/libdummy/include
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("vendor/libdummy")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Boost 1.69.0 REQUIRED)

macro(GroupSources curdir)
    file(
        GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
        ${PROJECT_SOURCE_DIR}/${curdir}/*
    )
    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child})
        else()
            string(REPLACE "/" "\\" groupname ${curdir})
            source_group(
                ${groupname}
                FILES
                ${PROJECT_SOURCE_DIR}/${curdir}/${child}
            )
        endif()
    endforeach()
endmacro()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.ui")
file(GLOB_RECURSE HEADERS "include/*.hpp")

GroupSources(include)
GroupSources(src)
GroupSources(ui)

add_executable(dummyeditor ${SOURCES} ${HEADERS} icons.qrc)

target_link_libraries(dummyeditor
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Widgets
    dummy
    ${Boost_LIBRARIES}
)

target_include_directories(dummyeditor PUBLIC ${Boost_INCLUDE_DIR})