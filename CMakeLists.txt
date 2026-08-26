CMAKE_MINIMUM_REQUIRED(VERSION 4.2.0)
PROJECT(GooseUI VERSION 0.0.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# ------------------------------- Options -------------------------------
OPTION(GOOSEUI_XORG_SUPPORT "X11 Support" ON)
OPTION(GOOSEUI_WAYLAND_SUPPORT "Wayland Support" OFF)

OPTION(GOOSEUI_INCLUDE_OPENGL "Use OpenGL" ON)
OPTION(GOOSEUI_INCLUDE_VULKAN "Use Vulkan" OFF)

OPTION(GOOSEUI_INSTALL "ExportTargets" ON)
OPTION(GOOSEUI_TESTS "IncludeTests" ON)

# -------------------------------- Files --------------------------------

INCLUDE("${CMAKE_SOURCE_DIR}/cmake/.cmake/core.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/.cmake/widgets.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/.cmake/graphics.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/.cmake/platform.cmake")

# ------------------------------ Create Lib ------------------------------
ADD_LIBRARY(${PROJECT_NAME} ${GOOSEUI_SRC} "include")
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC "include")

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${GOOSEUI_INCLUDE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE ${GOOSEUI_LIBRARY})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME} PRIVATE ${GOOSEUI_BUILD_FLAGS})

if(GOOSEUI_TESTS)
    ADD_SUBDIRECTORY("tests")
endif()

if(GOOSEUI_INSTALL)
    # TODO
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "GooseUI" VERSION ${PROJECT_VERSION} SOVERSION "0" FOLDER "GooseUI")