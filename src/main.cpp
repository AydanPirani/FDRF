#include "common.h"
#include "resource.h"
#include "drf_implementation.h"
#include "exact_queue.h"
#include "fuzzy_queue.h"

using namespace DRF;

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