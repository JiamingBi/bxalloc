/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#include "bxalloc.hpp"
#include <cstring>
#include <string>
#include "clibpm.h"

using namespace std;
extern unsigned long long libpm_min;
extern unsigned long long libpm_max;
namespace storage{

void *bx_alloc::pmemalloc_init_helper(const char *path, size_t size){
      string filename=path;
      BXP_init("bxalloc", REGION_SIZE);
      return BXP_getstart();
}
void *bx_alloc::pmemalloc_static_area_helper(){
	return NULL;

}

void *bx_alloc::pmemalloc_reserve_helper(size_t size){
	void *ret=BXP_malloc(size);
//	printf("min=%lld max=%lld p=%lld\n",libpm_min,libpm_max,(unsigned long long)ret);
//	if(libpm_min<=(unsigned long long)ret&&(unsigned long long)ret<=libpm_max) printf("true\n");
//	else printf("false\n");
	
	return ret;
}

void bx_alloc::pmemalloc_free_helper(void *abs_ptr_){
	BXP_free(abs_ptr_);
}


void bx_alloc::pmemalloc_check_helper(const char *path){
	return ;
}

void bx_alloc::pmemalloc_activate_helper(void *abs_ptr_){
	return ;
}

void bx_alloc::pmemalloc_close_helper(){
	BXP_close();
}
}