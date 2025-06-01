//
// Created by ferdinand on 5/21/25.
//

#pragma once

#include <vector>

#include "ffilesys/file-io.h"

class DictParser {
public:
    DictParser(std::vector<std::string>& tokensTableSource) :
        tokensTableRef(tokensTableSource) {};

    void parse(const std::string& filename);
private:
    ffilesys::File dictFile;
    size_t fileSize = 0;
    std::string tokenBuffer;
    int readPos = 0;

    std::vector<std::string>& tokensTableRef;

    std::string readLine();
    void skipSpaces();

    bool eof() const {
        return readPos == fileSize;
    }
};
