# SPDX-License-Identifier: Apache-2.0
# The QML module a host places in its window: the Terminal surface and the
# session behind it. Static, so a host links footpathplugin and imports
# Footpath.
qt_add_library(footpath STATIC)
qt_add_qml_module(footpath
    URI Footpath
    VERSION 1.0
    QML_FILES Terminal.qml
    SOURCES
        src/OutputModel.h src/OutputModel.cpp
        src/TerminalSession.h src/TerminalSession.cpp
)
target_include_directories(footpath PUBLIC src)
target_link_libraries(footpath PUBLIC Qt6::Quick footpath-core)

if(FOOTPATH_BUILD_TESTS)
    qt_add_executable(tst_terminalsession tests/tst_terminalsession.cpp)
    target_link_libraries(tst_terminalsession PRIVATE Qt6::Test footpath)
    add_test(NAME ui_terminalsession COMMAND tst_terminalsession)

    qt_add_executable(tst_terminal tests/tst_terminal.cpp)
    target_link_libraries(tst_terminal PRIVATE Qt6::QuickTest footpathplugin)
    target_compile_definitions(tst_terminal PRIVATE
        QUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests"
    )
    add_test(NAME ui_terminal COMMAND tst_terminal)

    set_tests_properties(ui_terminalsession ui_terminal PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QML_IMPORT_PATH=${QT_QML_OUTPUT_DIRECTORY}"
    )
endif()
