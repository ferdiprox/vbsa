//
// Created by ferdinand on 5/21/25.
//

#pragma once

#include <string>
#include <vector>
#include <list>

#include "assembler/file-process/token/token-trace.h"

class Assembler {
public:
    // assembler will just skip them
    std::vector<std::string> tokensBlackList {
        "{",
    };

    //bool compressWhitespaces = false;
    bool enableIncludeGuards = false;

    // error levels control (if false, these cases will be logged as warn)
    bool errorImplicitStringCast = true;

    void assemble(const std::string& filename, const std::string& outputName);
private:
    void binarize(const std::string &outputName, std::list<Token> &tokensList);
};

inline Assembler mainAssembler;

inline void levelControlledError(const std::string& message, const token_iter_t& subject, const bool controller) {
    if (controller) {
        tokenError(message, subject);
    }
    tokenWarn(message, subject);
}
