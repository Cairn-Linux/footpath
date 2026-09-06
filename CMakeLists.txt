# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.28)

project(footpath VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Built on its own, or inside a host's tree (Cairn adds this directory as a
# submodule). The host's flags apply then; these are for building alone.
if(PROJECT_IS_TOP_LEVEL)
    add_compile_options(-Wall -Wextra -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    endif()
    set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")
endif()
option(FOOTPATH_BUILD_TESTS "Build Footpath's tests" ${PROJECT_IS_TOP_LEVEL})

find_package(Qt6 REQUIRED COMPONENTS Core Quick LinguistTools)
if(FOOTPATH_BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test QuickTest)
    enable_testing()
endif()
qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

add_subdirectory(core)
add_subdirectory(ui)

qt_add_lupdate(
    SOURCE_TARGETS footpath-core footpath
    TS_FILES i18n/footpath_en.ts
    LUPDATE_TARGET footpath_lupdate
)
