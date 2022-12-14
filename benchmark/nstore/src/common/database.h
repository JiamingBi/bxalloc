#pragma once

#include "config.h"
#include "table.h"
#include "plist.h"
#include "cow_pbtree.h"
#include <set>

namespace storage {

class database {
 public:
  database(config conf, struct static_info* sp, unsigned int tid)
      : tables(NULL),
        log(NULL),
        dirs(NULL) {

    PM_EQU((sp->itr), (sp->itr + 1));

    // TABLES
    plist<table*>* _tables = new ((plist<table*>*) pmalloc(sizeof(plist<table*>))) plist<table*>(&sp->ptrs[get_next_pp()],
                                               					&sp->ptrs[get_next_pp()]);
    pmemalloc_activate(_tables);
    tables = _tables;

    // LOG
    log = new ((plist<char*>*) pmalloc(sizeof(plist<char*>))) plist<char*>(&sp->ptrs[get_next_pp()], &sp->ptrs[get_next_pp()]);
    pmemalloc_activate(log);

    // DIRS
    if (conf.etype == engine_type::SP) {
//	die();	
      dirs = new  cow_pbtree(
          false, (conf.fs_path + std::to_string(tid) + "_" + "cow.nvm").c_str(),
          NULL);
      // No activation
    }

    if (conf.etype == engine_type::OPT_SP) {
//	die();	
      dirs = new ((cow_btree*)pmalloc(sizeof(cow_btree))) cow_pbtree(true, NULL, &sp->ptrs[sp->itr++]);
      pmemalloc_activate(dirs);
    }
  }

  ~database() {
    // clean up tables
    std::vector<table*> table_vec = tables->get_data();
    for (table* table : table_vec)
      delete table;

    delete tables;
    delete[] log;
  }

  void reset(config& conf, unsigned int tid) {

    if (conf.etype == engine_type::SP) {
	die();	
      dirs = new cow_pbtree(
          false, (conf.fs_path + std::to_string(tid) + "_" + "cow.nvm").c_str(),
          NULL);
    }

    // Clear all table data and indices
    if (conf.etype == engine_type::WAL || conf.etype == engine_type::LSM) {
      std::vector<table*> tab_vec = tables->get_data();

      for (table* tab : tab_vec) {
        tab->pm_data->clear();
        std::vector<table_index*> indices = tab->indices->get_data();
        for (table_index* index : indices) {
          index->pm_map->clear();
          index->off_map->clear();
        }
      }
    }

  }

  plist<table*>* tables;
  plist<char*>* log;

  // SP and OPT_SP
  cow_pbtree* dirs;
};

}
