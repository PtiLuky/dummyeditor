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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/exe)

# enable a sub-cmake
add_subdirectory("submodules/dummyRPG")

###############################################################################

set(FORM_FILES
    forms/CinematicsWidget.ui
    forms/GeneralWindow.ui
    forms/mapeditdialog.ui
    forms/mapFloorTreeWidget.ui
    forms/spritesWidget.ui
)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)

add_executable(dummyeditor
    include/editor/project.hpp
    include/utils/definitions.hpp
    include/utils/logger.hpp
    include/widgets/cinematicsWidget.hpp
    include/widgets/generalWindow.hpp
    include/widgets/mapTools.hpp
    include/widgets/spritesWidget.hpp
    include/widgetsMap/chipsetGraphicsScene.hpp
    include/widgetsMap/graphicItem.hpp
    include/widgetsMap/layerItems.hpp
    include/widgetsMap/mapEditDialog.hpp
    include/widgetsMap/mapFloorTreeModel.hpp
    include/widgetsMap/mapFloorTreeWidget.hpp
    include/widgetsMap/mapGraphicsScene.hpp
    include/widgetsMap/mapsTree.hpp

    src/main.cpp
    src/editor/project.cpp
    src/utils/logger.cpp
    src/widgets/cinematicsWidget.cpp
    src/widgets/generalWindow.cpp
    src/widgets/mapTools.cpp
    src/widgets/spritesWidget.cpp
    src/widgetsMap/chipsetGraphicsScene.cpp
    src/widgetsMap/graphicItem.cpp
    src/widgetsMap/layerItems.cpp
    src/widgetsMap/mapEditDialog.cpp
    src/widgetsMap/mapFloorTreeModel.cpp
    src/widgetsMap/mapFloorTreeWidget.cpp
    src/widgetsMap/mapGraphicsScene.cpp
    src/widgetsMap/mapsTree.cpp

    "${FORM_FILES}"
     icons.qrc
)

source_group(_ REGULAR_EXPRESSION "(src|include|forms)/*")
source_group(editor REGULAR_EXPRESSION "(src|include)/editor/*")
source_group(widgets REGULAR_EXPRESSION "(src|include)/widgets/*")
source_group(widgetsMap REGULAR_EXPRESSION "(src|include)/widgetsMap/*")
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
