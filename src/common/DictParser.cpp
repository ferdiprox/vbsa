//
// Created by ferdinand on 5/21/25.
//

#include "DictParser.h"

#include "core/Logger.h"

void DictParser::parse(const std::string &filename) {
    dictFile.open(filename, ffilesys::FOF_Read);
    fileSize = std::filesystem::file_size(filename);

    char c;

    for (readPos =  0; readPos < fileSize; readPos++) {
        tokenBuffer = readLine();

        if (tokenBuffer == "\\n") {
            tokenBuffer.clear();
            tokenBuffer += '\n';
        }

        tokensTableRef.emplace_back(tokenBuffer);
    }
}

std::string DictParser::readLine() {
    char c;
    std::string output;

    for (; readPos < fileSize; readPos++) {
        dictFile.readChar(c);

        switch (c) {
            default:
                output += c;
                break;
            case '.':
                readPos = fileSize;
                dictFile.close();
                return output;
            case ',':
                skipSpaces();
                return output;
        }
    }

    dictFile.close();
    return output;
}

void DictParser::skipSpaces() {
    char c;

    for (; readPos < fileSize; readPos++) {
        dictFile.readChar(c);
        if (!std::iswspace(c)) {
            dictFile.seek(-1, ffilesys::FO_Current);
            return;
        }
    }
}
