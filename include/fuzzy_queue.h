#ifndef DRF_FUZZY_H
#define DRF_FUZZY_H

#include "common.h"
#include "drf_implementation.h"

#include <unordered_set>

namespace DRF {
    struct ListNode {
        ListNode* next=nullptr;
        ListNode* prev=nullptr;
        std::unordered_set<ID_T>* nodes;
    };

    class FuzzyQueue: public DRFImplementation {
        ListNode* nodePointers[BUCKET_COUNT];
        ListNode* headPtr = nullptr;
        ListNode* incrementPtr = nullptr;

        std::unordered_map<ID_T, Resource> *requestMap;
        std::unordered_map<ID_T, Resource> *assignedResources;
        std::unordered_map<ID_T, float> *processShares;


        Resource currentResources;
        Resource initialResources;
        int nodeCt = 0;
        bool isValid = true;


        public:
            std::unordered_map<ID_T, Resource>& getShares();
            FuzzyQueue();
            void init(Resource init);
            void registerProcess(ID_T id, Resource& request);
            ID_T processNext();
            bool isRunning();
            void displayShares();
            friend float compare(DRFImplementation& impl1, DRFImplementation& impl2);
    };
}

#endif