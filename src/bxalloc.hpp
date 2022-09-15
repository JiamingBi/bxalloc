/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#ifndef _bxalloc_HPP_
#define _bxalloc_HPP_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
/* return 1 if it's a restart, otherwise 0. */
extern "C" int BXP_init(const char* _id, uint64_t size = 10*1024*1024*1024ULL);
#include "BaseMeta.hpp"
namespace bxalloc{
    extern bool initialized;
    /* persistent metadata and their layout */
    extern BaseMeta* base_md;
};
template<class T>
T* BXP_get_root(uint64_t i){
    assert(bxalloc::initialized);
    return bxalloc::base_md->get_root<T>(i);
}
extern "C"{
#else /* __cplusplus ends */
// This is a version for pure c only
void* BXP_get_root_c(uint64_t i);
/* return 1 if it's a restart, otherwise 0. */
int BXP_init(const char* _id, uint64_t size);
#endif

/* return 1 if it's dirty, otherwise 0. */
int BXP_recover();
void BXP_close();
void* BXP_malloc(size_t sz);
void BXP_free(void* ptr);
void* BXP_set_root(void* ptr, uint64_t i);
size_t BXP_malloc_size(void* ptr);
void* BXP_calloc(size_t num, size_t size);
void* BXP_realloc(void* ptr, size_t new_size);
/* return 1 if ptr is in range of bxalloc heap, otherwise 0. */
int BXP_in_prange(void* ptr);
/* return 1 if the query is invalid, otherwise 0 and write start and end addr to the parameter. */
int BXP_region_range(int idx, void** start_addr, void** end_addr);
void *BXP_getstart();
void BXP_output(int size, int count, _Float64 **objsize);
#ifdef __cplusplus
}
#endif

#define BXP_pthread_create(thd, attr, f, arg) pm_thread_create(thd, attr, f, arg)
/*
 ************class bxalloc************
 * This is a persistent lock-free allocator based on LRMalloc.
 *
 * Function:
 * 		_init(string id, uint64_t thd_num):
 * 			Construct the singleton with id to decide where the data 
 * 			maps to. If the file exists, it tries to restart; otherwise,
 * 			it starts from scratch.
 * 		_close():
 * 			Shutdown the allocator by cleaning up free list 
 * 			and RegionManager pointer, but BaseMeta data will
 * 			preserve for remapping during restart.
 * 		T* _p_malloc<T>(size_t sz):
 * 			Malloc a block with size sz and type T.
 * 			Currently it only supports small allocation (<=MAX_SMALLSIZE).
 * 			If T is not void, sz will be ignored.
 * 		void _p_free(void* ptr):
 * 			Free the block pointed by ptr.
 * 		void* _set_root(void* ptr, uint64_t i):
 * 			Set i-th root to ptr, and return old i-th root if any.
 * 		void* _get_root(uint64_t i):
 * 			Return i-th root.
 *
 * Note: Main data is stored in *base_md and is mapped to 
 * filepath, which is $(HEAPFILE_PREFIX)$(id).


 * It's from paper: 
 * 		LRMalloc: A Modern and Competitive Lock-Free Dynamic Memory Allocator
 * by 
 * 		Ricardo Leite and Ricardo Rocha
 *
 * p_malloc() and p_free() have large portion of code from the open source
 * project https://github.com/ricleite/lrmalloc.
 *
 * Adapted and reimplemented by:
 * 		Wentao Cai (wcai6@cs.rochester.edu)
 *
 */

#endif /* _bxalloc_HPP_ */
