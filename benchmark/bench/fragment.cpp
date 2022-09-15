#include "AllocatorMacro.hpp"
#include <iostream>
#include <string>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctime>
using namespace std;

#define THRESHOLD 2147483648 //2GB
struct obj{
    void* obj_ptr;
    size_t size;
};

void sig_abt(int count)
{
   // my_allocator->print();
    exit(1);
}


int main(int argc, char* argv[]){
    pm_init();
    srand(time(NULL));
    srand((unsigned)time(NULL));
    signal(SIGABRT, sig_abt);
    uint64_t sz = stoll(argv[1]);
    vector<obj> obj_vec;
    //warm up
    cout<<"executing warm up phase...";
    uint64_t alloc_sz = 0;
    uint64_t obj_sz = 65536;
    while(alloc_sz < sz)
    {
        void* allocate_ptr = pm_malloc(obj_sz);
        obj this_obj;
        this_obj.obj_ptr = allocate_ptr;
        this_obj.size = obj_sz;
        obj_vec.push_back(this_obj);
        alloc_sz += obj_sz;
    }
   // my_allocator->print();
    cout<<"complete"<<endl<<"execute benchmark"<<endl;
    //benchmark
    while(obj_sz <= THRESHOLD)
    {
        //free
        while(alloc_sz > sz/2)
        {
            size_t pos = rand() % obj_vec.size();
           pm_free(obj_vec[pos].obj_ptr);
            alloc_sz -= obj_vec[pos].size;
            obj_vec.erase(obj_vec.begin() + pos);
        }
       // my_allocator->print();
        cout<<"free complete"<<endl;
        //alloc again
        obj_sz = obj_sz * 2;
        while(alloc_sz < sz)
        {   
            void* allocate_ptr = pm_malloc(obj_sz);
            obj this_obj;
            this_obj.obj_ptr = allocate_ptr;
            this_obj.size = obj_sz;
            obj_vec.push_back(this_obj);
            alloc_sz += obj_sz;
        }
      //  my_allocator->print();
        cout<<"alloc complete"<<endl;   
    }
    return 0;
}

//nvm_malloc 15g->24G->32G->40G->50G->65G