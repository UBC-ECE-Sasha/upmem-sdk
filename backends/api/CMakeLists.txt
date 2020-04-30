# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0048 NEW)

find_package(JNI REQUIRED)
find_package(Threads REQUIRED)
find_package(LibElf REQUIRED)

set( JAVA_HEADERS "${JAVA_INCLUDE_PATH}" "${JAVA_INCLUDE_PATH2}" )

set(VC_INCLUDE_DIRECTORIES ../verbose/src)
set(INCLUDE_DIRECTORIES include/ ${VC_INCLUDE_DIRECTORIES} ${LIBELF_INCLUDE_DIR})

set(PROPERTIES_SOURCES
        ../commons/src/properties/dpu_properties.c
        ../commons/src/properties/dpu_properties_loader.c
        )

set(PCB_SOURCES
        ../commons/src/pcb/dpu_pcb_transformation.c
        )

set(DESCRIPTION_SOURCES
        ../commons/src/types/dpu_description.c
        )

set(COMMONS_SOURCES
        ${PROPERTIES_SOURCES}
        ${PCB_SOURCES}
        ${DESCRIPTION_SOURCES}
        ../commons/src/dpu_predef_programs.c
        ../commons/src/dpu_package.c
        )

set(SOURCES
        src/api/dpu.c
        src/dpu_custom.c
        src/dpu_log.c
        src/dpu_elf.c
        src/dpu_error.c
        src/dpu_config.c
        src/dpu_debug.c
        src/dpu_internals.c
        src/dpu_loader.c
        src/dpu_profiler.c
        src/dpu_program.c
        src/dpu_management.c
        src/dpu_memory.c
        src/dpu_rank_handler_allocator.c
        src/dpu_runner.c

        src/runtime_info.c
        src/ufi_utils.c
        )

set(ALL_SOURCES ${SOURCES} ${COMMONS_SOURCES})

set(JNI_SOURCES
        src/api/java_api.c
        )

add_library( dpu SHARED ${ALL_SOURCES} )
target_include_directories( dpu PUBLIC ${INCLUDE_DIRECTORIES} )
target_link_libraries( dpu m ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${LIBELF_LIBRARIES} dpu-ufi dpuverbose )
target_compile_definitions(dpu PUBLIC DPU_TOOLS_VERSION=${UPMEM_VERSION})
set_target_properties(dpu PROPERTIES VERSION ${UPMEM_VERSION})

add_library(dpujni SHARED ${JNI_SOURCES})
target_include_directories(dpujni PUBLIC ${INCLUDE_DIRECTORIES} ${JAVA_HEADERS})
target_link_libraries(dpujni dpu dpuverbose )
set_target_properties(dpujni PROPERTIES VERSION ${UPMEM_VERSION})

install(
    TARGETS dpu dpujni
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dpu
    )

## TEST SUITE ##################################################################

if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    #todo can we automatically find what we have on the machine?
    add_definitions(-D__HAVE_FSIM__)
    #add_definitions(-D__HAVE_CAS__)
    #add_definitions(-D__HAVE_HW__)
    set(TEST_SOURCES test/commons.h test/runtests.h test/runtests.c )
    function(deftest testName)
        add_executable(${testName} test/${testName}.c ${TEST_SOURCES})
        target_include_directories(${testName} PUBLIC src ${INCLUDE_DIRECTORIES})
        target_link_libraries(${testName} dpu)
        add_test(NAME ${testName} COMMAND ${testName})
    endfunction(deftest)

    deftest(GetDeleteTest)
    deftest(ReadWriteWramTest)
    deftest(ReadWriteIramTest)
    deftest(ReadWriteMramTest)
    deftest(BitOrderingTest)
endif()
