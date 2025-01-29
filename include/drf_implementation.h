#ifndef DRF_IMPLEMENTATION_H
#define DRF_IMPLEMENTATION_H

#include "common.h"
#include "resource.h"
#include <unordered_map>

namespace DRF {
    class DRFImplementation {
        public:
            virtual void init(Resource resource) = 0;
            virtual ID_T processNext() = 0;
            virtual void registerProcess(ID_T id, Resource& resources) = 0;
            virtual ~DRFImplementation() = default;
            virtual bool isRunning() = 0;
            virtual void displayShares() = 0;
            virtual std::unordered_map<ID_T, Resource>& getShares() = 0;
    };
}

#endif