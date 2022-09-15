/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#include "bxalloc.hpp"

#include <string>
#include <functional>
#include <atomic>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include "RegionManager.hpp"
#include "BaseMeta.hpp"
#include "pm_config.hpp"
#include "LockFreeQueue.hpp"
#include <optional>
#include "BuddyAllocator.h"

using namespace std;

namespace bxalloc{
    bool initialized = false;
    /* persistent metadata and their layout */
    BaseMeta* base_md;
    Regions* _rgs;
    BuddyManager *buddymanager;
    LockFreeQueue<uint64_t,80000> *_queue;
    std::function<void(const CrossPtr<char, SB_IDX>&, GarbageCollection&)> roots_filter_func[MAX_ROOTS];
    ClassSize myclasssize;
    //extern SizeClass sizeclass;
    
};
using namespace bxalloc;
extern void public_flush_cache();

int _BXP_init(const char* _id, uint64_t size){
    string filepath;
    string id(_id);
    // thread_num = thd_num;

    // reinitialize global variables in case they haven't
    //new (&sizeclass) SizeClass();

    filepath = HEAPFILE_PREFIX + id;
    assert(sizeof(Descriptor) == DESCSIZE); // check desc size
    assert(size < MAX_SB_REGION_SIZE && size >= MIN_SB_REGION_SIZE); // ensure user input is >=MAX_SB_REGION_SIZE
    uint64_t sbsize=0.5*size;

    uint64_t num_sb = size/SBSIZE;
    bool restart = Regions::exists_test(filepath+"_basemd");
    bool lagersbstart=false;
    bool recover=RegionManager::exists_test(filepath+"_large_sb");
    _rgs = new Regions();
    for(int i=0; i<LAST_IDX;i++){
    switch(i){
    case DESC_IDX:
        _rgs->create(filepath+"_desc", num_sb*DESCSIZE, true, true);
        break;
    case SB_IDX:
        _rgs->create(filepath+"_sb", num_sb*SBSIZE, true, false);
        break;
    case LAR_SB_IDX:
        _rgs->create(filepath+"_large_sb",MIN_SB_REGION_SIZE,true,true);
        _rgs->regions[LAR_SB_IDX]->FILESIZE=(size-sbsize)/SBSIZE*SBSIZE;
        break;
    case QUEUE_IDX:
        _queue=new LockFreeQueue<uint64_t,80000>(filepath+"_queue");
        _rgs->regions[i]=_queue->getmgr();
        _rgs->regions_address[i]=_queue->getregionstart();
        _rgs->cur_idx++;
        break;
    case META_IDX:
        base_md = _rgs->create_for<BaseMeta>(filepath+"_basemd", sizeof(BaseMeta), true);
        break;
    } // switch
    }
    
    char *startaddress=_rgs->lookup(LAR_SB_IDX);
    //bool recover=RegionManager::exists_test(filepath+"_large_sb");
    char * curr_addr = _rgs->regions[LAR_SB_IDX]->curr_addr_ptr->load();
    int totalmemory=int((uint64_t)curr_addr-(uint64_t)startaddress);
    buddymanager=new BuddyManager(startaddress,128*1024,MIN_SB_REGION_SIZE,totalmemory,recover);
    buddymanager->initBuddyAllocator();
    //std::cout<<totalmemory<<std::endl;
    initialized = true;
    return (int)restart;
}

struct bxallocHolder{
    int init_ret_val;
    bxallocHolder(const char* _id, uint64_t size){
        init_ret_val = _BXP_init(_id,size);
    }
    ~bxallocHolder(){
        // #ifndef MEM_CONSUME_TEST
        // flush_region would affect the memory consumption result (rss) and 
        // thus is disabled for benchmark testing. To enable, simply comment out
        // -DMEM_CONSUME_TEST flag in Makefile.
        _rgs->flush_region(DESC_IDX);
        _rgs->flush_region(SB_IDX);
        // #endif
        base_md->writeback();
       // _queue->cleanup();
        initialized = false;
        delete _rgs;
    }
};

/* 
 * mmap the existing heap file corresponding to id. aka restart,
 * 		and if multiple heaps exist, print out and let user select;
 * if such a heap doesn't exist, create one. aka start.
 * id is the distinguishable identity of applications.
 */
int BXP_init(const char* _id, uint64_t size){
    static bxallocHolder _holder(_id,size);
    return _holder.init_ret_val;
}

int BXP_recover(){
    return (int) base_md->restart();
}

// we assume BXP_close is called by the last exiting thread.
void BXP_close(){
    // Wentao: this is a noop as the real function body is now i ~bxallocHolder
}

void* BXP_malloc(size_t sz){
    assert(initialized&&"RPMalloc isn't initialized!");
    //void *ret=nullptr;
    if(UNLIKELY(sz>SBSIZE))
        return buddymanager->balloc(sz);
    //if(ret!=nullptr) return ret;
    return base_md->do_malloc(sz);
}

void BXP_free(void* ptr){
    assert(initialized&&"RPMalloc isn't initialized!");
    if(_rgs->in_range(LAR_SB_IDX,ptr)) buddymanager->bfree(ptr);
    else
    base_md->do_free(ptr);
}

void* BXP_set_root(void* ptr, uint64_t i){
    if(bxalloc::initialized==false){
        BXP_init("no_explicit_init");
    }
    return base_md->set_root(ptr,i);
}
void* BXP_get_root_c(uint64_t i){
    assert(initialized);
    return (void*)base_md->get_root<char>(i);
}

// return the size of ptr in byte.
// No check for whether ptr is allocated or isn't null
size_t BXP_malloc_size(void* ptr){
    const Descriptor* desc = base_md->desc_lookup(ptr);
    return (size_t)desc->block_size;
}

void* BXP_realloc(void* ptr, size_t new_size){
    if(ptr == nullptr) return BXP_malloc(new_size);
    if(!_rgs->in_range(SB_IDX, ptr)) return nullptr;
    size_t old_size = BXP_malloc_size(ptr);
    if(old_size == new_size) {
        return ptr;
    }
    void* new_ptr = BXP_malloc(new_size);
    if(UNLIKELY(new_ptr == nullptr)) return nullptr;
    memcpy(new_ptr, ptr, old_size);
    FLUSH(new_ptr);
    FLUSHFENCE;
    BXP_free(ptr);
    return new_ptr;
}

void* BXP_calloc(size_t num, size_t size){
    void* ptr = BXP_malloc(num*size);
    if(UNLIKELY(ptr == nullptr)) return nullptr;
    size_t real_size = BXP_malloc_size(ptr);
    memset(ptr, 0, real_size);
    FLUSH(ptr);
    FLUSHFENCE;
    return ptr;
}

int BXP_in_prange(void* ptr){
    if(_rgs->in_range(SB_IDX,ptr)) return 1;
    else return 0;
}

int BXP_region_range(int idx, void** start_addr, void** end_addr){
    if(start_addr == nullptr || end_addr == nullptr || idx>=_rgs->cur_idx){
        return 1;
    }
    *start_addr = (void*)_rgs->regions_address[idx];
    *end_addr = (void*) ((uint64_t)_rgs->regions_address[idx] + _rgs->regions[idx]->FILESIZE);
    return 0;
}
void *BXP_getstart(){
    return (void*)_rgs->regions_address[SB_IDX];
}
void BXP_output(int size, int count, _Float64 **objsize){
  //_rgs->regions[i]->_get_region_used();
  Descriptor* des_start;
  char* sb_start_address = _rgs->regions_address[1];
  char* sb_end_address = _rgs->regions[SB_IDX]->curr_addr_ptr->load();
  //_Float64 small_rate = 0;
  //_Float64 large_rate = 0;
  size_t small_alloc_count = 0;
  size_t large_alloc_count = 0;
  size_t total_alloc_count = 0;
  //sb_start_address+=0x1234;
  //first sb
  while(sb_start_address <= sb_end_address)
  {
    des_start = base_md->desc_lookup(sb_start_address);
    Anchor this_anchor = des_start->anchor.load();
    if(this_anchor.state == SB_FULL && this_anchor.avail == 0 && this_anchor.count == 0 )// this is a large alloc
    {
      size_t blk_size = des_start->block_size;
      float f_sb_count = (float) blk_size / 65536;
      size_t sb_count = ceil(f_sb_count);
      objsize[0][0] += sb_count; 
      objsize[1][0] += sb_count;
      large_alloc_count += sb_count;
      total_alloc_count += sb_count;
    }
    else if(this_anchor.state != SB_ERROR) // this is a small alloc
    {
      small_alloc_count++;
      total_alloc_count++;
      int pos = ClassSize::Index(des_start->block_size);
      _Float64 small_rate;
      if(this_anchor.state == SB_FULL)
        small_rate = 1;// all blocks are used
      else if(this_anchor.state == SB_EMPTY)
        small_rate = (_Float64) this_anchor.count / (_Float64) des_start->maxcount;// all blocks are empty but init
      else
        small_rate = (_Float64) (this_anchor.count) / (_Float64) des_start->maxcount;
      _Float64 big_rate = (_Float64) des_start->maxcount * (_Float64) des_start->block_size / (_Float64) SBSIZE;
      objsize[0][pos] += small_rate * big_rate;
      objsize[1][pos] ++;
      total_alloc_count++;
      small_alloc_count++;
    }
    sb_start_address += 0x10000; //64kb, a superblock
  }

  std::cout<<"total alloced "<<total_alloc_count<<"super block, and "<<"small alloc used "<<small_alloc_count<<" large alloc used "<<large_alloc_count<<std::endl;
};