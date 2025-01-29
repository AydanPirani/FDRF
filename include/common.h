#ifndef DRF_COMMON_H
#define DRF_COMMON_H

#include "drf_implementation.h"
#include <utility>

namespace DRF
{
    using ID_T = size_t;
    using ID_ENTRY = std::pair<ID_T, Resource>;
    using SHARE_ENTRY = std::pair<float, ID_T>;
    using RESOURCE_T = long long;

    constexpr int BUCKET_COUNT = 10;    
}

#endif