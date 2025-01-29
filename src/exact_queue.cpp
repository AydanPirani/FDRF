#include "exact_queue.h"
#include <iostream>

namespace DRF {
    std::unordered_map<ID_T, Resource>& ExactQueue::getShares() {
        return *assignedResources;
    }

    ExactQueue::ExactQueue() { }

    void ExactQueue::init(Resource resource) {
        _initialResources = resource;
        _currentResources = resource;

        queue = new std::priority_queue<SHARE_ENTRY>();
        requestMap = new std::unordered_map<ID_T, Resource>();
        assignedResources = new std::unordered_map<ID_T, Resource>();
        _isRunning = true;
    }

    ExactQueue::~ExactQueue(){
        delete queue;
        delete requestMap;
        delete assignedResources;
    }

    ID_T ExactQueue::processNext() {
        if (queue->size() == 0) {
            _isRunning = false;
            return -1;
        }

        auto currEntry = queue->top();
        float currShare = -currEntry.first;
        int currId = currEntry.second;
        queue->pop();

        Resource nodeRequest = requestMap->at(currId);
        
        if (_currentResources < nodeRequest) {
            return currId;
        }
        
        _currentResources -= nodeRequest;
        assignedResources->at(currId) += nodeRequest;

        currShare += nodeRequest.getDominantShare(_initialResources);
        queue->push(std::make_pair<>(-currShare, currId));
        return currId;
    }

    void ExactQueue::registerProcess(ID_T id, Resource& request) {
        requestMap->emplace(id, request);
        assignedResources->emplace(id, Resource());
        queue->push(std::make_pair<>(-0.0, id));
        return;
    }

    bool ExactQueue::isRunning() {
        return _isRunning;
    }

    void ExactQueue::displayShares() {
        for (auto& [id, resource]: *assignedResources) {
            std::cout << "id: " << id << ", resource: " << resource.toString() << std::endl;
        }
}
}