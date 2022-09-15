/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */
#include "ThreadCache.h"

using namespace bxalloc;
namespace bxalloc{
    thread_local ThreadCache tsl_cache;
    extern ClassSize myclasssize;
}
namespace bxalloc{
	extern void public_flush_cache();
}

ThreadCache::ThreadCache(){
    for(int i=0;i<NLIST;i++){
      //  uint32_t temp=SBSIZE/ClassSize::getsizefromindex(i);
       uint32_t temp=SBSIZE/myclasssize.indextosize[i];
        //std::cout<<i<<" "<<temp<<"  "<<ClassSize::getsizefromindex(i)<<std::endl;
        if(temp>2048) _flist[i].SetMaxSize(2048);
        else if(temp>512) _flist[i].SetMaxSize(512);
      //  else if(temp>64) _flist[i].SetMaxSize(64);
        else
         _flist[i].SetMaxSize(temp);  
    }
}
ThreadCache::~ThreadCache(){
    bxalloc::public_flush_cache();
}