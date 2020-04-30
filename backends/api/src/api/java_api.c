/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <jni.h>
#include <stdlib.h>

#include <runtime_info.h>
#include <dpu_config.h>
#include <dpu_custom.h>
#include <dpu_debug.h>
#include <dpu_description.h>
#include <dpu_loader.h>
#include <dpu_management.h>
#include <dpu_profiler.h>
#include <dpu_runner.h>
#include <dpu_transfer_mram.h>
#include <dpu_memory.h>

#define __UNUSED_PARAM__ __attribute__((unused))

#define __DPU_JNI_FUNCTION__ __attribute__((used))

static inline struct dpu_rank_t *
_this_rank(jlong lp)
{
    return (struct dpu_rank_t *)((void *)lp);
}

static inline void
flush_outputs(void)
{
    fflush(stdout);
    fflush(stderr);
}

static inline jobject
buildDpuDescription(JNIEnv *env, dpu_description_t description)
{
    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuDescription");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(IIIIIIIIIII)V");

    jint chip_id = description->signature.chip_id;
    jint nr_of_cis = description->topology.nr_of_control_interfaces;
    jint nr_of_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;
    jint mram_size = description->memories.mram_size;
    jint wram_size = description->memories.wram_size;
    jint iram_size = description->memories.iram_size;
    jint dbg_mram_size = description->memories.dbg_mram_size;
    jint nr_of_threads = description->dpu.nr_of_threads;
    jint nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;
    jint nr_of_notify_bits = description->dpu.nr_of_notify_bits;
    jint nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;

    return (*env)->NewObject(env,
        cls,
        constructor,
        chip_id,
        nr_of_cis,
        nr_of_dpus_per_ci,
        mram_size,
        wram_size,
        iram_size,
        dbg_mram_size,
        nr_of_threads,
        nr_of_atomic_bits,
        nr_of_notify_bits,
        nr_of_work_registers_per_thread);
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_getBackendDescription(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring profile)
{

    const char *c_friendly_profile = (*env)->GetStringUTFChars(env, profile, 0);
    struct _dpu_description_t *description;

    if (dpu_get_profile_description(c_friendly_profile, &description) != DPU_OK) {
        (*env)->ReleaseStringUTFChars(env, profile, c_friendly_profile);
        flush_outputs();
        return NULL;
    }
    (*env)->ReleaseStringUTFChars(env, profile, c_friendly_profile);

    flush_outputs();

    jobject dpuDescription = buildDpuDescription(env, description);

    dpu_free_description(description);

    return dpuDescription;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_getTargetDescription(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp)
{
    return buildDpuDescription(env, dpu_get_description(_this_rank(lp)));
}

/* is_attaching: when a process is attaching a DPU (generally for debug purposes), there are 2 important things to do:
 *              - Do not reset the rank
 *              - Find the right color !
 */
JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_reserveDpuRank(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jstring profile,
    jboolean is_attaching,
    __UNUSED_PARAM__ jint attaching_to_slice_id,
    __UNUSED_PARAM__ jint attaching_to_dpu_id,
    __UNUSED_PARAM__ jint host_pid)
{
    const char *c_friendly_profile = (*env)->GetStringUTFChars(env, profile, 0);

    struct dpu_rank_t *rank;
    long lp;

    if (dpu_get_rank_of_type(c_friendly_profile, &rank) != DPU_OK) {
        lp = 0;
    } else {
        lp = (long)rank;
        if (!is_attaching) {
            if (dpu_reset_rank(rank) != DPU_OK) {
                (void)dpu_free_rank(rank);
                lp = 0;
            }
        }
    }

    (*env)->ReleaseStringUTFChars(env, profile, c_friendly_profile);

    flush_outputs();
    return lp;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_releaseDpuRank(__UNUSED_PARAM__ JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp)
{
    (void)dpu_free_rank(_this_rank(lp));
    flush_outputs();
}

JNIEXPORT jboolean JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_isDpuEnabled(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id)
{
    return dpu_is_enabled(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id));
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_resetDpu(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id)
{
    dpu_soft_reset_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id));

    flush_outputs();
    return lp;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_stopDpu(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t context = malloc(sizeof(*context));
    uint32_t nr_of_atomic_bits;
    uint8_t nr_of_dpu_threads, nr_of_work_registers_per_thread;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;
    nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;
    nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;

    context->registers = malloc(nr_of_dpu_threads * nr_of_work_registers_per_thread * sizeof(*(context->registers)));
    context->scheduling = malloc(nr_of_dpu_threads * sizeof(*(context->scheduling)));
    context->pcs = malloc(nr_of_dpu_threads * sizeof(*(context->pcs)));
    context->zero_flags = malloc(nr_of_dpu_threads * sizeof(*(context->zero_flags)));
    context->carry_flags = malloc(nr_of_dpu_threads * sizeof(*(context->carry_flags)));
    context->atomic_register = malloc(nr_of_atomic_bits * sizeof(*(context->atomic_register)));

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_dpu_threads; ++each_thread) {
        context->scheduling[each_thread] = 0xFF;
    }

    context->nr_of_running_threads = 0;
    context->bkp_fault = false;
    context->dma_fault = false;
    context->mem_fault = false;

    dpu_stop_threads_for_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, dpu_id), context);

    jlong contextPtr = (jlong)context;
    jboolean bkpFault = (jboolean)context->bkp_fault;
    jboolean dmaFault = (jboolean)context->dma_fault;
    jboolean memFault = (jboolean)context->mem_fault;
    jint bkpIndex = (jint)context->bkp_fault_thread_index;
    jint dmaIndex = (jint)context->dma_fault_thread_index;
    jint memIndex = (jint)context->mem_fault_thread_index;
    jint bkpFaultId = (jint)context->bkp_fault_id;
    jbyteArray scheduling = (*env)->NewByteArray(env, nr_of_dpu_threads);
    jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);

    (*env)->SetByteArrayRegion(env, scheduling, 0, nr_of_dpu_threads, (const jbyte *)context->scheduling);
    (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)context->pcs);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuFaultResult");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(IIJZZZIIII[B[S)V");

    information = (*env)->NewObject(env,
        cls,
        constructor,
        dpu_id,
        ci_id,
        contextPtr,
        bkpFault,
        dmaFault,
        memFault,
        bkpIndex,
        dmaIndex,
        memIndex,
        bkpFaultId,
        scheduling,
        pcs);

    flush_outputs();
    return information;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_stopRank(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint dpu_id,
    jint host_pid)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t context;
    uint32_t nr_of_atomic_bits;
    uint8_t nr_of_dpu_threads, nr_of_work_registers_per_thread;
    uint32_t nr_dpus_per_ci, nr_cis;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;
    nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;
    nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;
    nr_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;
    nr_cis = description->topology.nr_of_control_interfaces;

    context = malloc(sizeof(*context) * nr_cis * nr_dpus_per_ci);

    for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            uint32_t idx_dpu_context = each_dpu * description->topology.nr_of_control_interfaces + each_slice;

            context[idx_dpu_context].registers
                = malloc(nr_of_dpu_threads * nr_of_work_registers_per_thread * sizeof(*(context[idx_dpu_context].registers)));
            context[idx_dpu_context].scheduling = malloc(nr_of_dpu_threads * sizeof(*(context[idx_dpu_context].scheduling)));
            context[idx_dpu_context].pcs = malloc(nr_of_dpu_threads * sizeof(*(context[idx_dpu_context].pcs)));
            context[idx_dpu_context].zero_flags = malloc(nr_of_dpu_threads * sizeof(*(context[idx_dpu_context].zero_flags)));
            context[idx_dpu_context].carry_flags = malloc(nr_of_dpu_threads * sizeof(*(context[idx_dpu_context].carry_flags)));
            context[idx_dpu_context].atomic_register
                = malloc(nr_of_atomic_bits * sizeof(*(context[idx_dpu_context].atomic_register)));

            for (dpu_thread_t each_thread = 0; each_thread < nr_of_dpu_threads; ++each_thread) {
                context[idx_dpu_context].scheduling[each_thread] = 0xFF;
            }

            context[idx_dpu_context].nr_of_running_threads = 0;
            context[idx_dpu_context].bkp_fault = false;
            context[idx_dpu_context].dma_fault = false;
            context[idx_dpu_context].mem_fault = false;
        }

        dpu_save_slice_context_and_extract_debug_info_from_pid_for_dpu(dpu_get(_this_rank(lp), each_slice, dpu_id), host_pid);
    }

    dpu_stop_dpus_for_rank(_this_rank(lp), context);

    jclass arrayListClass = (*env)->FindClass(env, "java/util/ArrayList");
    jmethodID constructorList = (*env)->GetMethodID(env, arrayListClass, "<init>", "()V");
    jmethodID addList = (*env)->GetMethodID(env, arrayListClass, "add", "(Ljava/lang/Object;)Z");

    jobject array_information = (*env)->NewObject(env, arrayListClass, constructorList);

    for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
        for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
            uint32_t idx_dpu_context = each_dpu * description->topology.nr_of_control_interfaces + each_slice;

            jlong contextPtr = (jlong)&context[idx_dpu_context];
            jboolean bkpFault = (jboolean)context[idx_dpu_context].bkp_fault;
            jboolean dmaFault = (jboolean)context[idx_dpu_context].dma_fault;
            jboolean memFault = (jboolean)context[idx_dpu_context].mem_fault;
            jint bkpIndex = (jint)context[idx_dpu_context].bkp_fault_thread_index;
            jint dmaIndex = (jint)context[idx_dpu_context].dma_fault_thread_index;
            jint memIndex = (jint)context[idx_dpu_context].mem_fault_thread_index;
            jint bkpFaultId = (jint)context->bkp_fault_id;
            jbyteArray scheduling = (*env)->NewByteArray(env, nr_of_dpu_threads);
            jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);

            (*env)->SetByteArrayRegion(env, scheduling, 0, nr_of_dpu_threads, (const jbyte *)context[idx_dpu_context].scheduling);
            (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)context[idx_dpu_context].pcs);

            jobject information;

            jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuFaultResult");

            jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(IIJZZZIIII[B[S)V");

            information = (*env)->NewObject(env,
                cls,
                constructor,
                each_dpu,
                each_slice,
                contextPtr,
                bkpFault,
                dmaFault,
                memFault,
                bkpIndex,
                dmaIndex,
                memIndex,
                bkpFaultId,
                scheduling,
                pcs);

            (*env)->CallObjectMethod(env, array_information, addList, information);
        }
    }

    flush_outputs();
    return array_information;
}
#pragma GCC diagnostic pop

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_resumeDpu(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jlong dump_pointer)
{
    dpu_context_t context = (dpu_context_t)dump_pointer;
    struct dpu_t *dpu = dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, dpu_id);

    dpu_resume_threads_for_dpu(dpu, context);
    dpu_restore_slice_context_for_dpu(dpu);

    free(context->registers);
    free(context->scheduling);
    free(context->pcs);
    free(context->zero_flags);
    free(context->carry_flags);
    free(context->atomic_register);

    flush_outputs();
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_resumeRank(__UNUSED_PARAM__ JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jlong dump_pointer)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t context = (dpu_context_t)dump_pointer;
    uint32_t nr_dpus_per_ci, nr_cis;
    struct _dpu_description_t *description = dpu_get_description(rank);

    dpu_resume_dpus_for_rank(rank, context);
    nr_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;
    nr_cis = description->topology.nr_of_control_interfaces;

    for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
        for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
            uint32_t idx_dpu_context = each_dpu * description->topology.nr_of_control_interfaces + each_slice;

            free(context[idx_dpu_context].registers);
            free(context[idx_dpu_context].pcs);
            free(context[idx_dpu_context].zero_flags);
            free(context[idx_dpu_context].carry_flags);
            free(context[idx_dpu_context].atomic_register);
        }
    }

    flush_outputs();
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_launchThread(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint thread,
    jboolean should_resume)
{
    bool ignored;

    dpu_launch_thread_on_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        (dpu_thread_t)thread,
        (bool)should_resume,
        &ignored);

    flush_outputs();
    return lp;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_launchThreadOnRank(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint thread,
    jboolean should_resume)
{
    dpu_bitfield_t thread_was_running[DPU_MAX_NR_CIS];

    dpu_launch_thread_on_rank(_this_rank(lp), (dpu_thread_t)thread, (bool)should_resume, thread_was_running);

    flush_outputs();
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_pollDpu(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jint ci_id, jint dpu_id)
{
    bool has_internal_error, is_running, is_in_fault;

    has_internal_error
        = dpu_poll_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), &is_running, &is_in_fault)
        != DPU_OK;

    jobject result;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuPollResult");
    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(JZZZ)V");

    result = (*env)->NewObject(env, cls, constructor, lp, is_running, is_in_fault, has_internal_error);

    flush_outputs();
    return result;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_pollRank(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jintArray running, jintArray fault)
{
    jint *native_running = (*env)->GetIntArrayElements(env, running, 0);
    jint *native_fault = (*env)->GetIntArrayElements(env, fault, 0);

    struct dpu_rank_t *rank = _this_rank(lp);

    dpu_poll_rank(rank, (dpu_bitfield_t *)native_running, (dpu_bitfield_t *)native_fault);

    uint8_t nr_control_interfaces = dpu_get_description(rank)->topology.nr_of_control_interfaces;
    (*env)->SetIntArrayRegion(env, running, 0, nr_control_interfaces, native_running);
    (*env)->SetIntArrayRegion(env, fault, 0, nr_control_interfaces, native_fault);

    (*env)->ReleaseIntArrayElements(env, running, native_running, 0);
    (*env)->ReleaseIntArrayElements(env, fault, native_fault, 0);
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyToWram(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint toWordAtIndex,
    jintArray source,
    jint nbOfWords)
{
    jint *fill = (*env)->GetIntArrayElements(env, source, 0);

    dpu_copy_to_wram_for_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        (wram_addr_t)toWordAtIndex,
        (dpuword_t *)fill,
        (wram_size_t)nbOfWords);

    (*env)->ReleaseIntArrayElements(env, source, fill, 0);

    flush_outputs();
    return lp;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyFromWram(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint fromWordAtIndex,
    jint nbOfWords,
    jintArray destination)
{
    jint *fill = (*env)->GetIntArrayElements(env, destination, 0);

    dpu_copy_from_wram_for_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        (dpuword_t *)fill,
        (wram_addr_t)fromWordAtIndex,
        (wram_size_t)nbOfWords);

    (*env)->SetIntArrayRegion(env, destination, 0, nbOfWords, fill);

    (*env)->ReleaseIntArrayElements(env, destination, fill, 0);

    flush_outputs();
    return lp;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyToIram(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint toInstructionAtIndex,
    jlongArray source,
    jint nbOfInstructions)
{
    jlong *fill = (*env)->GetLongArrayElements(env, source, 0);

    dpu_copy_to_iram_for_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        (iram_addr_t)toInstructionAtIndex,
        (dpuinstruction_t *)fill,
        (iram_size_t)nbOfInstructions);

    (*env)->ReleaseLongArrayElements(env, source, fill, 0);

    flush_outputs();
    return lp;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyFromIram(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint fromInstructionAtIndex,
    jint nbOfInstructions,
    jlongArray destination)
{
    jlong *fill = (*env)->GetLongArrayElements(env, destination, 0);

    dpu_copy_from_iram_for_dpu(dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        (dpuinstruction_t *)fill,
        (iram_addr_t)fromInstructionAtIndex,
        (iram_size_t)nbOfInstructions);

    (*env)->SetLongArrayRegion(env, destination, 0, nbOfInstructions, fill);

    (*env)->ReleaseLongArrayElements(env, destination, fill, 0);

    flush_outputs();
    return lp;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyToMramNumber(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint toByteAtIndex,
    jbyteArray source,
    jint nbOfBytes,
    jint mramNumber)
{
    jbyte *fill = (*env)->GetByteArrayElements(env, source, 0);

    struct dpu_rank_t *rank = _this_rank(lp);
    struct dpu_transfer_mram *transfer_matrix;

    dpu_transfer_matrix_allocate(rank, &transfer_matrix);
    dpu_transfer_matrix_add_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        transfer_matrix,
        (uint8_t *)fill,
        (mram_size_t)nbOfBytes,
        (mram_addr_t)toByteAtIndex,
        (uint8_t)mramNumber);

    dpu_copy_to_mrams(rank, transfer_matrix);

    dpu_transfer_matrix_free(rank, transfer_matrix);

    (*env)->ReleaseByteArrayElements(env, source, fill, 0);

    flush_outputs();
    return lp;
}

static void
mram_access(JNIEnv *env, jlong lp, jobject transfer, dpu_error_t (*transfer_fn)(struct dpu_rank_t *, struct dpu_transfer_mram *))
{
    struct dpu_rank_t *rank = _this_rank(lp);
    struct dpu_transfer_mram *transfer_matrix;

    dpu_transfer_matrix_allocate(rank, &transfer_matrix);

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuMramTransfer");
    jmethodID isTransferEnabled = (*env)->GetMethodID(env, cls, "transferIsEnabledForDpu", ("(II)Z"));
    jmethodID getOffset = (*env)->GetMethodID(env, cls, "getTransferOffsetForDpu", ("(II)I"));
    jmethodID getStorage = (*env)->GetMethodID(env, cls, "getTransferStorageForDpu", ("(II)[B"));
    jmethodID getLength = (*env)->GetMethodID(env, cls, "getTransferLengthForDpu", ("(II)I"));

    dpu_description_t description = dpu_get_description(rank);

    uint8_t nr_cis = description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;

    jbyteArray *storages = calloc(nr_cis * nr_dpus_per_ci, sizeof(*storages));
    jbyte **buffers = calloc(nr_cis * nr_dpus_per_ci, sizeof(*buffers));

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            jboolean transfer_enabled = (*env)->CallBooleanMethod(env, transfer, isTransferEnabled, each_ci, each_dpu);

            if (transfer_enabled) {
                jint offset = (*env)->CallIntMethod(env, transfer, getOffset, each_ci, each_dpu);
                jbyteArray storage = (*env)->CallObjectMethod(env, transfer, getStorage, each_ci, each_dpu);
                jint length = (*env)->CallIntMethod(env, transfer, getLength, each_ci, each_dpu);

                jbyte *buffer = (*env)->GetPrimitiveArrayCritical(env, storage, NULL);

                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);
                dpu_transfer_matrix_add_dpu(dpu, transfer_matrix, buffer, length, offset, DPU_PRIMARY_MRAM);

                storages[each_ci * nr_dpus_per_ci + each_dpu] = storage;
                buffers[each_ci * nr_dpus_per_ci + each_dpu] = buffer;
            }
        }
    }

    transfer_fn(rank, transfer_matrix);

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            jbyteArray storage = storages[each_ci * nr_dpus_per_ci + each_dpu];
            jbyte *buffer = buffers[each_ci * nr_dpus_per_ci + each_dpu];

            if (buffer != NULL) {
                (*env)->ReleasePrimitiveArrayCritical(env, storage, buffer, 0);
            }
        }
    }

    dpu_transfer_matrix_free(rank, transfer_matrix);
    flush_outputs();
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyToMrams(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jobject transfer)
{
    mram_access(env, lp, transfer, (dpu_error_t(*)(struct dpu_rank_t *, struct dpu_transfer_mram *))dpu_copy_to_mrams);
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyFromMrams(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jobject transfer)
{
    mram_access(env, lp, transfer, dpu_copy_from_mrams);
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_copyFromMramNumber(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint fromByteAtIndex,
    jint nbOfBytes,
    jbyteArray destination,
    jint mramNumber)
{
    jbyte *fill = (*env)->GetByteArrayElements(env, destination, 0);

    struct dpu_rank_t *rank = _this_rank(lp);
    struct dpu_transfer_mram *transfer_matrix;

    dpu_transfer_matrix_allocate(rank, &transfer_matrix);
    dpu_transfer_matrix_add_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        transfer_matrix,
        (uint8_t *)fill,
        (mram_size_t)nbOfBytes,
        (mram_addr_t)fromByteAtIndex,
        (uint8_t)mramNumber);

    dpu_copy_from_mrams(rank, transfer_matrix);

    dpu_transfer_matrix_free(rank, transfer_matrix);

    (*env)->SetByteArrayRegion(env, destination, 0, nbOfBytes, fill);

    (*env)->ReleaseByteArrayElements(env, destination, fill, 0);

    flush_outputs();
    return lp;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_triggerFaultOnRank(__UNUSED_PARAM__ JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp)
{
    (void)dpu_trigger_fault_on_rank(_this_rank(lp));
    flush_outputs();
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_clearFaultOnRank(__UNUSED_PARAM__ JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp)
{
    (void)dpu_clear_fault_on_rank(_this_rank(lp));
    flush_outputs();
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_dumpDpuContext(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jint ci_id, jint dpu_id)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_slice_id_t slice_id = (dpu_slice_id_t)ci_id;
    dpu_member_id_t dpu = (dpu_member_id_t)dpu_id;
    struct dpu_t *the_dpu = dpu_get(rank, slice_id, dpu);
    struct _dpu_context_t *dpu_context;
    uint32_t nr_of_atomic_bits;
    uint8_t nr_of_dpu_threads, nr_of_work_registers_per_thread;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;
    nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;
    nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;

    dpu_context = dpu_alloc_dpu_context(rank);

    dpu_extract_pcs_for_dpu(the_dpu, dpu_context);
    dpu_extract_context_for_dpu(the_dpu, dpu_context);

    jbooleanArray atomicBits = (*env)->NewBooleanArray(env, nr_of_atomic_bits);
    jintArray registers = (*env)->NewIntArray(env, nr_of_work_registers_per_thread * nr_of_dpu_threads);
    jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);
    jbooleanArray zeroFlags = (*env)->NewBooleanArray(env, nr_of_dpu_threads);
    jbooleanArray carryFlags = (*env)->NewBooleanArray(env, nr_of_dpu_threads);

    (*env)->SetBooleanArrayRegion(env, atomicBits, 0, nr_of_atomic_bits, (const jboolean *)dpu_context->atomic_register);
    (*env)->SetIntArrayRegion(
        env, registers, 0, nr_of_work_registers_per_thread * nr_of_dpu_threads, (const jint *)dpu_context->registers);
    (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)dpu_context->pcs);
    (*env)->SetBooleanArrayRegion(env, zeroFlags, 0, nr_of_dpu_threads, (const jboolean *)dpu_context->zero_flags);
    (*env)->SetBooleanArrayRegion(env, carryFlags, 0, nr_of_dpu_threads, (const jboolean *)dpu_context->carry_flags);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuContextDump");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "([Z[I[S[Z[Z)V");

    information = (*env)->NewObject(env, cls, constructor, atomicBits, registers, pcs, zeroFlags, carryFlags);

    dpu_free_dpu_context(dpu_context);

    flush_outputs();
    return information;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_initializeFaultProcess(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jint ci_id, jint dpu_id)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t context = malloc(sizeof(*context));
    uint32_t nr_of_atomic_bits;
    uint8_t nr_of_dpu_threads, nr_of_work_registers_per_thread;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;
    nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;
    nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;

    context->registers = malloc(nr_of_dpu_threads * nr_of_work_registers_per_thread * sizeof(*(context->registers)));
    context->scheduling = malloc(nr_of_dpu_threads * sizeof(*(context->scheduling)));
    context->pcs = malloc(nr_of_dpu_threads * sizeof(*(context->pcs)));
    context->zero_flags = malloc(nr_of_dpu_threads * sizeof(*(context->zero_flags)));
    context->carry_flags = malloc(nr_of_dpu_threads * sizeof(*(context->carry_flags)));
    context->atomic_register = malloc(nr_of_atomic_bits * sizeof(*(context->atomic_register)));

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_dpu_threads; ++each_thread) {
        context->scheduling[each_thread] = 0xFF;
    }

    context->nr_of_running_threads = 0;
    context->bkp_fault = false;
    context->dma_fault = false;
    context->mem_fault = false;

    dpu_initialize_fault_process_for_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), context);

    jlong contextPtr = (jlong)context;
    jboolean bkpFault = (jboolean)context->bkp_fault;
    jboolean dmaFault = (jboolean)context->dma_fault;
    jboolean memFault = (jboolean)context->mem_fault;
    jint bkpIndex = (jint)context->bkp_fault_thread_index;
    jint dmaIndex = (jint)context->dma_fault_thread_index;
    jint memIndex = (jint)context->mem_fault_thread_index;
    jint bkpFaultId = (jint)context->bkp_fault_id;
    jbyteArray scheduling = (*env)->NewByteArray(env, nr_of_dpu_threads);
    jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);

    (*env)->SetByteArrayRegion(env, scheduling, 0, nr_of_dpu_threads, (const jbyte *)context->scheduling);
    (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)context->pcs);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuFaultResult");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(IIJZZZIIII[B[S)V");

    information = (*env)->NewObject(env,
        cls,
        constructor,
        dpu_id,
        ci_id,
        contextPtr,
        bkpFault,
        dmaFault,
        memFault,
        bkpIndex,
        dmaIndex,
        memIndex,
        bkpFaultId,
        scheduling,
        pcs);

    flush_outputs();
    return information;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_dumpFaultRemainingDpuContext(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jlong dump_pointer)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t dpu_context = (dpu_context_t)dump_pointer;
    uint32_t nr_of_atomic_bits;
    uint8_t nr_of_dpu_threads, nr_of_work_registers_per_thread;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;
    nr_of_work_registers_per_thread = description->dpu.nr_of_work_registers_per_thread;
    nr_of_atomic_bits = description->dpu.nr_of_atomic_bits;

    dpu_extract_context_for_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), dpu_context);

    jbooleanArray atomicBits = (*env)->NewBooleanArray(env, nr_of_atomic_bits);
    jintArray registers = (*env)->NewIntArray(env, nr_of_work_registers_per_thread * nr_of_dpu_threads);
    jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);
    jbooleanArray zeroFlags = (*env)->NewBooleanArray(env, nr_of_dpu_threads);
    jbooleanArray carryFlags = (*env)->NewBooleanArray(env, nr_of_dpu_threads);

    (*env)->SetBooleanArrayRegion(env, atomicBits, 0, nr_of_atomic_bits, (const jboolean *)dpu_context->atomic_register);
    (*env)->SetIntArrayRegion(
        env, registers, 0, nr_of_work_registers_per_thread * nr_of_dpu_threads, (const jint *)dpu_context->registers);
    (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)dpu_context->pcs);
    (*env)->SetBooleanArrayRegion(env, zeroFlags, 0, nr_of_dpu_threads, (const jboolean *)dpu_context->zero_flags);
    (*env)->SetBooleanArrayRegion(env, carryFlags, 0, nr_of_dpu_threads, (const jboolean *)dpu_context->carry_flags);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuContextDump");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "([Z[I[S[Z[Z)V");

    information = (*env)->NewObject(env, cls, constructor, atomicBits, registers, pcs, zeroFlags, carryFlags);

    flush_outputs();
    return information;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_doDebugStep(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jlong dump_pointer,
    jint thread)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    dpu_context_t context = (dpu_context_t)dump_pointer;
    uint8_t nr_of_dpu_threads;
    struct _dpu_description_t *description = dpu_get_description(rank);

    nr_of_dpu_threads = description->dpu.nr_of_threads;

    context->bkp_fault = false;
    context->dma_fault = false;
    context->mem_fault = false;

    dpu_execute_thread_step_in_fault_for_dpu(
        dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), (dpu_thread_t)thread, context);

    jlong contextPtr = (jlong)context;
    jboolean bkpFault = (jboolean)context->bkp_fault;
    jboolean dmaFault = (jboolean)context->dma_fault;
    jboolean memFault = (jboolean)context->mem_fault;
    jint bkpIndex = (jint)context->bkp_fault_thread_index;
    jint dmaIndex = (jint)context->dma_fault_thread_index;
    jint memIndex = (jint)context->mem_fault_thread_index;
    jint bkpFaultId = (jint)context->bkp_fault_id;
    jbyteArray scheduling = (*env)->NewByteArray(env, nr_of_dpu_threads);
    jshortArray pcs = (*env)->NewShortArray(env, nr_of_dpu_threads);

    (*env)->SetByteArrayRegion(env, scheduling, 0, nr_of_dpu_threads, (const jbyte *)context->scheduling);
    (*env)->SetShortArrayRegion(env, pcs, 0, nr_of_dpu_threads, (const jshort *)context->pcs);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuFaultResult");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(IIJZZZIIII[B[S)V");

    information = (*env)->NewObject(env,
        cls,
        constructor,
        dpu_id,
        ci_id,
        contextPtr,
        bkpFault,
        dmaFault,
        memFault,
        bkpIndex,
        dmaIndex,
        memIndex,
        bkpFaultId,
        scheduling,
        pcs);

    flush_outputs();
    return information;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_finalizeFaultProcess(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jlong dump_pointer,
    jboolean just_clean)
{
    dpu_context_t context = (dpu_context_t)dump_pointer;
    struct dpu_rank_t *rank = _this_rank(lp);

    if (!just_clean) {
        dpu_finalize_fault_process_for_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), context);
    }

    free(context->registers);
    free(context->pcs);
    free(context->zero_flags);
    free(context->carry_flags);
    free(context->scheduling);
    free(context->atomic_register);
    free(context);

    flush_outputs();
}

JNIEXPORT jboolean JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_readAndUpdateNotifyBit(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jint notify_bit_index,
    jboolean value)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    bool was_set;

    dpu_get_and_update_notify_status_on_dpu(
        dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), (dpu_notify_bit_id_t)notify_bit_index, value, &was_set);

    flush_outputs();

    return (jboolean)was_set;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_systemReport(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jstring output)
{
    const char *c_friendly_output = (*env)->GetStringUTFChars(env, output, 0);
    struct dpu_rank_t *rank = _this_rank(lp);

    dpu_custom_for_dpu(dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id),
        DPU_COMMAND_SYSTEM_REPORT,
        (dpu_custom_command_args_t)c_friendly_output);

    (*env)->ReleaseStringUTFChars(env, output, c_friendly_output);
    flush_outputs();
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_preExecution(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id)
{
    dpu_custom_for_dpu(
        dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), DPU_COMMAND_DPU_PREEXECUTION, NULL);
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_postExecution(__UNUSED_PARAM__ JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id)
{
    dpu_custom_for_dpu(
        dpu_get(_this_rank(lp), (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id), DPU_COMMAND_DPU_POSTEXECUTION, NULL);
}

JNIEXPORT jstring JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_utilFormatString(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring format, jstring string)
{
    const char *c_format = (*env)->GetStringUTFChars(env, format, 0);
    const char *c_string = (*env)->GetStringUTFChars(env, string, 0);

    size_t needed = snprintf(NULL, 0, c_format, c_string) + 1;
    char *buffer = malloc(needed);
    snprintf(buffer, needed, c_format, c_string);

    jstring formated = (*env)->NewStringUTF(env, buffer);
    free(buffer);

    (*env)->ReleaseStringUTFChars(env, format, c_format);
    (*env)->ReleaseStringUTFChars(env, string, c_string);

    return formated;
}

JNIEXPORT jstring JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_utilFormatInt(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring format, jint value)
{
    const char *c_format = (*env)->GetStringUTFChars(env, format, 0);

    size_t needed = snprintf(NULL, 0, c_format, value) + 1;
    char *buffer = malloc(needed);
    snprintf(buffer, needed, c_format, value);

    jstring formated = (*env)->NewStringUTF(env, buffer);
    free(buffer);

    (*env)->ReleaseStringUTFChars(env, format, c_format);

    return formated;
}

JNIEXPORT jstring JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_utilFormatLong(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring format, jlong value)
{
    const char *c_format = (*env)->GetStringUTFChars(env, format, 0);

    size_t needed = snprintf(NULL, 0, c_format, value) + 1;
    char *buffer = malloc(needed);
    snprintf(buffer, needed, c_format, value);

    jstring formated = (*env)->NewStringUTF(env, buffer);
    free(buffer);

    (*env)->ReleaseStringUTFChars(env, format, c_format);

    return formated;
}

JNIEXPORT jstring JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_utilFormatDouble(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring format, jdouble value)
{
    const char *c_format = (*env)->GetStringUTFChars(env, format, 0);

    size_t needed = snprintf(NULL, 0, c_format, value) + 1;
    char *buffer = malloc(needed);
    snprintf(buffer, needed, c_format, value);

    jstring formated = (*env)->NewStringUTF(env, buffer);
    free(buffer);

    (*env)->ReleaseStringUTFChars(env, format, c_format);

    return formated;
}

JNIEXPORT jstring JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_utilFormatChar(JNIEnv *env, __UNUSED_PARAM__ jobject that, jstring format, jbyte value)
{
    const char *c_format = (*env)->GetStringUTFChars(env, format, 0);

    size_t needed = snprintf(NULL, 0, c_format, value) + 1;
    char *buffer = malloc(needed);
    snprintf(buffer, needed, c_format, value);

    jstring formated = (*env)->NewStringUTF(env, buffer);
    free(buffer);

    (*env)->ReleaseStringUTFChars(env, format, c_format);

    return formated;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_openElfFile(JNIEnv *env, __UNUSED_PARAM__ jobject unused, jstring file_name)
{
    dpu_elf_file_t file;
    const char *path = (*env)->GetStringUTFChars(env, file_name, 0);
    dpu_error_t err = dpu_elf_open(path, &file);
    return err ? -1L : (long)file;
}

JNIEXPORT jlong JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_mapElfFile(JNIEnv *env,
    __UNUSED_PARAM__ jobject unused,
    jbyteArray buffer,
    jint buffer_size,
    jstring file_or_null)
{
    dpu_elf_file_t file;
    jbyte *fill = (*env)->GetByteArrayElements(env, buffer, 0);
    const char *filename = (file_or_null == NULL) ? NULL : (*env)->GetStringUTFChars(env, file_or_null, 0);
    dpu_error_t err = dpu_elf_map((uint8_t *)fill, (unsigned int)buffer_size, &file, filename);
    if (filename != NULL) {
        (*env)->ReleaseStringUTFChars(env, file_or_null, filename);
    }
    return err ? -1L : (long)file;
}

JNIEXPORT void JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_closeElfFile(__UNUSED_PARAM__ JNIEnv *env, __UNUSED_PARAM__ jobject unused, jlong elf_info)
{
    dpu_elf_file_t file = (dpu_elf_file_t)elf_info;
    dpu_elf_close(file);
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_loadElfProgram(JNIEnv *env,
    __UNUSED_PARAM__ jobject that,
    jlong lp,
    jint ci_id,
    jint dpu_id,
    jlong elf_info,
    jshort mcountAddress,
    jshort retMcountAddress,
    jint threadProfilingAddress)
{
    struct dpu_rank_t *rank = _this_rank(lp);
    struct dpu_t *dpu = dpu_get(rank, (dpu_slice_id_t)ci_id, (dpu_member_id_t)dpu_id);

    dpu_fill_profiling_info(rank, (iram_addr_t)mcountAddress, (iram_addr_t)retMcountAddress, (wram_addr_t)threadProfilingAddress);

    struct _dpu_loader_context_t context;
    dpu_loader_fill_dpu_context(&context, dpu);

    dpu_elf_load((dpu_elf_file_t)elf_info, &context);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuLoaderResult");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(III)V");

    information = (*env)->NewObject(
        env, cls, constructor, context.nr_of_instructions, context.nr_of_wram_words, context.nr_of_mram_bytes);

    flush_outputs();

    return information;
}

JNIEXPORT jobject JNICALL __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_loadElfProgramOnRank(JNIEnv *env, __UNUSED_PARAM__ jobject that, jlong lp, jlong elf_info)
{
    struct _dpu_loader_context_t context;
    dpu_loader_fill_rank_context(&context, _this_rank(lp));

    dpu_elf_load((dpu_elf_file_t)elf_info, &context);

    jobject information;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuLoaderResult");

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(III)V");

    information = (*env)->NewObject(
        env, cls, constructor, context.nr_of_instructions, context.nr_of_wram_words, context.nr_of_mram_bytes);

    flush_outputs();

    return information;
}

JNIEXPORT jbyteArray __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_loadElfSection(JNIEnv *env, __UNUSED_PARAM__ jobject unused, jlong elf_info, jstring name)
{
    const char *c_friendly_name = (*env)->GetStringUTFChars(env, name, 0);
    unsigned int size;
    uint8_t *buffer;
    dpu_error_t err;

    err = dpu_elf_load_section((dpu_elf_file_t)elf_info, c_friendly_name, &size, &buffer);
    if (err) {
        return NULL;
    } else {
        jbyteArray result = (*env)->NewByteArray(env, size);
        if (buffer)
            (*env)->SetByteArrayRegion(env, result, 0, size, (const signed char *)buffer);
        return result;
    }
}

JNIEXPORT void __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_loadElfSymbols(JNIEnv *env,
    __UNUSED_PARAM__ jobject unused,
    jlong elf_info,
    jstring name,
    jobject map)
{
    const char *c_friendly_name = (*env)->GetStringUTFChars(env, name, 0);
    dpu_error_t err;
    dpu_elf_symbols_t *symbols;

    // Preparing a hash map
    jclass hashMapClass = (*env)->FindClass(env, "java/util/HashMap");
    jmethodID put = (*env)->GetMethodID(env, hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jclass elfSymbolClass = (*env)->FindClass(env, "com/upmem/ElfSymbol");
    jmethodID newElfSymbol = (*env)->GetMethodID(env, elfSymbolClass, "<init>", "(II)V");

    err = dpu_elf_load_symbols((dpu_elf_file_t)elf_info, c_friendly_name, &symbols);
    if (!err) {
        unsigned int each_symbol;
        for (each_symbol = 0; each_symbol < symbols->nr_symbols; each_symbol++) {
            jstring symbol_name = (*env)->NewStringUTF(env, symbols->map[each_symbol].name);
            uint32_t value = symbols->map[each_symbol].value;
            uint32_t size = symbols->map[each_symbol].size;

            if ((value & 0x80000000) != 0) {
                value = (value & ~0x80000000) >> 3;
                size = size >> 3;
            } else if ((value & 0x08000000) != 0) {
                value = value & ~0x08000000;
            }

            jobject symbol_value = (*env)->NewObject(env, elfSymbolClass, newElfSymbol, value, size);
            (*env)->CallObjectMethod(env, map, put, symbol_name, symbol_value);
        }
    }
}

JNIEXPORT void __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_getElfSections(JNIEnv *env, __UNUSED_PARAM__ jobject unused, jlong elf_info, jobject list)
{
    dpu_error_t err;
    char **section_names;
    unsigned int nr_sections;

    jclass arrayListClass = (*env)->FindClass(env, "java/util/ArrayList");
    jmethodID add = (*env)->GetMethodID(env, arrayListClass, "add", "(Ljava/lang/Object;)Z");

    err = dpu_elf_get_sections((dpu_elf_file_t)elf_info, &nr_sections, &section_names);
    if (!err) {
        unsigned int each_section;
        for (each_section = 0; each_section < nr_sections; each_section++) {
            jstring section_name = (*env)->NewStringUTF(env, section_names[each_section]);
            (*env)->CallObjectMethod(env, list, add, section_name);
        }
        free(section_names);
    }
}

JNIEXPORT jobject __DPU_JNI_FUNCTION__
Java_com_upmem_dpujni_DpuJNI_getRuntimeInfo(JNIEnv *env, __UNUSED_PARAM__ jobject unused, jlong elf_info)
{
    dpu_elf_runtime_info_t runtime;

    jclass cls = (*env)->FindClass(env, "com/upmem/dpujni/DpuRuntimeInfo");
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jobject runtime_info = (*env)->NewObject(env, cls, ctor);

#define __GET_SYMBOL_METHOD(name) (*env)->GetMethodID(env, cls, name, ("(II)V"))
#define __GET_INT_METHOD(name) (*env)->GetMethodID(env, cls, name, ("(I)V"))
    jmethodID setHeapPointerReset = __GET_SYMBOL_METHOD("setSysHeapPointerReset");
    jmethodID setHeapPointer = __GET_SYMBOL_METHOD("setSysHeapPointer");
    jmethodID setSysWqTable = __GET_SYMBOL_METHOD("setSysWqTable");
    jmethodID setStackTable = __GET_SYMBOL_METHOD("setStackTable");
    jmethodID setStdoutBuffer = __GET_SYMBOL_METHOD("setStdoutBuffer");
    jmethodID setStdoutWritePointer = __GET_SYMBOL_METHOD("setStdoutWritePointer");
    jmethodID setSysEnd = __GET_SYMBOL_METHOD("setSysEnd");
    jmethodID setMcount = __GET_SYMBOL_METHOD("setMcount");
    jmethodID setRetMcount = __GET_SYMBOL_METHOD("setRetMcount");
    jmethodID setThreadProfiling = __GET_SYMBOL_METHOD("setThreadProfiling");
    jmethodID setNrThreads = __GET_INT_METHOD("setNrThreads");

#define __SET_SYMBOL_INFO(method, field)                                                                                         \
    if (field.exists)                                                                                                            \
    (*env)->CallObjectMethod(env, runtime_info, method, field.value, field.size)
#define __SET_INT_INFO(method, field)                                                                                            \
    if (field.exists)                                                                                                            \
    (*env)->CallObjectMethod(env, runtime_info, method, field.value)

    dpu_elf_get_runtime_info((dpu_elf_file_t)elf_info, &runtime);
    __SET_SYMBOL_INFO(setHeapPointerReset, runtime.sys_heap_pointer_reset);
    __SET_SYMBOL_INFO(setHeapPointer, runtime.sys_heap_pointer);
    __SET_SYMBOL_INFO(setSysWqTable, runtime.sys_wq_table);
    __SET_SYMBOL_INFO(setStackTable, runtime.sys_stack_table);
    __SET_SYMBOL_INFO(setStdoutBuffer, runtime.printf_buffer);
    __SET_SYMBOL_INFO(setStdoutWritePointer, runtime.printf_state);
    __SET_SYMBOL_INFO(setSysEnd, runtime.sys_end);
    __SET_SYMBOL_INFO(setMcount, runtime.mcount);
    __SET_SYMBOL_INFO(setRetMcount, runtime.ret_mcount);
    __SET_SYMBOL_INFO(setThreadProfiling, runtime.thread_profiling);
    __SET_INT_INFO(setNrThreads, runtime.nr_threads);

    // Preparing a hash map
    jclass hashMapClass = (*env)->FindClass(env, "java/util/HashMap");
    jmethodID put = (*env)->GetMethodID(env, hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jclass elfSymbolClass = (*env)->FindClass(env, "com/upmem/ElfSymbol");
    jmethodID newElfSymbol = (*env)->GetMethodID(env, elfSymbolClass, "<init>", "(II)V");

    jmethodID getMutexInfo = (*env)->GetMethodID(env, cls, "getMutexInfo", ("()Ljava/util/HashMap;"));
    jobject mutexMap = (*env)->CallObjectMethod(env, runtime_info, getMutexInfo);

    unsigned int each_symbol;
    for (each_symbol = 0; each_symbol < runtime.mutex_info.nr_symbols; each_symbol++) {
        jstring symbol_name = (*env)->NewStringUTF(env, runtime.mutex_info.map[each_symbol].name);
        jobject symbol_value = (*env)->NewObject(env,
            elfSymbolClass,
            newElfSymbol,
            runtime.mutex_info.map[each_symbol].value,
            runtime.mutex_info.map[each_symbol].size);
        (*env)->CallObjectMethod(env, mutexMap, put, symbol_name, symbol_value);
    }

    jmethodID getSemaphoreInfo = (*env)->GetMethodID(env, cls, "getSemaphoreInfo", ("()Ljava/util/HashMap;"));
    jobject semaphoreMap = (*env)->CallObjectMethod(env, runtime_info, getSemaphoreInfo);

    for (each_symbol = 0; each_symbol < runtime.semaphore_info.nr_symbols; each_symbol++) {
        jstring symbol_name = (*env)->NewStringUTF(env, runtime.semaphore_info.map[each_symbol].name);
        jobject symbol_value = (*env)->NewObject(env,
            elfSymbolClass,
            newElfSymbol,
            runtime.semaphore_info.map[each_symbol].value,
            runtime.semaphore_info.map[each_symbol].size);
        (*env)->CallObjectMethod(env, semaphoreMap, put, symbol_name, symbol_value);
    }

    jmethodID getBarrierInfo = (*env)->GetMethodID(env, cls, "getBarrierInfo", ("()Ljava/util/HashMap;"));
    jobject barrierMap = (*env)->CallObjectMethod(env, runtime_info, getBarrierInfo);

    for (each_symbol = 0; each_symbol < runtime.barrier_info.nr_symbols; each_symbol++) {
        jstring symbol_name = (*env)->NewStringUTF(env, runtime.barrier_info.map[each_symbol].name);
        jobject symbol_value = (*env)->NewObject(env,
            elfSymbolClass,
            newElfSymbol,
            runtime.barrier_info.map[each_symbol].value,
            runtime.barrier_info.map[each_symbol].size);
        (*env)->CallObjectMethod(env, barrierMap, put, symbol_name, symbol_value);
    }

    free_runtime_info(&runtime);

    return runtime_info;
}
