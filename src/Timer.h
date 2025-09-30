#ifndef TIMER_H
#define TIMER_H
#include <string>
#include <chrono>
#include <iostream>

class Timer {
public:
    explicit Timer(std::string name, long long* elapsed = nullptr) : name(std::move(name)), elapsed_(elapsed) { start = std::chrono::high_resolution_clock::now(); }
    void stop() {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

        if (elapsed_ != nullptr) *elapsed_ = duration;
        std::cout << name << ": " << duration << "(ms)\n";
        is_stopped = true;
    }
    ~Timer() {
        if (!is_stopped) {
            stop();
        }
    }
private:
    const std::string name;
    long long* elapsed_;
    std::chrono::high_resolution_clock::time_point start;
    bool is_stopped = false;
};

#endif