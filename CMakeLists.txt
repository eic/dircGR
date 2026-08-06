cmake_minimum_required(VERSION 3.31.11 FATAL_ERROR)
project(dircGR VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#---- Conditional Environment CXX Standard Selection
# Check if the official container software stack directory exists on the system
if(EXISTS "/opt/software")
    message(STATUS ">>> Detected container environment via path signature.")
    message(STATUS ">>> Globally forcing C++20 standard configuration.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20" CACHE STRING "Forced standard" FORCE)
    set(CHOSEN_FEATURE "cxx_std_20")
    set(CPPCHECK_STD "c++20")
else()
    message(STATUS ">>> Detected you are outside container environment.")
    message(STATUS ">>> Globally forcing C++17 standard configuration.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" CACHE STRING "Forced standard" FORCE)
    set(CHOSEN_FEATURE "cxx_std_17")
    set(CPPCHECK_STD "c++17")    
endif()

#---- Force release mode
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Forced Release Build" FORCE)

#---- Setup ROOT
find_package(ROOT REQUIRED COMPONENTS Core Hist Gpad RIO Tree MathCore Physics)
# 1. Clean out any old/conflicting -std= flags from the ROOT flags variable
string(REGEX REPLACE "-std=[^ ]+" "" ROOT_CXX_FLAGS_CLEAN "${ROOT_CXX_FLAGS}")

#---- Define executable and header file tracking
set(HEADER_FILES
    dircGR.h
    cosangupper.h   
    cosanglower.h   
)
set(SOURCE_FILES
    dircGR.cpp
    main.cpp
)
add_executable(dircGR_app ${SOURCE_FILES} ${HEADER_FILES})

#---- Use SHELL to force CMake to split the space-separated flag string
target_compile_options(dircGR_app PRIVATE "SHELL:${ROOT_CXX_FLAGS_CLEAN}")

#---- This locks the compiler binary rule into the chosen standard BEFORE checking ROOT dependencies
target_compile_features(dircGR_app PRIVATE ${CHOSEN_FEATURE})

#---- Target-level properties configuration
set_target_properties(dircGR_app PROPERTIES
    LINK_FLAGS "${ROOT_EXE_LINKER_FLAGS}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)

#---- Link dependences 
target_link_libraries(dircGR_app PRIVATE ${ROOT_LIBRARIES})
# This forces ROOT headers to be parsed using the C++ standard property bound to the target
target_include_directories(dircGR_app SYSTEM PRIVATE ${ROOT_INCLUDE_DIRS})

#---- Post-target static analysis cppcheck
find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    add_custom_target(check 
        COMMAND ${CPPCHECK_PATH} --std=${CPPCHECK_STD} --enable=all --inconclusive --library=posix 
                -i build 
                --check-level=normal     # or exhaustive
                --suppress=normalCheckLevelMaxBranches
                --suppress=missingIncludeSystem --suppress=missingInclude 
                --suppress=cstyleCast --suppress=variableScope --suppress=funcArgNamesDifferent 
                --suppress=unreadVariable --suppress=constVariablePointer --suppress=constParameter 
                --suppress=virtualCallInConstructor 
                --checkers-report=cppcheck.txt 
                -isystem $ENV{ROOTSYS}/include ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cppcheck static analysis..."
    )
else()
    message(WARNING "cppcheck not found -> analysis skipped.")
endif()
