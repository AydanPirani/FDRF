#include <queue>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <format>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <random>
#include <bitset>

#include "common.h"
#include "resource.h"


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

class ExactQueue : public DRFImplementation {
    std::unordered_map<ID_T, Resource> *requestMap;
    std::unordered_map<ID_T, Resource> *assignedResources;
    std::priority_queue<SHARE_ENTRY> *queue;

    Resource _currentResources;
    Resource _initialResources;
    bool _isRunning;

    public:
        std::unordered_map<ID_T, Resource>& getShares() {
            return *assignedResources;
        }
        ExactQueue() { }

        void init(Resource resource) {
            _initialResources = resource;
            _currentResources = resource;

            queue = new std::priority_queue<SHARE_ENTRY>();
            requestMap = new std::unordered_map<ID_T, Resource>();
            assignedResources = new std::unordered_map<ID_T, Resource>();
            _isRunning = true;
        }

        ~ExactQueue(){
            delete queue;
            delete requestMap;
            delete assignedResources;
        }

        ID_T processNext() {
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

        void registerProcess(ID_T id, Resource& request) {
            requestMap->emplace(id, request);
            assignedResources->emplace(id, Resource());
            queue->push(std::make_pair<>(-0.0, id));
            return;
        }

        bool isRunning() {
            return _isRunning;
        }

        void displayShares() {
            for (auto& [id, resource]: *assignedResources) {
                std::cout << "id: " << id << ", resource: " << resource.toString() << std::endl;
            }
        }

        friend float compare(DRFImplementation& impl1, DRFImplementation& impl2);
};

struct ListNode {
    ListNode* next=nullptr;
    ListNode* prev=nullptr;
    std::unordered_set<ID_T>* nodes;
};

const int BUCKET_COUNT = 10;

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
        std::unordered_map<ID_T, Resource>& getShares() {
            return *assignedResources;
        }

        FuzzyQueue() {
        }
        
        void init(Resource init) {
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
        void registerProcess(ID_T id, Resource& request) {
            requestMap->emplace(id, request);
            assignedResources->emplace(id, Resource());
            processShares->emplace(id, 0);

            nodePointers[0]->nodes->insert(id);
            incrementPtr = headPtr;
            nodeCt++;
        }

        ID_T processNext() {
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

        bool isRunning() {
            return isValid && nodeCt != 0;
        }

        void displayShares() {
            for (auto& [id, resource]: *assignedResources) {
                std::cout << "id: " << id << ", resource: " << resource.toString() << std::endl;
            }
        }
        friend float compare(DRFImplementation& impl1, DRFImplementation& impl2);
};

float compare(DRFImplementation& impl1, DRFImplementation& impl2) {
    auto& shares1 = impl1.getShares();
    auto& shares2 = impl2.getShares();

    std::vector<RESOURCE_T> impl1_cpu;
    std::vector<RESOURCE_T> impl2_cpu;
    std::vector<RESOURCE_T> impl1_ram;
    std::vector<RESOURCE_T> impl2_ram;

    RESOURCE_T min_cpu1 = std::numeric_limits<RESOURCE_T>::max();
    RESOURCE_T max_cpu1 = std::numeric_limits<RESOURCE_T>::lowest();
    RESOURCE_T min_ram1 = std::numeric_limits<RESOURCE_T>::max();
    RESOURCE_T max_ram1 = std::numeric_limits<RESOURCE_T>::lowest();

    RESOURCE_T min_cpu2 = std::numeric_limits<RESOURCE_T>::max();
    RESOURCE_T max_cpu2 = std::numeric_limits<RESOURCE_T>::lowest();
    RESOURCE_T min_ram2 = std::numeric_limits<RESOURCE_T>::max();
    RESOURCE_T max_ram2 = std::numeric_limits<RESOURCE_T>::lowest();


    for (auto& [id, r] : shares1) {
        Resource resource1 = shares1.at(id);
        Resource resource2 = shares2.at(id);
        impl1_cpu.push_back(resource1._cpu);
        impl1_ram.push_back(resource1._ram);
        impl2_cpu.push_back(resource2._cpu);
        impl2_ram.push_back(resource2._ram);

        min_cpu1 = std::min(min_cpu1, resource1._cpu);
        max_cpu1 = std::max(max_cpu1, resource1._cpu);
        min_ram1 = std::min(min_ram1, resource1._ram);
        max_ram1 = std::max(max_ram1, resource1._ram);

        min_cpu2 = std::min(min_cpu2, resource2._cpu);
        max_cpu2 = std::max(max_cpu2, resource2._cpu);
        min_ram2 = std::min(min_ram2, resource2._ram);
        max_ram2 = std::max(max_ram2, resource2._ram);

    }
    
    float cpu1_range = max_cpu1 - min_cpu1;
    float ram1_range = max_ram1 - min_ram1;

    float cpu2_range = max_cpu2 - min_cpu2;
    float ram2_range = max_ram2 - min_ram2;

    float diff = 0;
    for (size_t i = 0; i < impl1_cpu.size(); i++) {

        float cpu_delta = abs((impl1_cpu[i]-min_cpu1)/cpu1_range - (impl2_cpu[i]-min_cpu2)/cpu2_range);
        float ram_delta = abs((impl1_ram[i]-min_ram1)/ram1_range - (impl2_ram[i]-min_ram2)/cpu2_range);
        
        diff += cpu_delta*cpu_delta + ram_delta*ram_delta;
    }

    return diff/(2*impl1_cpu.size());

}

void test(DRFImplementation& impl1, DRFImplementation& impl2) {
    Resource initialResources(9LL*1024LL*1024LL, 18LL*1024LL*1024LL);
    const int numProcesses = 2000;

    impl1.init(initialResources);
    impl2.init(initialResources);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> cpuDist(1, 100);
    std::uniform_int_distribution<> memDist(1, 200);


    // Register processes
    for(int i = 1; i <= numProcesses; ++i){
        int cpu = cpuDist(gen);
        int memory = memDist(gen);

        Resource res1(cpu, memory);
        Resource res2(cpu, memory);

        impl1.registerProcess(i, res1);
        impl2.registerProcess(i, res2);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    
    long long ctr1 = 0;
    while(impl1.isRunning()) {
        impl1.processNext();
        ctr1++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "1 Time taken: " << elapsed.count() << " seconds." << std::endl;
    std::cout << "1 Iterations: " << ctr1 << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    
    long long ctr2 = 0;
    while(impl2.isRunning()) {
        impl2.processNext();
        ctr2++;
    }

    end_time = std::chrono::high_resolution_clock::now();

    elapsed = end_time - start_time;
    std::cout << "2 Time taken: " << elapsed.count() << " seconds." << std::endl;
    std::cout << "2 Iterations: " << ctr2 << std::endl;
    std::cout << "Overall closeness: " << compare(impl1, impl2) << std::endl;
}

int main() {
    Resource initialResources(9, 18);
    Resource aResource(1, 4);
    Resource bResource(3, 1);
    
    ExactQueue eq;
    FuzzyQueue fq;

    test(eq, fq);

    // eq.init(initialResources);
    // eq.registerProcess(1, aResource);
    // eq.registerProcess(2, bResource);

    // while (eq.isRunning()) {
    //     eq.processNext();
    // }
    // eq.displayShares();

    // aResource = Resource(1, 4);
    // bResource = Resource(3, 1);

    // fq.init(initialResources);
    // fq.registerProcess(1, aResource);
    // fq.registerProcess(2, bResource);

    // while (fq.isRunning()) {
    //     fq.processNext();
    // }
    // fq.displayShares();
    // std::cout << compare(eq, fq) << std::endl;

    return 0;
}