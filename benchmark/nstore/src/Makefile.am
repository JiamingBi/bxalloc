## Makefile.am -- Process this file with automake to produce Makefile.in

bin_PROGRAMS = nstore pmem_check

AM_CPPFLAGS = -I$(srcdir)/common -I../allocator/makalu_alloc/include -I../../../src -Wno-pointer-arith -I../allocator/nvm_alloc/src 
AM_CXXFLAGS = -Wall -Wextra  -std=c++17  -Wno-unused-parameter -O3 -g -fpermissive -fno-omit-frame-pointer -fPIC -fno-sized-deallocation
AM_LDFLAGS  = $(PTHREAD_CFLAGS)
LIBS = -lrt  -pthread -L../allocator/makalu_alloc/lib -lmakalu -L../../../ -lbxalloc -L../allocator/nvm_alloc -lnvmmalloc -Wl,-rpath=../allocator/nvm_alloc -Wl,-rpath=../allocator/makalu_alloc/lib -Wl,-rpath=../../../ -ljemalloc -lstdc++ -latomic -lpmemobj 

noinst_LIBRARIES = libpm.a
libpm_a_SOURCES = libpm.cpp utils.cpp makalu_alloc.cpp bx_alloc.cpp pm_alloc.cpp pmdk_alloc.cpp nvm_alloc.cpp

#AM_CPPFLAGS = $(BOOST_CPPFLAGS) 
#AM_LDFLAGS = $(BOOST_SYSTEM_LDFLAGS) $(BOOST_THREAD_LDFLAGS) $(PTHREAD_CFLAGS)
#LIBS = $(BOOST_SYSTEM_LIBS) $(BOOST_THREAD_LIBS) 

nstore_SOURCES  =  	main.cpp \
					wal_engine.cpp  \
					sp_engine.cpp  \
					lsm_engine.cpp  \
					opt_wal_engine.cpp  \
					opt_sp_engine.cpp  \
					opt_lsm_engine.cpp  \
	   				test_benchmark.cpp \
	   				ycsb_benchmark.cpp \
					tpcc_benchmark.cpp \
					utils.cpp\
					makalu_alloc.cpp\
					bx_alloc.cpp\
					pm_alloc.cpp\
					nvm_alloc.cpp

nstore_LDADD  = libpm.a

pmem_check_SOURCES = pmem_check.cpp

pmem_check_LDADD  = libpm.a


