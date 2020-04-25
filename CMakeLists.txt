cmake_minimum_required(VERSION 3.10.0)

project(dummyeditor)

# Specify C++ version
set(CMAKE_CXX_STANDARD 17)

# Qt pre-compilation activation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# depedencies Qt, Boost and Lua
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Widgets REQUIRED)

# enable a sub-cmake
add_subdirectory("submodules/dummyRPG")

###############################################################################

set(FORM_FILES
    forms/CinematicsWidget.ui
    forms/GeneralWindow.ui
    forms/mapeditdialog.ui
    forms/mapFloorTreeWidget.ui
)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)

add_executable(dummyeditor
    include/editor/project.hpp
    include/utils/definitions.hpp
    include/utils/logger.hpp
    include/widgets/chipsetGraphicsScene.hpp
    include/widgets/cinematicsWidget.hpp
    include/widgets/generalWindow.hpp
    include/widgets/graphicItem.hpp
    include/widgets/layerItems.hpp
    include/widgets/mapEditDialog.hpp
    include/widgets/mapFloorTreeModel.hpp
    include/widgets/mapFloorTreeWidget.hpp
    include/widgets/mapGraphicsScene.hpp
    include/widgets/mapsTree.hpp
    include/widgets/mapTools.hpp

    src/main.cpp
    src/editor/project.cpp
    src/utils/logger.cpp
    src/widgets/chipsetGraphicsScene.cpp
    src/widgets/cinematicsWidget.cpp
    src/widgets/generalWindow.cpp
    src/widgets/graphicItem.cpp
    src/widgets/layerItems.cpp
    src/widgets/mapEditDialog.cpp
    src/widgets/mapFloorTreeModel.cpp
    src/widgets/mapFloorTreeWidget.cpp
    src/widgets/mapGraphicsScene.cpp
    src/widgets/mapsTree.cpp
    src/widgets/mapTools.cpp

    "${FORM_FILES}"
     icons.qrc
)

source_group(_ REGULAR_EXPRESSION "(src|include)/*")
source_group(editor REGULAR_EXPRESSION "(src|include)/editor/*")
source_group(widgets REGULAR_EXPRESSION "(src|include)/widgets/*")
source_group(utils REGULAR_EXPRESSION "(src|include)/utils/*")

target_include_directories(dummyeditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(dummyeditor
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    Qt5::Widgets
    dummyrpg)

# Add compilation warnings
if(MSVC)
  target_compile_options(dummyeditor PRIVATE /W4 /W14640)
else()
  target_compile_options(dummyeditor PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
endif()
