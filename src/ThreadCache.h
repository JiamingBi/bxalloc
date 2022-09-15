/*
 * Copyright (C) 2021 University of Hunan . All rights reserved.
 * Licenced under the MIT licence. See LICENSE file in the project root for
 * details. 
 */
#include "FreeList.hpp"
#include "ClassSize.hpp"
#include "pm_config.hpp"
#include "pfence_util.h"
#include "pptr.hpp"
#include "RegionManager.hpp"
class ThreadCache{
    public:
        FreeList _flist[NLIST];
        ThreadCache();
        ~ThreadCache();
};

namespace bxalloc
{
    extern thread_local ThreadCache tsl_cache;
};// namespace bxalloc
