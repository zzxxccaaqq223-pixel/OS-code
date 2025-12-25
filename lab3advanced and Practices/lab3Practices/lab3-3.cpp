#include <iostream>
#include <thread>
#include <mutex>
std::mutex mtx;
int counter = 0;
void increment(int id) {
for (int i = 0; i < 5; i++) {
mtx.lock();
counter++;
std::cout << "Thread " << id << " incremented counter to " << counter << "\n";
mtx.unlock();
std::this_thread::sleep_for(std::chrono::milliseconds(300));
}
}
int main() {
std::thread t1(increment, 1);
std::thread t2(increment, 2);
t1.join();
t2.join();
std::cout << "Final counter = " << counter << "\n";
return 0;
}