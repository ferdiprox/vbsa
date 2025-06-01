//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <vector>
#include <set>

#include "ffilesys/file-io.h"

class Formatter {
public:
    std::vector<std::string> targetFiles;

    bool overwriteFormatted = false;

    void formatAll();
private:

    std::string buffer;
    size_t i = 0;

    // simply exists to track definitions names
    std::set<std::string> localDefinesMap;

    void formatFile(const std::string& fileName);

    void formatString();
    void formatDirective();

    void formatNumber(std::string& number);

    void skipSpaces();
    std::string substringToken();
    std::string substringQuoted();
};
