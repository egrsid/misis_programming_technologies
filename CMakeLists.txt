cmake_minimum_required(VERSION 3.20)

project(sidorov-e-v-25-03)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin.relwithdbg)

include_directories(prj.labs prj.thirdparty)

add_subdirectory(prj.labs)
add_subdirectory(prj.test)

find_package(Doxygen)  # найдет doxygen, если он установлен
option(BUILD_DOCUMENTATION "Create HTML based documentation (requires Doxygen)" ${DOXYGEN_FOUND})