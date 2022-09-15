/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
#include "BaseMeta.hpp"
using namespace std;
using namespace bxalloc;
namespace bxalloc{
	extern Regions* _rgs;
}
BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
	basic_block_size = _basic_block_size;
	total_memory_size = _total_memory_length;
	start = malloc(total_memory_size);
	int numBlocks = log2(_total_memory_length/_basic_block_size)+1;
	FreeList.resize(numBlocks);
	FreeList[numBlocks - 1].head = (BlockHeader*)start;
	FreeList[numBlocks - 1].head->block_size = _total_memory_length;
	FreeList[numBlocks - 1].head->free = true;
	FreeList[numBlocks - 1].head->next = nullptr;
}
BuddyAllocator::BuddyAllocator (void *start,int _basic_block_size, int max_block_size,int _total_memory_length,bool recover){
	basic_block_size = _basic_block_size;
	total_memory_size = (_total_memory_length/_basic_block_size)*_basic_block_size;
	this->start = start;
//	int numBlocks = log2(_total_memory_length/_basic_block_size)+1;
	int numBlocks = log2(max_block_size/_basic_block_size)+1;
	//std::cout<<numBlocks<<std::endl;

	FreeList.resize(numBlocks);
	char *temp=(char*)start;
	if(!recover){
		//std::cout<<"重建"<<std::endl;
	for(int i=0;i<_total_memory_length/max_block_size;i++){
		BlockHeader* b=(BlockHeader*)temp;
		new (b) BlockHeader();
		b->block_size=max_block_size;
		b->free=true;
		b->next=nullptr;
		FLUSH(b);
		FLUSHFENCE;
		FreeList[numBlocks-1].insert(b);
		temp=temp+max_block_size;
	  }
	}
	else{
		//std::cout<<"恢复"<<std::endl;
		char* curr=(char*) start;
		while(curr<(char*)start+_total_memory_length){
			BlockHeader *curr_block=(BlockHeader*)curr;
			if(curr_block->free){
				int idx=log2(curr_block->block_size/_basic_block_size);
				FreeList[idx].insert(curr_block);
			}
			curr+=curr_block->block_size;
		}
	}
	/*FreeList[numBlocks - 1].head = (BlockHeader*)start;
	FreeList[numBlocks - 1].head->block_size = _total_memory_length;
	FreeList[numBlocks - 1].head->free = true;
	FreeList[numBlocks - 1].head->next = nullptr;*/
}
BuddyAllocator::~BuddyAllocator (){
	//::free(start);
	//for(int i=0;i<FreeList.size();i++)
	FreeList.clear();
}

BlockHeader* getbuddy (BlockHeader* addr, char* start){
	return (BlockHeader*)((((char*)addr - start) ^ addr->block_size) + start);
}
// given a block address, this function returns the address of its buddy

bool arebuddies (BlockHeader* block1, BlockHeader* block2, char* start){
	if(getbuddy(block1, start) == block2){
		return true;
	}
	return false;
}
// checks whether the two blocks are buddies or not
// note that two adjacent blocks are not buddies when they are different sizes

BlockHeader* merge (BlockHeader* block1, BlockHeader* block2){
	if(block1 < block2){
		block1->block_size *= 2;
		FLUSH(block1);
		FLUSHFENCE;
		return block1;
	}
	else{
		block2->block_size *= 2;
		FLUSH(block2);
		FLUSHFENCE;
		return block2;
	}
}
// this function merges the two blocks returns the beginning address of the merged block
// note that either block1 can be to the left of block2, or the other way around

BlockHeader* split (BlockHeader* block){
	BlockHeader* block1 = block;
	BlockHeader* block2 = (BlockHeader*)((char*)block + (block->block_size / 2));

	block2->free = true;
	block2->block_size = block->block_size / 2;
	block2->next = nullptr;
	FLUSH(block2);
	block1->next = block2;
	block1->block_size = block->block_size / 2;
	FLUSH(block1);
	FLUSHFENCE;
	return block1;
}
// splits the given block by putting a new header halfway through the block
// also, the original header needs to be corrected

void* BuddyAllocator::alloc(int length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
	int i = 0;
	int len=FreeList.size();
	while(!FreeList[i].head || length + int(sizeof(BlockHeader)) > (1<<i)*basic_block_size){
		i++;
		if(i == len){
			//cout << length << endl;
			void* ret = nullptr;
			std::cout << "Request larger than available memory " << std::endl;
			int res = 0;
    		while(res == 0) {
        	res = _rgs->expand(LAR_SB_IDX,&ret,PAGESIZE, MIN_SB_REGION_SIZE);
        	assert(res != -1 && "space runs out!");
    		}
			std::cout<<ret<<endl;
			char *temp=(char*)ret;
			total_memory_size+=MIN_SB_REGION_SIZE;
			BlockHeader* b=(BlockHeader*)temp;
			new (b) BlockHeader();
			b->block_size=max_block_size;
			b->free=true;
			b->next=nullptr;
			FLUSH(b);
			FLUSHFENCE;
			std::cout<<"a"<<std::endl;
			FreeList[len-1].insert(b);
			std::cout<<"b"<<std::endl;
			return alloc(length);
		}
		// return 0 if there is not sufficient available space in memory
	}
	// traverse FreeList until a non null BlockHeader with size greater than request

	while(FreeList[i].head->block_size / 2 > length + int(sizeof(BlockHeader)) &&
			FreeList[i].head->block_size / 2 >= basic_block_size){

		BlockHeader* b = FreeList[i].head->next;
		FreeList[i-1].head = ::split(FreeList[i].head);
		FreeList[i].head = b;
		i--;
	}
	// split memory until basic block size is reached or smallest power of 2 greater than request
	// update FreeList simultaneously

	BlockHeader* b = FreeList[i].head;
	FreeList[i].remove(FreeList[i].head);	// remove allocated block of memory from FreeList
	b->free = false;
	//printlist();
	return b + 1;	// return address of memory block plus the size of the block header
}

void BuddyAllocator::free(void* a) {
  /* Same here! */
	BlockHeader* b = (BlockHeader*)a - 1;
	b->free = true;

	int i = log2(b->block_size/basic_block_size);
	int len=FreeList.size();
	if(::getbuddy(b,(char*)start)->free && b->block_size == ::getbuddy(b,(char*)start)->block_size){
		while(::getbuddy(b,(char*)start)->free && i < len - 1){
			FreeList[i].remove(b);
			FreeList[i].remove(::getbuddy(b,(char*)start));
			b = ::merge(b,::getbuddy(b,(char*)start));
			FreeList[i+1].insert(b);
			i++;
		}
	}
	else{
		FLUSH(b);
		FLUSHFENCE;
		FreeList[i].insert(b);
	}
	

}

bool BuddyAllocator::get_free_memory(size_t size){
  size_t total_free_memory = 0;
  int len=FreeList.size();
  for (int i=0; i<len; i++){
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      total_free_memory += blocksize;
	  if(total_free_memory>=size) return true;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != blocksize){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
  }	
  return false;
}

void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  int len=FreeList.size();
  for (int i=0; i<len; i++){
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      total_free_memory += blocksize;
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != blocksize){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;
    cout << "Amount of available free memory: " << total_free_memory << " bytes" << endl;
  }
}

