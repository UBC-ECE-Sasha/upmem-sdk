# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0063 NEW)

set( CMAKE_C_VISIBILITY_PRESET default )

set( SOURCE_DIRECTORY src/ )
set( COMMONS_INCLUDE_DIRECTORIES ../commons/include/ ../commons/src/properties ../commons/src/pcb ../verbose/src
        ../commons/src ../api/include ../api/src/include)

set( COMMONS_SOURCES
        ../commons/src/properties/dpu_properties.c
        ../commons/src/types/dpu_types.c
        )

set( UFI_SOURCES ${COMMONS_SOURCES}
        src/dpu_operation.h
        src/dpu_operation.c
        src/dpu_packet.h
        src/dpu_planner.c
        src/dpu_query.h
        src/dpu_query.c
        src/dpu_transaction.h
        src/dpu_transaction.c
        src/list.h
        src/dpu_commands.h
        src/dpu_operation_type.h
        src/dpu_operation_type_dump.c
        src/dpu_query_constructors.c
        src/dpu_query_constructors.h
        src/dpu_query_parameters.h
        src/dpu_query_parameters_dump.c
        src/dpu_query_result_update.c
        src/dpu_query_result_dump.c
        src/dpu_query_target_dump.c
        src/dpu_query_to_operations.c
        src/dpu_query_type.h
        src/dpu_query_type_dump.c
        src/dpu_operation_to_command.c
        )

add_library( dpu-ufi SHARED ${UFI_SOURCES} )
target_include_directories( dpu-ufi PUBLIC ${COMMONS_INCLUDE_DIRECTORIES} ${SOURCE_DIRECTORY} )
target_link_libraries( dpu-ufi dpuverbose )
set_target_properties(dpu-ufi PROPERTIES VERSION ${UPMEM_VERSION})

install(
    TARGETS dpu-ufi
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

## TEST SUITE ##################################################################

if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    function(deftest testName)
        add_executable(${testName} test/${testName}.c)
        target_include_directories(${testName} PUBLIC src ${COMMONS_INCLUDE_DIRECTORIES} ${SOURCE_DIRECTORY} )
        target_link_libraries(${testName} dpu-ufi dpuverbose)
        add_test(NAME ${testName} COMMAND ${testName})
    endfunction(deftest)

    deftest(basic_test)
    deftest(dpu_command_template_test)
endif()
