//
// Created by ferdinand on 5/21/25.
//

#pragma once

#include <iostream>

#include "ffilesys/file-io.h"

class Logger {
public:

    void init();

    Logger& operator <<(const std::string &var);
    Logger& operator <<(char var);
    Logger& operator <<(int var);

    Logger& info();
    Logger& warn();
    Logger& err();

    [[noreturn]] void abort();

private:
    ffilesys::File outputFile;
};

inline Logger mainLogger;

