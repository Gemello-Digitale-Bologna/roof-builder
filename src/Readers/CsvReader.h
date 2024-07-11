#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <locale>
#include <codecvt>
#include "Reader.h"

class CsvReader {
public:
    CsvReader();
    void Read();
    void Read(const std::string& filePath);
    std::vector<std::string>* GetPtr();
    std::vector<std::string> Get();
    void Flush();
    ~CsvReader();

private:
    std::vector<std::string> m_Lines;
};