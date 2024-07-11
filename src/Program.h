#pragma once

#include <iostream>
#include <span>
#include <stdint.h>
#include <map>
#include "dotenv.h"

using namespace dotenv;

class Program {
public:
    Program();
    ~Program();

    static Program& Get();

    void Execute();

private:
    std::string BUILDINGS_CSV;

    std::string PATH_LAS;
    std::string PATH_LAS_NP;

    std::string PATH_EDG_IO;
    std::string PATH_EDG_MAT;
    std::string PATH_RDG_IO;

    std::string PATH_OUT;
};
