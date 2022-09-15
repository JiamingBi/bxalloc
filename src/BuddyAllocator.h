/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include "pfence_util.h"
#include "RegionManager.hpp"
#include <mutex>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
	bool free;			// is the block free or used
	int block_size;  	// size of the block
	BlockHeader* next; 	// pointer to the next block

public:
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeaders.
public:
	BlockHeader* head;		// you need a head of the list
public:
	void insert (BlockHeader* b){	// adds a block to the list
		b->next = head;
		head = b;
	}

	void remove (BlockHeader* b){  // removes a block from the list
		// free memory of b?
		BlockHeader* currPtr = head;
		if(head == b){
			head = head->next;
		}
		else{
			while(currPtr){
				if(currPtr->next == b){
					currPtr->next = currPtr->next->next;
				}
				currPtr = currPtr->next;
			}
		}
	}
};


class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;
	int max_block_size;  //todo
public:
	void* start;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	BlockHeader* getbuddy (BlockHeader* addr, char* start);
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2, char* start);
	// checks whether the two blocks are buddies or not
	// note that two adjacent blocks are not buddies when they are different sizes

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected


public:
	BuddyAllocator(){};
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. 
	*/ 
	BuddyAllocator (void *start,int _basic_block_size,int max_block_size, int _total_memory_length,bool recover); 
	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	void* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	void free(void* _a); 
	/* Frees the section of physical memory previously allocated 
	   using alloc(). */ 
	bool get_free_memory(size_t size);
	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function prints how many free blocks of each size belong to the allocator
	at that point. It also prints the total amount of free memory available just by summing
	up all these blocks.
	Assuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

class BuddyManager{
public:
	BuddyManager(){};
	BuddyManager(void *start,int a,int b,int c,bool p){
		largesb=start;
		baseblock=a;
		maxblock=b;
		total=c;
		revover=p;
	}
	~BuddyManager(){
		cleanup();
	};
	void initBuddyAllocator(){
		///std::cout<<largesb<<" "<<baseblock<<" "<<maxblock<<" "<<total<<" "<<revover<<std::endl;
		_buddy=new BuddyAllocator(largesb,baseblock,maxblock,total,revover);
	}
	void *balloc(size_t size){
		std::lock_guard<std::mutex> lock(buddymtx);
		return _buddy->alloc(size);
	}
	void bfree(void *ptr){
		std::lock_guard<std::mutex> lock(buddymtx);
		_buddy->free(ptr);
	}
	void cleanup(){
		//delete mgr;
		//delete _buddy;
	}
private:
	//const std::string id;
	/* manager to map, remap, and unmap the heap */
	//RegionManager* mgr;//initialized when ArrayQueue constructs
	void *largesb=nullptr;
	int baseblock=128*1024;
	int maxblock=1024*1024*1024;
	int total=1024*1024*1024;
	bool revover=false;
	BuddyAllocator *_buddy;
	std::mutex buddymtx;
};

#endif 
