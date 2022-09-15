/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */
#ifndef _ClassSize_HPP_
#define _ClassSize_HPP_

#include "pm_config.hpp"
#include "pfence_util.h"
#include "pptr.hpp"
#include "RegionManager.hpp"
#include <unordered_map>
#include<iostream>
using namespace std;
class ClassSize
{
	// 控制在12%左右的内碎片浪费,申请小内存的时候，对齐数也小，这样又能控制内存大小，又能避免太多的内存碎片浪费，不然你申请16个字节，只用一个字节，不是很亏
	// [1,128]				8byte对齐		freelist[0,16)  
	// [129,1024]			16byte对齐		freelist[16,72)   16/129???
	// [1025,8*1024]		128byte对齐		freelist[72,128)  128/1025???
	// [8*1024+1,24*1024]	512byte对齐		freelist[128,160)
public:
	//对齐规则，(size+7)&~7
	
	unordered_map<size_t,size_t> sizetoindex;
	size_t indextosize[NLIST];
	ClassSize(){
		for(size_t i=0;i<NLIST;i++)
		{
			indextosize[i]=getsizefromindex(i);
			sizetoindex.insert(pair<size_t,size_t>(indextosize[i],i));
			/*std::cout<<i<<" "<<indextosize[i]<<std::endl;
			
    		auto iterator = sizetoindex.find(indextosize[i]);//find()返回一个指向2的迭代器
    		if (iterator != sizetoindex.end())
	    	cout <<iterator->first << "," << iterator->second << endl<<endl;*/
			}
	}
	static inline size_t _RoundUp(size_t size, size_t align)
	{
		return (size + align - 1)&~(align - 1);
	}

	//给我算以什么来对齐，在上面的范围里，向上取整对齐
	//推算出对齐数
	static inline size_t RoundUp(size_t size)
	{
		assert(size <= SBSIZE);
		if (size <= 128){
			return _RoundUp(size, 8);
		}
		else if (size <= 1024){
			return _RoundUp(size, 16);
		}
		else if (size <= 8192){
			return _RoundUp(size, 128);
		}
		else if (size <= 65536){
			return _RoundUp(size, 512);
		}
		else
			return -1;
	}

	//映射规则，align_shift对齐数
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		//比如第一个块，(size+7)>>3-1，减一是因为不是从0开始的
		return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
	}

	//映射，哪个位数后面挂的链表
	static inline size_t Index(size_t bytes)
	{
		assert(bytes < SBSIZE);
		//每个区间有多少个自由链表
		static int group_array[4] = { 16, 56, 56, 48 };
		if (bytes <= 128)
		{
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024)
		{
			//_Index(减去第一个区间的，16对应的是4)+上一个区间的链表数，就是我现在的位置
			return _Index(bytes - 128, 4) + group_array[0];
		}
		else if (bytes <= 8192)
		{
			return _Index(bytes - 1024, 7) + group_array[0] + group_array[1];
		}
		else if (bytes <= 24*1024)
		{
			return _Index(bytes - 8192, 9) + group_array[0] + group_array[1] + group_array[2];
		}
		else
			return -1;

	}
    static inline size_t getsizefromindex(size_t index){
        assert(index<NLIST);
        if(index<16) return (index+1)*8;
        else if(index<72) return 128+(index+1-16)*16;
        else if(index<128) return 1024+(index+1-72)*128;
        else if(index<160) return 8*1024+(index+1-128)*512;
        else return -1;
    } 
};

#endif