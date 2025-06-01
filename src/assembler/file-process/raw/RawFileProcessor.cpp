//
// Created by ferdinand on 5/24/25.
//

#include "RawFileProcessor.h"

#define MODULE_NAME "Raw File Processor"
#include "assembler/file-process/raw/file-trace.h"
#include "assembler/file-process/raw/FilePreprocessor.h"
#include "assembler/Assembler.h"

void RawFileProcessor::load(const std::string &filename) {
    fileName = filename;
    ffilesys::readFullFile(filename, fileBuffer);
}

void RawFileProcessor::basicConversionPhase() {

    for (i = 0; i < fileBuffer.size(); i++) {
        switch (fileBuffer[i]) {
            default:
                break;
            case '\r':
                if (fileBuffer[i + 1] != '\n') {
                    traceError("expected a '\\n' character after '\\r'");
                }

                // erasing \r to keep only \n characters
                fileBuffer.erase(i, 1);
                i--;
                break;
            case '\\': {
                if (!std::iswspace(fileBuffer[i + 1])) {
                    break;
                }

                size_t newlinePos = fileBuffer.find('\n', i);
                if (newlinePos == std::string::npos) {
                    traceError("expected a '\\n' character after '\\'");
                }
                newlinePos++;

                int erasingSize = newlinePos - i;
                fileBuffer.erase(i, erasingSize);
                i--;
                balanceNewlineMerging();
            }
                break;
        }
    }
}

void RawFileProcessor::commentErasingPhase() {
    size_t commentEnd;

    for (i = 0; i < fileBuffer.size(); i++) {
        if (fileBuffer[i] != '/') {
            continue;
        }

        if (fileBuffer[i + 1] == '/') {
            commentEnd = fileBuffer.find('\n', i);

            if (commentEnd == std::string::npos) {
                commentEnd = fileBuffer.size();
            }

            fileBuffer.erase(i, commentEnd - i);
            // comments are replaced with whitespace characters
            fileBuffer.insert(fileBuffer.begin() + i, ' ');
            i--;
        }
        else if (fileBuffer [i + 1] == '*') {
            commentEnd = fileBuffer.find("*/", i);

            if (commentEnd == std::string::npos) {
                traceError("couldn't find a '*/' symbols as the end of c-style comment");
            }

            std::string totalCommentReplacement = " ";

            for (int j = i; j < commentEnd; j++) {
                if (fileBuffer[j] == '\n') {
                    totalCommentReplacement += '\n';
                }
            }

            // length of '*/'
            commentEnd += 2;

            fileBuffer.erase(i, commentEnd - i);
            fileBuffer.insert(fileBuffer.begin() + i, totalCommentReplacement.begin(), totalCommentReplacement.end());
            i--;
        }
    }
}

void RawFileProcessor::preprocessingPhase() {
    FilePreprocessor preprocessor(*this);
    preprocessor.execute();
}

void RawFileProcessor::balanceNewlineMerging() {
    int j;
    for (j = i; j >= 0; j--) {
        if (fileBuffer[j] == '\n') {
            break;
        }
    }

    if (j == -1) {
        j = 0;
    }

    fileBuffer.insert(fileBuffer.begin() + j, '\n');
}

void RawFileProcessor::compressWhitespaces() {
    int beg, end;
    bool hadNewline;
    for (i = 0; i < fileBuffer.size(); i++) {
        if (!std::iswspace(fileBuffer[i])) {
            continue;
        }

        hadNewline = fileBuffer[i] == '\n';
        beg = i;

        for (end = ++i; end < fileBuffer.size(); end++) {
            if (!std::iswspace(fileBuffer[end])) {
                break;
            }

            if (fileBuffer[end] == '\n') {
                hadNewline = true;
            }
        }

        fileBuffer.erase(beg, end - beg);
        fileBuffer.insert(fileBuffer.begin() + beg, hadNewline ? '\n' : ' ');

        i = beg;
    }
}


void RawFileProcessor::fullProcess() {
    totalIncludedFiles.insert(fileName);

    subprocess();

    ffilesys::writeFullFile("subprocessed-tmp.scr", fileBuffer);
}

void RawFileProcessor::subprocess() {
    basicConversionPhase();
    commentErasingPhase();
    //if (mainAssembler.compressWhitespaces) {
    //    compressWhitespaces();
    //}
    preprocessingPhase();
}
