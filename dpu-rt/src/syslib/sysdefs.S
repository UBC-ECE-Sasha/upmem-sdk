//
// Copyright 2020 UPMEM. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef __SYS_DEFINES_S
#define __SYS_DEFINES_S

//
// System Pointers
//
#define __HEAP_POINTER__ __sys_heap_pointer
#define __SP_TABLE__ __sys_thread_stack_table_ptr
#define __SEM_TABLE__ __sys_semaphore_table
#define __WQ_TABLE__ __sys_wq_table
#define __MUTEX_TABLE__ __sys_mutex_table
#define __VBUFFER_TABLE__ __sys_virtual_buffer_table
#define __PRINT_BUFFER__ __sys_temp_buffer_entry0

#define __NB_VBUFFER__ __sys_vbuffer_nb
#define __NB_SEMAPHORES__ __sys_sem_nb
#define __NB_MUTEX__ __sys_mutex_nb

//
// Special Registers
//
#define __RET__      r23
#define __SP__       r22
#define __VAL_RET__  r0
#define __VAL_DRET__ d0

// Kernel nano-stack: the builder reserves a number of longwords at the very beginnning of the WRAM
// to store and restore temporary values. The kernel nano-stack size is 4 longwords, it should be good
// not to use the very first entry, since it's the null pointer.
#define __SYS_SAVE_REGISTER0__(r)     sw          id4, __sys_thread_nanostack_entry_0, r
#define __SYS_RESTORE_REGISTER0__(r)  lw r,       id4, __sys_thread_nanostack_entry_0
#define __SYS_SAVE_REGISTER1__(r)     sw          id4, __sys_thread_nanostack_entry_1, r
#define __SYS_RESTORE_REGISTER1__(r)  lw r,       id4, __sys_thread_nanostack_entry_1
#define __SYS_SAVE_RET__()            sw          id4, __sys_thread_nanostack_entry_2, __RET__
#define __SYS_RESTORE_RET__()         lw __RET__, id4, __sys_thread_nanostack_entry_2

// A list of "fault codes"
#define __FAULT_ALLOC_HEAP_FULL__      fault 1
#define __FAULT_DIVISION_BY_ZERO__     fault 2
#define __FAULT_ASSERT_FAILED__        fault 3
#define __FAULT_HALT__                 fault 4

#define __WQ_NO_ENTRY 0xFF

// Use this macro at the beginning of an assembly function in order to get profiled.
#ifdef DPU_PROFILING
#define __ADD_PROFILING_ENTRY__        call __RET__, mcount
#else
#define __ADD_PROFILING_ENTRY__
#endif

#endif
