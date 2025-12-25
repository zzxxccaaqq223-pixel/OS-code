#include <windows.h>
#include <iostream>
DWORD WINAPI MyThreadFunc(LPVOID lpParam) {
int id = *((int*)lpParam);
for (int i = 0; i < 5; i++) {
std::cout << "Thread " << id << " running...\n";
Sleep(500); // 0.5 sec
}
return 0;
}
int main() {
DWORD threadID;
int param = 1;
HANDLE hThread = CreateThread(NULL, 0, MyThreadFunc, &param, 0, &threadID);
if (hThread == NULL) {
std::cout << "Failed to create thread.\n";
return 1;
}
WaitForSingleObject(hThread, INFINITE);
CloseHandle(hThread);
std::cout << "Main thread finished.\n";
return 0;
}