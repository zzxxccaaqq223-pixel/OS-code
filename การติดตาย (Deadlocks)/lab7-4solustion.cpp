// Solution: Lock all resources in order A -> B -> C

void process1() {
    resourceA.lock();
    resourceB.lock();
    // Work...
    resourceB.unlock();
    resourceA.unlock();
}

void process2() {
    resourceB.lock();
    resourceC.lock();
    // Work...
    resourceC.unlock();
    resourceB.unlock();
}

void process3() {
    resourceA.lock();  // Changed from resourceC
    resourceC.lock();
    // Work...
    resourceC.unlock();
    resourceA.unlock();
}