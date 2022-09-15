// clibpm

#include "clibpm.h"

std::mutex pmp_mutex;

// throw calls are deprecated in C+11 onwards.
#if __cplusplus >= 201103L
#define NOEXCEPT noexcept
#define THROW_BAD_ALLOC
#else
#define NOEXCEPT throw ()
#define THROW_BAD_ALLOC throw (std::bad_alloc)
#endif

// Global new and delete

unsigned long long libpm_min=LIBPM;
unsigned long long libpm_max=LIBPM+PMSIZE;

void* operator new(size_t sz) THROW_BAD_ALLOC {
    void* ret = calloc(1, sz);
    return ret;
}

void operator delete(void *p, std::size_t sz) NOEXCEPT {
    if (libpm_min <= (unsigned long long) p && (unsigned long long) p <= libpm_max)
	pfree(p);
    else
    	free(p);
    return;
    fprintf(stderr, "%p\n", p+sz);
}

void operator delete[](void *p, std::size_t sz) NOEXCEPT {
	assert(0);
	fprintf(stderr, "%p\n", p+sz);
	return;
}


void operator delete(void *p) NOEXCEPT {
  //printf("%lld %lld %lld\n",(unsigned long long) p,libpm_min,libpm_max);
    if (libpm_min <= (unsigned long long) p && (unsigned long long) p <=libpm_max)
	pfree(p);
    else
    	free(p);
}

void *operator new[](std::size_t sz) THROW_BAD_ALLOC {
    void* ret = calloc(1, sz);
    return ret;
}

void operator delete[](void *p) NOEXCEPT {
    if (libpm_min <= (unsigned long long) p && (unsigned long long) p <= libpm_max)
	pfree(p);
    else
    	free(p);
}

void* pmalloc(size_t sz) {
  pmp_mutex.lock();
  void* ret = storage::pmemalloc_reserve(sz);
  pmp_mutex.unlock();
  return ret;
}

void pfree(void *p) {
  pmp_mutex.lock();
  storage::pmemalloc_free(p);
  pmp_mutex.unlock();
}

namespace storage {

unsigned int get_next_pp() {
  pmp_mutex.lock();
  unsigned int ret = sp->itr;
  PM_EQU((sp->itr), (sp->itr+1));
  pmp_mutex.unlock();
  return ret;
}


struct static_info *sp;
allocator* alloc;
pm_alloc pm1;
bx_alloc pm2;
makalu_alloc pm3;
nvm_alloc pm4;
pmdk_alloc pm5;

char *base_addr = nullptr;
char *curr_addr = nullptr;
int pmem_debug;




void *pmemalloc_init(const char *path, size_t size){
  return alloc->pmemalloc_init_helper(path,size);
}

void *pmemalloc_static_area(){
  return alloc->pmemalloc_static_area_helper();
}
void *pmemalloc_reserve(size_t size){
  return alloc->pmemalloc_reserve_helper(size);
}
void pmemalloc_free(void *abs_ptr_){
  alloc->pmemalloc_free_helper(abs_ptr_);
  return ;
}
void pmemalloc_check(const char *path){
  alloc->pmemalloc_check_helper(path);
  return ;
}
void pmemalloc_activate_helper2(void *abs_ptr_){
  alloc->pmemalloc_activate_helper(abs_ptr_);
  return ;
}
void pmemalloc_close(){
  alloc->pmemalloc_close_helper();
}

// debug -- printf-like debug messages
void debug(const char *file, int line, const char *func, const char *fmt, ...) {
  va_list ap;
  int save_errno;

  save_errno = errno;
  fprintf(stderr, "debug: %s:%d %s()", file, line, func);
  if (fmt) {
    fprintf(stderr, ": ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
  }
  fprintf(stderr, "\n");
  errno = save_errno;
}

// fatal -- printf-like error exits, with and without errno printing
void fatal(int err, const char *file, int line, const char *func,
           const char *fmt, ...) {
  va_list ap;

  fprintf(stderr, "ERROR: %s:%d %s()", file, line, func);
  if (fmt) {
    fprintf(stderr, ": ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
  }
  if (err)
    fprintf(stderr, ": %s", strerror(err));
  fprintf(stderr, "\n");
  exit(1);
}

}
