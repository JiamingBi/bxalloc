#include "AllocatorMacro.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <pptr.hpp>
#include <sys/mman.h>
#include <ctime>
using namespace std;
#define OBJNUM 1000000
#define FILESIZE1 34359746560


int alloc_count = 0;
int objsize;
size_t total_size = 0;
vector<double> my_vec;
/*struct args{
    int start_pos;
    int end_pos;
    int objSize;
    char **address_ptr;
};*/

//RALLOC and MAKALU trigger SIGABT
void sig_abt(int count)
{
    cout<<"failed"<<count<<"count is"<<alloc_count<<endl;
    alloc_count++;
    pm_output(0, total_size, my_vec);
    //pm_output(1, total_size, my_vec);
    exit(1);
}


int main(int argc, char* argv[]){
    pm_init();
    objsize = stoi(string(argv[1]));
    char **objs = new char * [OBJNUM];
    
    signal(SIGABRT, sig_abt);
    
    my_vec.push_back((double)objsize);
    int iter_count = OBJNUM;
    if(objsize > 10000)
        iter_count = 10000;
    for(int i=0; i<iter_count; i++)
    {
        //if(i%2==0)
        //{
            //objs[i] = (char*)  my_allocator->allocate(objsize*10);
            //total_size += objsize*10;
        //}
            
        //else
        //{
            //objsize=rand()%1000; 
            objs[i] = (char*) pm_malloc(objsize);
            total_size += objsize;
        //}
        
        /*
        {
            //my_allocator->output(0, i, objsize);
            my_allocator->output(0, i, objsize);

        }
        if(i==9437120)
        {
            objs[i] = (char*) my_allocator->allocate(objsize);
        }
        //cout<<i<<endl;
        objs[i] = (char*) my_allocator->allocate(objsize);
        
        //PMDK only return null ptr
        if(objs[i]==NULL)
        {
            cout<<"filed at"<<i<<endl;
            exit(1);
        }
        alloc_count++;*/

    }
   // size_t total_alloc = my_allocator->get_alloc_size();
    pm_output(0, total_size, my_vec);
    //pm_output(1, total_size, my_vec);

    return 0;
}