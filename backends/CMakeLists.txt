# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED UPMEM_VERSION)
    set(UPMEM_VERSION 0.0.0)
endif()

if (NOT DEFINED CLANG_FORMAT_EXE)
    set(CLANG_FORMAT_EXE clang-format)
endif()

project(backends VERSION ${UPMEM_VERSION})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )


include(GNUInstallDirs)

# Common compile flags for our libraries
set( STRICT_COMPILE_FLAGS "-Wall -Werror -Wextra -std=gnu11" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${STRICT_COMPILE_FLAGS}" )
set( CMAKE_C_VISIBILITY_PRESET hidden )

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags")
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags")

file(GLOB_RECURSE ALL_SOURCE_FILES *.c *.h)
list(SORT ALL_SOURCE_FILES)

file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format-pattern "")
set(FIRST_SOURCE_FILE 1)

foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    string(REGEX MATCHALL "${CMAKE_CURRENT_SOURCE_DIR}(/.*)?/(target|build|CMakeFiles|(cmake-build-.*)|casim)" EXCLUDED_FILE_FOUND ${SOURCE_FILE})
    if (NOT ${EXCLUDED_FILE_FOUND} EQUAL -1)
        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
    else()
        if (FIRST_SOURCE_FILE)
            set(FIRST_SOURCE_FILE 0)
        else()
            file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format-pattern "\n")
        endif()

        STRING(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}/" "" SRC_WITHOUT_PREFIX ${SOURCE_FILE})
        if (NOT ${SRC_WITHOUT_PREFIX} EQUAL -1)
            file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format-pattern "${SRC_WITHOUT_PREFIX}")
        else()
            file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format-pattern "${SOURCE_FILE}")
        endif ()
    endif ()
endforeach ()


add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE}
        -style=file
        -i
        ${ALL_SOURCE_FILES}
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} USES_TERMINAL)

macro(add_optional_subdirectory subdirectory)
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory})
        add_subdirectory(${subdirectory})
    endif()
endmacro()

add_subdirectory(api)
add_subdirectory(ufi)
add_subdirectory(hw)
add_subdirectory(verbose)
add_subdirectory(host-lldb-attach-dpu)

add_optional_subdirectory(fsim)
add_optional_subdirectory(casim)
add_optional_subdirectory(modelsim)
add_optional_subdirectory(backup_spi)
add_optional_subdirectory(spi_player)

if (DPU_INSTALL_SOURCES)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE
        DESTINATION src/backends
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/api
        DESTINATION src/backends
        PATTERN "test" EXCLUDE
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        DESTINATION src/backends
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/commons
        DESTINATION src/backends
        PATTERN "src/tcp" EXCLUDE
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/host-lldb-attach-dpu
        DESTINATION src/backends
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/hw
        DESTINATION src/backends
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ufi
        DESTINATION src/backends
        PATTERN "test" EXCLUDE
        )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/verbose
        DESTINATION src/backends
        PATTERN "test" EXCLUDE
        )
endif()
