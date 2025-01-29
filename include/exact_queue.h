#ifndef DRF_EXACT_H
#define DRF_EXACT_H

#include "resource.h"
#include "drf_implementation.h"

#include <queue>

namespace DRF {
    class ExactQueue : public DRFImplementation {
        std::unordered_map<ID_T, Resource> *requestMap;
        std::unordered_map<ID_T, Resource> *assignedResources;
        std::priority_queue<SHARE_ENTRY> *queue;

        Resource _currentResources;
        Resource _initialResources;
        bool _isRunning;

        public:
            std::unordered_map<ID_T, Resource>& getShares();
            ExactQueue();
            void init(Resource resource);
            ~ExactQueue();
            ID_T processNext();
            void registerProcess(ID_T id, Resource& request);
            bool isRunning();
            void displayShares();

            friend float compare(DRFImplementation& impl1, DRFImplementation& impl2);
    };
}

#endif