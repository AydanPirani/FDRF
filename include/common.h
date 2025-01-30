#ifndef DRF_COMMON_H
#define DRF_COMMON_H

#include <utility>

namespace DRF
{
    class DRFImplementation;
    
    constexpr int BUCKET_COUNT = 10;    
    float compare(DRFImplementation& impl1, DRFImplementation& impl2);
    void test(DRFImplementation& impl1, DRFImplementation& impl2);
}

#endif