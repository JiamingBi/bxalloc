#include "clibpm.h"
#include "makalu.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cassert>
using namespace std;
extern unsigned long long libpm_min;
extern unsigned long long libpm_max;
namespace storage{


extern char *base_addr;
extern char *curr_addr;

static void *__map_persistent_region(string filename){
      int fd; 
      fd  = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR);

      off_t offt = lseek(fd, REGION_SIZE-1, SEEK_SET);
      assert(offt != -1);

      int result = write(fd, "", 1); 
      assert(result != -1);

      void * addr =
          mmap(0, REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      assert(addr != MAP_FAILED);

      *((intptr_t*)addr) = (intptr_t) addr;
      base_addr = (char*) addr;
      //adress to remap to, the root pointer to gc metadata, 
      //and the curr pointer at the end of the day
      curr_addr = (char*) ((size_t)addr + 3 * sizeof(intptr_t));
      printf("Addr: %p\n", addr);
      printf("Base_addr: %p\n", base_addr);
      printf("Current_addr: %p\n", curr_addr);
      return (void*)base_addr;
}
static int __nvm_region_allocator(void** memptr, size_t alignment, size_t size)
  {   
      char* next;
      char* res; 
      
      if (((alignment & (~alignment + 1)) != alignment)  ||   //should be multiple of 2
          (alignment < sizeof(void*))) return 1; //should be atleast the size of void*
      size_t aln_adj = (size_t) curr_addr & (alignment - 1);
      
      if (aln_adj != 0)
          curr_addr += (alignment - aln_adj);
      
      res = curr_addr; 
      next = curr_addr + size;
      if (next > base_addr + REGION_SIZE){
          printf("\n----Ran out of space in mmaped file-----\n");
          return 1;
      }
      curr_addr = next;
      *memptr = res;
      //printf("Current NVM Region Addr: %p\n", curr_addr);
      
      return 0;
  }

void *makalu_alloc::pmemalloc_init_helper(const char *path, size_t size){
	string filename(path);
	void *ret=__map_persistent_region(filename);
    MAK_start(&__nvm_region_allocator);
    return ret;
}

void *makalu_alloc::pmemalloc_static_area_helper(){
    return NULL;
}

void *makalu_alloc::pmemalloc_reserve_helper(size_t size){
    void *ret=MAK_malloc(size);
//    printf("min=%lld max=%lld p=%lld\n",libpm_min,libpm_max,(unsigned long long)ret);
//	if(libpm_min<=(unsigned long long)ret&&(unsigned long long)ret<=libpm_max) printf("true\n");
//	else printf("false\n");
	return ret;
}

void makalu_alloc::pmemalloc_free_helper(void *abs_ptr_){
//    if(libpm_min<=(unsigned long long)abs_ptr_&&(unsigned long long)abs_ptr_<=libpm_max) printf("true\n");
//	else printf("false\n");
	MAK_free(abs_ptr_);
}


void makalu_alloc::pmemalloc_check_helper(const char *path){
     return ;
}
void makalu_alloc::pmemalloc_activate_helper(void *abs_ptr_){
  return ;
}

void makalu_alloc::pmemalloc_close_helper(){
    MAK_close(); 
}
}