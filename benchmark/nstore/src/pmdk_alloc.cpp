/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */
#include <libpmemobj.h>
#include "clibpm.h"

using namespace std;

namespace storage{

PMEMobjpool* pop;
PMEMoid root;
struct PMDK_roots{
    void* roots[1024];
  };
static int dummy_construct(PMEMobjpool *pop, void *ptr, void *arg){return 0;}
void *pmdk_alloc::pmemalloc_init_helper(const char *path, size_t size){
    pop = pmemobj_create("/mnt/pmem/pmdk", "pmdk", REGION_SIZE, 0666);
  //  printf("pop:%lld",(unsigned long long )(void *)pop);
    if (pop == nullptr) {
      perror("pmemobj_create");
    }
    else {
      root = pmemobj_root(pop, sizeof (PMDK_roots));
    }
      return (void *)pop;
}
void *pmdk_alloc::pmemalloc_static_area_helper(){
	return NULL;

}

void *pmdk_alloc::pmemalloc_reserve_helper(size_t size){
    PMEMoid temp_ptr;
    int ret=pmemobj_alloc(pop, &temp_ptr, size, 0, dummy_construct,nullptr);
    if(ret==-1)return nullptr;
    return pmemobj_direct(temp_ptr);
}

void pmdk_alloc::pmemalloc_free_helper(void *abs_ptr_){
    if(abs_ptr_==nullptr) return;
    PMEMoid temp_ptr;
    temp_ptr = pmemobj_oid(abs_ptr_);
    pmemobj_free(&temp_ptr);
}


void pmdk_alloc::pmemalloc_check_helper(const char *path){
	return ;
}

void pmdk_alloc::pmemalloc_activate_helper(void *abs_ptr_){
	return ;
}

void pmdk_alloc::pmemalloc_close_helper(){
   pmemobj_close(pop);
}
}