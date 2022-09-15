#include <iostream>
#include <thread>
#include <atomic>
#include "bxalloc.hpp"
using namespace std;


void t(void *arg)
{
    int *tmp=(int*)arg;
    for (int i = 0; i < 20; ++i)
    {
        int *a=(int*)BXP_malloc(132*1024);
        *a=i;
        cout<<*tmp<<":  "<<*a<<endl;
       // BXP_free(a);
    }
}
void t2()
{
    for (int i = 0; i < 20; ++i)
    {
        int *a=(int*)BXP_malloc(sizeof(int));
        *a=i;
        cout<<"2:  "<<*a<<endl;
        BXP_free(a);
    }
}
int main()
{
    BXP_init("bxalloc",16*1024*1024*1024ULL);
   /* for(int i=0;i<4;i++){
        void *a=BXP_malloc(512*1024*1024-16);
        BXP_free(a);
    }*/
    //int *a=(int*)BXP_malloc(132*1024);
    //BXP_free(a);
    int o=1;
    int p=2;
    int q=3;
    thread th1(t,&o);
    thread th2(t,&p);
   
    th1.join(); //等待th1执行完
    th2.join(); //等待th2执行完
    thread th3(t,&q);
    th3.join();
    cout << "here is main\n\n";
   // BXP_close();
   /*_ArrayQueue<int,11> p;
   for(int i=0;i<10;i++)
   p.push(i);
   for(int i=0;i<10;i++)
   {
       auto j=p.pop();
       cout<<j.value()<<endl;
   }
   cout<<"结束"<<endl;  */
    return 0;
}