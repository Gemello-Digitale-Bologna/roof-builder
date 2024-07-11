#pragma once

#include <chrono>
#include <iostream>

class Timer {
public:
    Timer() {
    }

    ~Timer() {
    }

    void Start() {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    void Stop() {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - m_StartTimepoint).count() << "ms" << std::endl;
    }

    void Stop(std::string string) {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << string << std::chrono::duration_cast<std::chrono::milliseconds>(end - m_StartTimepoint).count() << "ms" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};