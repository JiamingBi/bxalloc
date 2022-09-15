#pragma once

#include <vector>
#include <thread>
#include <map>

#include "config.h"
#include "engine.h"
#include "timer.h"
#include "utils.h"
#include "database.h"
#include "libpm.h"
#include "makalu.h"

#include "test_benchmark.h"
#include "ycsb_benchmark.h"
#include "tpcc_benchmark.h"

namespace storage {
void * load_bh2(void *arg){
    benchmark *bh=(benchmark*)arg;
    bh->load();
    return NULL;
}

void  * execute_bh2(void *arg) {
    // Execute
    benchmark *bh=(benchmark*)arg;
    bh->execute();
    return NULL;
}
class coordinator {
 public:
  coordinator()
      : single(true),
        num_executors(1),
        num_txns(0) {
  }

  coordinator(const config conf) {
    single = conf.single;
    num_executors = conf.num_executors;
    num_txns = conf.num_txns;
//    printf("%d\n",mode);
    for (unsigned int i = 0; i < num_executors; i++) {
      tms.push_back(timer()); //volatile
      sps.push_back(static_info()); // volatile
    }
  }

  void execute_bh(benchmark* bh) {
    // Execute
    bh->execute();
  }

  void load_bh(benchmark* bh) {
    // Load
    bh->load();
  }


  void eval(const config conf) {
    if (!conf.recovery) {
      execute(conf);
    } else {
      recover(conf);
    }

  }

  void execute(const config conf) {
  //  std::vector<std::thread> executors;
  //  std::vector<std::thread> loaders;
     pthread_t executors[num_executors];
     pthread_t loaders[num_executors];
    benchmark** partitions = new benchmark*[num_executors]; // volatile

    for (unsigned int i = 0; i < num_executors; i++) {
      database* db = new database(conf, sp, i); // volatile
      partitions[i] = get_benchmark(conf, i, db);
    }

    assert (mtm_enable_trace == 0);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    std::cerr << "LOADING..." << std::endl;
    int mode=conf.choose;
    for (unsigned int i = 0; i < num_executors; i++)
    //  loaders.push_back(
    //      std::thread(&coordinator::load_bh, this, partitions[i]));
    {
      
       if(mode==3)  MAK_pthread_create(&loaders[i],&attr,&load_bh2,(void*)partitions[i]);
       else pthread_create(&loaders[i],&attr,&load_bh2,(void*)partitions[i]);
    }
    for (unsigned int i = 0; i < num_executors; i++)
    //  loaders[i].join();
    {
       if(mode==3)  MAK_pthread_join(loaders[i],NULL);
       else         pthread_join(loaders[i],NULL);  
    }
    if(conf.is_trace_enabled) {
	    int ret = write(tracing_on, "1", 1);
        if(ret == 1) {
            printf("Tracing Enabled\n");
        }
	    mtm_enable_trace = conf.is_trace_enabled;
    }
    std::cerr << "EXECUTING..." << std::endl;

    for (unsigned int i = 0; i < num_executors; i++)
      //std::cout<<i<<"\n";
    //  executors.push_back(
    //      std::thread(&coordinator::execute_bh, this, partitions[i]));
    {
       if(mode==3)  MAK_pthread_create(&executors[i],&attr,&execute_bh2,(void*)partitions[i]);
       else pthread_create(&executors[i],&attr,&execute_bh2,(void*)partitions[i]);
    }
    for (unsigned int i = 0; i < num_executors; i++)
    //  executors[i].join();
    {
       if(mode==3)  MAK_pthread_join(executors[i],NULL);
       else         pthread_join(executors[i],NULL); 
    }
    double max_dur = 0;
    for (unsigned int i = 0; i < num_executors; i++) {
      std::cerr << "dur :" << i << " :: " << tms[i].duration() << std::endl;
      max_dur = std::max(max_dur, tms[i].duration());
    }
    std::cerr << "max dur :" << max_dur << std::endl;
    display_stats(conf.etype, max_dur, num_txns);

  }

  void recover(const config conf) {

    database* db = new database(conf, sp, 0);
    benchmark* bh = get_benchmark(conf, 0, db);

    // Load
    bh->load();

    // Crash and recover
    bh->sim_crash();

  }

  benchmark* get_benchmark(const config state, unsigned int tid, database* db) {
    benchmark* bh = NULL;

    // Fix benchmark
    switch (state.btype) {
     case benchmark_type::TEST:
	//die();
      LOG_INFO("TEST");
      bh = new test_benchmark(state, tid, db, &tms[tid], &sps[tid]); // volatile
      break;

     case benchmark_type::YCSB:
        LOG_INFO("YCSB");
        bh = new ycsb_benchmark(state, tid, db, &tms[tid], &sps[tid]); // volatile
        break;

      case benchmark_type::TPCC:
        LOG_INFO("TPCC");
        bh = new tpcc_benchmark(state, tid, db, &tms[tid], &sps[tid]); // volatile
        break;

      default:
        std::cerr << "Unknown benchmark type :: " << state.btype << std::endl;
        break;
    }

    return bh;
  }

  bool single;
  unsigned int num_executors;
  unsigned int num_txns;

  std::vector<struct static_info> sps;
  std::vector<timer> tms;
};

}
