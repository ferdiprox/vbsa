//
// Created by ferdinand on 5/25/25.
//
#include "analyze-tool.h"

#include "common/Dictionary.h"
#include "common/parse-data.h"

void analyzer::analyze(const std::string &token) {
    std::optional<unsigned int> tokenIndex = mainDict.getIndex(token);

    if (isNumberChar(token[0])) {
        std::cout << "It looks like a number. \n";
        if (token.size() > 1 && token[1] == 'x') {
            std::cout << "It is a hex number.\n";
            std::cout << "std::stoi(16): " << std::stoi(token, nullptr, 16) << '\n';
        }
        else {
            std::cout << "std::stoi(): " << std::stoi(token) << '\n';
        }
    }
    else  if (token[0] == '#') {
        std::cout << "It is a preprocessor directive.\n";
    }
    else if (tokenIndex.has_value()) {
        std::cout << "It is a dictionary-defined token with index: " << tokenIndex.value() << ".\n";
    }
    else {
        std::cout << "It is not a dictionary-defined token.\n";

        std::cout << "It is a string.\n";
    }
}
