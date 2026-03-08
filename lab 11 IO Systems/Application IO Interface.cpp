// ============================================================
// Chapter 12 — Ex 3: Application I/O Interface (Block, Char, Network)
// Compile: g++ -std=c++17 -pthread ex3_app_io_interface.cpp -o ex3
// ============================================================
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;
using ms = chrono::milliseconds;

// ■■ BlockDevice ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class BlockDevice {
    static const size_t BS = 512;
    vector<vector<uint8_t>> sectors;
    bool open_ = false;
public:
    explicit BlockDevice(size_t n) : sectors(n, vector<uint8_t>(BS, 0)) {}
    void open()  { open_ = true;  cout << "[BLK] opened\n"; }
    void close() { open_ = false; cout << "[BLK] closed\n"; }
    vector<uint8_t> readSector(size_t i) {
        if (!open_) throw runtime_error("not open");
        cout << "[BLK] read sector " << i << "\n";
        return sectors.at(i);
    }
    void writeSector(size_t i, const vector<uint8_t>& d) {
        if (!open_) throw runtime_error("not open");
        sectors.at(i) = d;
        cout << "[BLK] wrote " << d.size() << " bytes to sector " << i << "\n";
    }
};

// ■■ CharDevice ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class CharDevice {
    queue<char> buf;
    mutex mtx;
    bool open_ = false;
public:
    void open()  { open_ = true;  cout << "[CHR] opened\n"; }
    void close() { open_ = false; cout << "[CHR] closed\n"; }
    void inject(const string& s) {
        lock_guard<mutex> lk(mtx);
        for (char c : s) buf.push(c);
    }
    char getChar() {
        lock_guard<mutex> lk(mtx);
        if (buf.empty()) return '\0';
        char c = buf.front(); buf.pop();
        cout << "[CHR] getChar='" << c << "'\n";
        return c;
    }
    bool putChar(char c) {
        if (!open_) return false;
        cout << "[CHR] putChar='" << c << "'\n";
        return true;
    }
};

// ■■ NetworkSocket ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class NetworkSocket {
    string ip;
    int port;
    queue<string> recvQ;
    mutex mtx;
    bool open_ = false;
    int latencyMs = 5;
public:
    NetworkSocket(string ip_, int p) : ip(move(ip_)), port(p) {}
    void open()  { open_ = true;  cout << "[NET] connected to " << ip << ":" << port << "\n"; }
    void close() { open_ = false; cout << "[NET] disconnected\n"; }
    void injectPacket(const string& pkt) {
        lock_guard<mutex> lk(mtx); recvQ.push(pkt);
    }
    // MODE 1: Blocking
    string blockingRead() {
        cout << "[NET/BLOCK] waiting for data...\n";
        while (true) {
            lock_guard<mutex> lk(mtx);
            if (!recvQ.empty()) {
                string s = recvQ.front(); recvQ.pop();
                cout << "[NET/BLOCK] got: '" << s << "'\n";
                return s;
            }
        }
    }
    // MODE 2: Non-blocking
    string nonBlockingRead() {
        lock_guard<mutex> lk(mtx);
        if (recvQ.empty()) { cout << "[NET/NB] EAGAIN — no data available\n"; return ""; }
        string s = recvQ.front(); recvQ.pop();
        cout << "[NET/NB] got: '" << s << "'\n";
        return s;
    }
    // MODE 3: Asynchronous
    void asyncRead(function<void(string)> callback) {
        cout << "[NET/ASYNC] I/O submitted — CPU free\n";
        async(launch::async, [this, cb=move(callback)]() {
            this_thread::sleep_for(ms(latencyMs));
            cb("async-response");
        }).wait();
    }
    bool send(const string& data) {
        if (!open_) return false;
        cout << "[NET] sent: '" << data << "'\n";
        return true;
    }
};

// ■■ main ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
int main() {
    cout << "=== Application I/O Interface Demo ===\n\n";

    // Block device
    BlockDevice disk(64);
    disk.open();
    vector<uint8_t> sector(512, 0xAB);
    disk.writeSector(0, sector);
    auto rd = disk.readSector(0);
    cout << "  First byte: 0x" << hex << (int)rd[0] << dec << "\n\n";

    // Char device
    CharDevice kbd;
    kbd.open();
    kbd.inject("Hello!");
    for (int i = 0; i < 6; i++) kbd.getChar();
    cout << "\n";

    // Network — three I/O modes
    NetworkSocket sock("10.0.0.1", 443);
    sock.open();
    sock.nonBlockingRead();                    // empty → EAGAIN
    sock.injectPacket("HTTP/1.1 200 OK");
    sock.blockingRead();                       // has data
    sock.asyncRead([](const string& s) {
        cout << "[NET/ASYNC] callback: '" << s << "'\n";
    });
    sock.close();
    kbd.close();
    disk.close();
    return 0;
}
