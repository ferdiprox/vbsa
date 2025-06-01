//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>

#include "ffilesys/file-io.h"

class RawFileProcessor {
public:
    std::string fileName, fileBuffer;

    void load(const std::string& filename);

    // replacing \r\n to \n, merging \ slashes with newlines
    void basicConversionPhase();
    void commentErasingPhase();
    void preprocessingPhase();

    void balanceNewlineMerging();

    // useless
    void compressWhitespaces();

    void fullProcess();
    void subprocess();
private:

    int i = 0;
};
