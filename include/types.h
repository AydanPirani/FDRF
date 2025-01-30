#ifndef DRF_TYPES_H
#define DRF_TYPES_H

#include <utility>

namespace DRF {
    struct Resource;
}

using ID_T = size_t;
using ID_ENTRY = std::pair<ID_T, DRF::Resource>;
using SHARE_ENTRY = std::pair<float, ID_T>;
using RESOURCE_T = long long;

#endif