#include "common.h"
#include "resource.h"
#include "drf_implementation.h"
#include <vector>

#include <chrono>
#include <random>
#include <iostream>

namespace DRF {
    float compare(DRFImplementation& impl1, DRFImplementation& impl2) {
        auto& shares1 = impl1.getShares();
        auto& shares2 = impl2.getShares();

        std::vector<RESOURCE_T> impl1_cpu;
        std::vector<RESOURCE_T> impl2_cpu;
        std::vector<RESOURCE_T> impl1_ram;
        std::vector<RESOURCE_T> impl2_ram;

        RESOURCE_T min_cpu = std::numeric_limits<RESOURCE_T>::max();
        RESOURCE_T max_cpu = std::numeric_limits<RESOURCE_T>::lowest();
        RESOURCE_T min_ram = std::numeric_limits<RESOURCE_T>::max();
        RESOURCE_T max_ram = std::numeric_limits<RESOURCE_T>::lowest();

        for (auto& [id, r] : shares1) {
            Resource resource1 = shares1.at(id);
            Resource resource2 = shares2.at(id);
            impl1_cpu.push_back(resource1._cpu);
            impl1_ram.push_back(resource1._ram);
            impl2_cpu.push_back(resource2._cpu);
            impl2_ram.push_back(resource2._ram);

            min_cpu = std::min(min_cpu, resource1._cpu);
            max_cpu = std::max(max_cpu, resource1._cpu);
            min_ram = std::min(min_ram, resource1._ram);
            max_ram = std::max(max_ram, resource1._ram);

            min_cpu = std::min(min_cpu, resource2._cpu);
            max_cpu = std::max(max_cpu, resource2._cpu);
            min_ram = std::min(min_ram, resource2._ram);
            max_ram = std::max(max_ram, resource2._ram);
        }
        
        float cpu_range = max_cpu - min_cpu;
        float ram_range = max_ram - min_ram;

        float diff = 0;
        for (size_t i = 0; i < impl1_cpu.size(); i++) {

            float cpu_delta = abs(impl1_cpu[i] - impl2_cpu[i])/cpu_range;
            float ram_delta = abs(impl1_ram[i] - impl2_ram[i])/ram_range;
            
            diff += cpu_delta*cpu_delta + ram_delta*ram_delta;
        }

        return 100 * diff/(2*impl1_cpu.size());
    }

    void test(DRFImplementation& impl1, DRFImplementation& impl2) {
        Resource initialResources(9LL*1024LL*1024LL*12LL, 18LL*1024LL*1024LL*12LL);
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
}
