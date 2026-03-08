// ============================================================
// Chapter 12 — Ex 2: I/O Hardware — Polling vs Interrupt vs DMA
// Compile: g++ -std=c++17 -pthread ex2_io_hardware.cpp -o ex2
// ============================================================
#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
using namespace std;

// ■■ StatusRegister ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct StatusRegister {
    bool busy=false, ready=true, error=false, done=false;
};

// ■■ IOPort ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class IOPort {
public:
    uint8_t data = 0;
    StatusRegister status;
    uint8_t control = 0;

    void writeData(uint8_t v) {
        if (status.busy) { status.error = true; return; }
        status.busy = true;
        status.ready = false;
        data = v;
        this_thread::sleep_for(chrono::microseconds(1));
        status.busy = false; status.ready = true; status.done = true;
    }
    uint8_t readData() { return data; }
    bool pollStatus() { return status.ready; }
};

// ■■ DeviceController ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class DeviceController {
    IOPort port;
    queue<function<void()>> irqQueue;
    mutex mtx;
public:
    // POLLING: CPU busy-waits for each byte
    long pollingIO(uint8_t d) {
        long cycles = 0;
        while (!port.pollStatus()) { ++cycles; }
        port.writeData(d);
        cycles += 2;
        return cycles;
    }

    // INTERRUPT-DRIVEN: CPU programs device, registers callback, is freed
    long interruptDrivenIO(uint8_t d, function<void()> cb) {
        long cycles = 1;
        port.writeData(d);
        { lock_guard<mutex> lk(mtx); irqQueue.push(cb); }
        processIRQ();
        cycles += 1;
        return cycles;
    }

    // DMA: CPU sets up once, DMA engine transfers entire buffer autonomously
    long dmaTransfer(vector<uint8_t>& dst, const vector<uint8_t>& src) {
        long cycles = 1;
        cout << "  [DMA] CPU programs DMA: src=buffer, dst=RAM, count=" << src.size() << "\n";
        cout << "  [DMA] CPU is FREE during transfer...\n";
        dst = src;
        cout << "  [DMA] Transfer complete — IRQ raised\n";
        cycles += 1;
        return cycles;
    }

    void processIRQ() {
        lock_guard<mutex> lk(mtx);
        while (!irqQueue.empty()) {
            irqQueue.front()();
            irqQueue.pop();
        }
    }
};

// ■■ main ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
int main() {
    const size_t N = 1024; // 1 KB
    vector<uint8_t> srcBuf(N), dstBuf;
    iota(srcBuf.begin(), srcBuf.end(), 0);

    DeviceController ctrl;
    long totalPoll = 0, totalIRQ = 0;

    cout << "=== I/O Hardware Mechanisms (1 KB transfer) ===\n\n";

    // Polling
    cout << "[POLLING] Transferring " << N << " bytes...\n";
    for (auto b : srcBuf) totalPoll += ctrl.pollingIO(b);
    cout << "  CPU cycles used: " << totalPoll << "\n\n";

    // Interrupt-driven
    cout << "[INTERRUPT] Transferring " << N << " bytes...\n";
    for (auto b : srcBuf)
        totalIRQ += ctrl.interruptDrivenIO(b, [&]{ /* ISR: wake blocked process */ });
    cout << "  CPU cycles used: " << totalIRQ << "\n\n";

    // DMA
    cout << "[DMA] Transferring " << N << " bytes...\n";
    long dmaC = ctrl.dmaTransfer(dstBuf, srcBuf);
    cout << "  CPU cycles used: " << dmaC << "\n\n";

    // Summary
    cout << "=== CPU Cycles Comparison ===\n";
    cout << "  Polling:          " << totalPoll << " cycles\n";
    cout << "  Interrupt-driven: " << totalIRQ  << " cycles\n";
    cout << "  DMA:              " << dmaC      << " cycles\n";
    cout << "  DMA speedup over polling: " << totalPoll / (double)dmaC << "x\n";
    return 0;
}
