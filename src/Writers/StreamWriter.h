#pragma once
#include <iostream> 
#include <fstream>
#include <string>

class StreamWriter {
public:
    static int Write(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << content;
            file.close();
        }
        else {
            std::cout << "Unable to open file: " << filename << std::endl;
            return -1;
        }
        return 0;
    }
};