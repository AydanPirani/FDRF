#include "fuzzy_queue.h"

#include <iostream>

namespace DRF {
    std::unordered_map<ID_T, Resource>& FuzzyQueue::getShares() {
        return *assignedResources;
    }

    FuzzyQueue::FuzzyQueue() {}

    void FuzzyQueue::init(Resource init) {
        initialResources = init;
        currentResources = init;

        requestMap = new std::unordered_map<ID_T, Resource>();
        assignedResources = new std::unordered_map<ID_T, Resource>();
        processShares = new std::unordered_map<ID_T, float>();
        nodeCt = 0;

        // Initialize buckets
        for (int i = 0; i < BUCKET_COUNT; i++) {
            ListNode* node = new ListNode();
            if (i == 0) {
                headPtr = node;
                incrementPtr = node;
            } else {
                node->prev = nodePointers[i-1];
                node->prev->next = node;
                node->next = nullptr;
            }
            node->nodes = new std::unordered_set<ID_T>();
            nodePointers[i] = node;
        }

    }
    void FuzzyQueue::registerProcess(ID_T id, Resource& request) {
        requestMap->emplace(id, request);
        assignedResources->emplace(id, Resource());
        processShares->emplace(id, 0);

        nodePointers[0]->nodes->insert(id);
        incrementPtr = headPtr;
        nodeCt++;
    }

    ID_T FuzzyQueue::processNext() {
        while (incrementPtr != nullptr && incrementPtr->nodes->empty()) {
            incrementPtr = incrementPtr->next;
        }

        if (incrementPtr == nullptr) {
            isValid = false;
            return -1;
        }

        auto& nodes = *(incrementPtr->nodes);
        ID_T currId;
        for (auto id: nodes) {
            currId = id;
            break;
        }
        nodes.erase(currId);

        Resource nodeRequest = requestMap->at(currId);
        float currShare = processShares->at(currId);
        
        if (currentResources < nodeRequest) {
            nodeCt--;
            return currId;
        }

        try {
            currShare += nodeRequest.getDominantShare(initialResources);
            currentResources -= nodeRequest;
            assignedResources->at(currId) += nodeRequest;
            processShares->at(currId) = currShare;
            int newIdx = static_cast<int>(currShare * 10);
            nodePointers[newIdx]->nodes->insert(currId);
        } catch (const std::exception& e) {
            // std::cout << e << std::endl;
        }
        return currId;
    }

    bool FuzzyQueue::isRunning() {
        return isValid && nodeCt != 0;
    }

    void FuzzyQueue::displayShares() {
        for (auto& [id, resource]: *assignedResources) {
            std::cout << "id: " << id << ", resource: " << resource.toString() << std::endl;
        }
    }
}