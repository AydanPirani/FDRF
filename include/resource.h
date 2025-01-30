#ifndef DRF_RESOURCE_H
#define DRF_RESOURCE_H 

#include <string>
#include "common.h"
#include "types.h"

namespace DRF {
    struct Resource {
        RESOURCE_T _cpu;
        RESOURCE_T _ram;

        Resource();

        Resource(RESOURCE_T cpu, RESOURCE_T ram);

        bool operator==(const Resource& other) const;

        bool operator<(const Resource& other) const;

        Resource& operator-=(const Resource& other);

        Resource& operator+=(const Resource& other);

        Resource& operator=(const Resource& other);

        float getDominantShare(const Resource& other);

        float getDiff(const Resource& other);

        std::string toString();

    };

    Resource operator-(Resource lhs, const Resource& rhs);
}

#endif