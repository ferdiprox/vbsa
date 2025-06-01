//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>
#include <list>
#include <stack>

#include "assembler/file-process/raw/RawFileProcessor.h"

#include "assembler/file-process/token/token-trace.h"

class TokenFileProcessor {
public:
    std::string fileName;
    std::list<Token> tokensList;

    void load(const RawFileProcessor& sourceProcessor);

    void process();
private:

    struct SourceData {
        std::string fileName;
        unsigned int line = 1;

        explicit SourceData(const std::string& myFileName) : fileName(myFileName) {}
    };

    std::stack<SourceData> sourcesStack;

    SourceData* currentSource = nullptr;;
    size_t i = 0;

    void processInterdirective(const RawFileProcessor& sourceProcessor);

    static void formatString(std::string& string);
};
