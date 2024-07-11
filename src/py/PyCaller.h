#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

class PyCaller {
public:
    void Load(const std::string& path) {
        executablePath = path;
    }

    void Execute(const std::vector<std::string>& args) {
        std::string command = "python3 " + executablePath;

        for (const auto& arg : args) {
            command += " " + arg;
        }

        system(command.c_str());
    }

private:
    std::string executablePath;
};
