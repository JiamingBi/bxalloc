/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */

#ifndef __FREELIST_HPP_
#define __FREELIST_HPP_

#include "pm_config.hpp"
#include "pfence_util.h"
#include "pptr.hpp"
#include "RegionManager.hpp"


class FreeList
{
public:
	char *head_list(){
		return _list;
	}
	bool Empty(){
		return _list == nullptr;
	}
	void PushRange(char* start,char* end,uint32_t  num){
		*(pptr<char>*)(end) = _list;
		_list = start;
		_size += num;
	}
    void PushList(char *obj,uint32_t num){
        _list=obj;
        _size=num;
		//std::cout<<"pushlist size:"<<_size<<std::endl;
    }
    void PopList(char* obj, size_t length){
        _list = obj;
        _size-=length;
    }
	char* Clear(){
		//不置maxsize，因为这次释放回来，这个线程已经申请过大批量的内存了，下次可能还会大批量的申请，就不用慢增长了
		_size = 0;
		char* list = _list;
		_list = nullptr;//链表置空，你就不留了，给上交给centralcache
		return list;
	}
	char* Pop(){
		//assert(_size);
		//assert(_list);
		char* obj = _list;

		_list = *(pptr<char>*)(obj);
		--_size;
		return obj;
	}
	void Push(char* obj){
		*(pptr<char>*)(obj) = _list;
		_list = obj;
		++_size;
	}
	uint32_t Size(){
		return _size;
	}
	void SetMaxSize(uint32_t maxsize){
		_maxsize = maxsize;
	}
	uint32_t Maxsize(){
		return _maxsize;
	}
	void setinit(){
		init=true;
	}
	bool getinit(){
		return init;
	}
private:
	char* _list=nullptr;//相当于下一个地址
	bool init=false;
	uint32_t _size=0;//当前链表有多少个对象
	uint32_t _maxsize=1;//当前链表的最大值
};

#endif

