# SPDX-License-Identifier: Apache-2.0
# The interpreter and what it sees, with no GUI dependency, so the trust
# boundary (what the shell refuses) is tested on its own.
qt_add_library(footpath-core STATIC
    src/Completion.h src/Completion.cpp
    src/DidYouMean.h src/DidYouMean.cpp
    src/Hints.h src/Hints.cpp
    src/HomeLayout.h src/HomeLayout.cpp
    src/Interpreter.h src/Interpreter.cpp
    src/Reply.h
    src/World.h src/World.cpp
)
target_include_directories(footpath-core PUBLIC src)
target_link_libraries(footpath-core PUBLIC Qt6::Core)

if(FOOTPATH_BUILD_TESTS)
    foreach(suite world homelayout didyoumean interpreter)
        qt_add_executable(tst_${suite} tests/tst_${suite}.cpp)
        target_link_libraries(tst_${suite} PRIVATE Qt6::Test footpath-core)
        add_test(NAME core_${suite} COMMAND tst_${suite})
    endforeach()
endif()
