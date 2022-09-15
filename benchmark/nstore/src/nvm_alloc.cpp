#include <nvm_malloc.h>
#include "clibpm.h"

using namespace std;

namespace storage{

void *nvm_alloc::pmemalloc_init_helper(const char *path, size_t size){
      return nvm_initialize("/mnt/pmem/",0);
}
void *nvm_alloc::pmemalloc_static_area_helper(){
	return NULL;

}

void *nvm_alloc::pmemalloc_reserve_helper(size_t size){
    return nvm_reserve(size);
}

void nvm_alloc::pmemalloc_free_helper(void *abs_ptr_){
    return nvm_free(abs_ptr_,nullptr,nullptr,nullptr,nullptr);
}


void nvm_alloc::pmemalloc_check_helper(const char *path){
	return ;
}

void nvm_alloc::pmemalloc_activate_helper(void *abs_ptr_){
    nvm_activate(abs_ptr_,nullptr,nullptr,nullptr,nullptr);
	return ;
}

void nvm_alloc::pmemalloc_close_helper(){
    nvm_teardown();
    return ;
}
}