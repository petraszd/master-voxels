cmake_minimum_required(VERSION 2.6)

set(cmake_build_type Release)

find_package(PkgConfig)
find_package(Qt4 REQUIRED)

pkg_check_modules(OGRE OGRE)

include_directories(${OGRE_CFLAGS} ${OGRE_INCLUDE_DIRS})
include_directories(${QT_INCLUDES})
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MASTER_SRC
    source/main.cpp
    source/mainwindow.cpp
    source/ogrewidget.cpp
    source/ogreapp.cpp
    source/cubeapp.cpp
    source/rttlistener.cpp
    source/textureutil.cpp
    source/cube.cpp
    source/dialogloglistener.cpp
    source/volume.cpp
    source/transfergenerator.cpp
    source/config.cpp
    source/multitexture.cpp
    )

set(MASTER_UI
    ui/mainwindow.ui
    )

set(MASTER_MOC_H
    include/mainwindow.h
    include/ogrewidget.h
    include/transfergenerator.h
    )

set(MASTER_RESOURCES
    resources/painting.qrc
    )

qt4_wrap_cpp(MASTER_MOC_SRC ${MASTER_MOC_H})
qt4_wrap_ui(MASTER_UI_H ${MASTER_UI})
qt4_add_resources(MASTER_RESOURCES_SRC ${MASTER_RESOURCES})

add_executable(master ${MASTER_SRC} ${MASTER_MOC_SRC} ${MASTER_UI_H} ${MASTER_RESOURCES_SRC})
target_link_libraries(master ${OGRE_LIBRARIES})
target_link_libraries(master ${QT_QTCORE_LIBRARY})
target_link_libraries(master ${QT_QTGUI_LIBRARY})

set_property(TARGET master PROPERTY
    RUNTIME_OUTPUT_DIRECTORY
    "${PROJECT_BINARY_DIR}/../bin/")

set(CONVERTER_SRC
    source/converter.cpp
    )

add_executable(converter ${CONVERTER_SRC})
set_property(TARGET converter PROPERTY
    RUNTIME_OUTPUT_DIRECTORY
    "${PROJECT_BINARY_DIR}/../bin/")

