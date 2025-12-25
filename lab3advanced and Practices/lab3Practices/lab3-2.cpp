#include <iostream>
#include <thread>
#include <chrono>
void worker(int id) {
for (int i = 0; i < 5; i++) {
std::cout << "Worker " << id << " running...\n";
std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
}
int main() {

std::thread t1(worker, 1);
std::thread t2(worker, 2);
t1.join();
t2.join();
std::cout << "All threads finished.\n";
return 0;
}