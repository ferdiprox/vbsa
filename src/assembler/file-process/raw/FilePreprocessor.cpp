//
// Created by ferdinand on 5/24/25.
//

#include "FilePreprocessor.h"

#define MODULE_NAME "File Preprocessor"
#include "assembler/Assembler.h"
#include "assembler/file-process/inter-directive.h"
#include "assembler/file-process/raw/file-trace.h"
#include "assembler/file-process/raw/RawFileProcessor.h"

#include "assembler/file-process/token/token-trace.h"

FilePreprocessor::FilePreprocessor(RawFileProcessor &parentProcessorRef)
    : fileName(parentProcessorRef.fileName), fileBuffer(parentProcessorRef.fileBuffer) {}

void FilePreprocessor::execute() {
    std::string token;
    bool isCurrentlyInString = false;

    for (; i < fileBuffer.size(); i++) {

        if (fileBuffer[i] != '#' && fileBuffer[i] != '$') {
            if (fileBuffer[i] == '"') {
                isCurrentlyInString ^= 1;
            }
            continue;
        }

        if (isCurrentlyInString) {
            continue;
        }

        currentDirectiveBegin = i;

        token = substringToken();

        if (token == "#define") {
            processDefineDirective();
        }
        else if (token == "#include") {
            processIncludeDirective();
        }
        else if (token == "#breakpoint_for") {
            processBreakpoint();
        }
        else {
            if (token[0] != '$') {
                traceError("unknown preprocessor directive: " + token);
            }

            processDefineInsertion(token);
        }
    }
}

void FilePreprocessor::skipSpaces() {
    for (; i < fileBuffer.size(); i++) {
        if (!std::iswspace(fileBuffer[i])) {
            break;
        }

        if (fileBuffer[i] == '\n') {
            traceError("preprocessor directives must be placed entirely into the one line");
        }
    }
}

std::string FilePreprocessor::substringToken() {
    int begin = i;

    if (fileBuffer[i] == '"') {
        traceError("quoted strings aren't supported here");
    }

    for (; i < fileBuffer.size(); i++) {
        if (std::iswspace(fileBuffer[i])) {
            break;
        }
    }

    return fileBuffer.substr(begin, i - begin);
}

std::string FilePreprocessor::substringQuoted() {
    if (fileBuffer[i] != '"') {
        traceError(std::string("non quoted-string tokens(") + fileBuffer[i] + ") aren't supported here");
    }

    i++;
    int begin = i;

    for (; i < fileBuffer.size(); i++) {
        switch (fileBuffer[i]) {
            default:
                break;
            case '\n':
                traceError("preprocessor directives must be placed entirely into the one line");
            case '"':
                return fileBuffer.substr(begin, i - begin);
        }
    }

    traceError("quoted string didn't ended with '\"' symbol, reached the end of file");
}

void FilePreprocessor::processIncludeDirective() {
    RawFileProcessor subProcessor;

    skipSpaces();

    const std::string includedFileName = substringQuoted();

    if (mainAssembler.enableIncludeGuards) {
        if (totalIncludedFiles.count(includedFileName)) {
            mainLogger.warn() << "double file include: \"" << includedFileName << "\" from \"" << fileName << "\"\n";
            finishDirective();
            return;
        }

        totalIncludedFiles.insert(includedFileName);
    }

    subProcessor.load(includedFileName);
    subProcessor.subprocess();

    finishDirective();

    InterdirectiveTemplate fileSeparator = InterdirectiveTemplate(InterdirectiveType::OpenFileSection);
    fileSeparator += includedFileName + '\0';

    fileBuffer.insert(fileBuffer.begin() + i, fileSeparator.begin(), fileSeparator.end());

    i += fileSeparator.size();

    std::string includedFileData = subProcessor.fileBuffer;

    fileBuffer.insert(fileBuffer.begin() + i, includedFileData.begin(), includedFileData.end());
    InterdirectiveTemplate fileEndSeparator = InterdirectiveTemplate(InterdirectiveType::CloseFileSection);

    fileBuffer.insert(fileBuffer.begin() + i + includedFileData.size(), fileEndSeparator.begin(), fileEndSeparator.end());
}

void FilePreprocessor::processDefineDirective() {
    skipSpaces();

    std::string newDefineName, newDefineData;

    newDefineName = substringToken();

    size_t definitionEnd = fileBuffer.find('\n', i);
    if (definitionEnd == std::string::npos) {
        definitionEnd = fileBuffer.size();
    }

    newDefineData = fileBuffer.substr(i, definitionEnd - i);

    definitionsMap[newDefineName] = newDefineData;

    finishDirective();
}

void FilePreprocessor::processDefineInsertion(std::string &token) {
    token.erase(0, 1);

    i = currentDirectiveBegin;

    auto defineIterator = definitionsMap.find(token);

    if (defineIterator == definitionsMap.end()) {
        traceError("\"" + token + "\" definition wasn't defined anywhere");
    }

    std::string defineData = defineIterator->second;

    fileBuffer.erase(i, token.size() + 1);
    fileBuffer.insert(fileBuffer.begin() + i, defineData.begin(), defineData.end());
}

void FilePreprocessor::processBreakpoint() {
    skipSpaces();
    std::string targetProcessor = substringToken();

    BreakpointTargetType target;

    if (targetProcessor == "preproc") {
        call_breakpoint();
        finishDirective();
        return;
    }
    if (targetProcessor == "mathproc") {
        target = BreakpointTargetType::MathProcessor;
    }
    else if (targetProcessor == "tokenproc") {
        target = BreakpointTargetType::TokenProcessor;
    }
    else if (targetProcessor == "assembler") {
        target = BreakpointTargetType::Assembler;
    }
    else {
        mainLogger.warn() << "Unknown breakpoint target: " << targetProcessor << " (ignored)\n";
        finishDirective();
        return;
    }

    InterdirectiveTemplate interdirective = InterdirectiveTemplate(InterdirectiveType::ProcessorBreakpoint);
    interdirective += char(target);

    finishDirective();

    fileBuffer.insert(fileBuffer.begin() + i, interdirective.begin(), interdirective.end());
}

void FilePreprocessor::finishDirective() {
    size_t directiveEnd = fileBuffer.find('\n', currentDirectiveBegin);
    if (directiveEnd == std::string::npos) {
        directiveEnd = fileBuffer.size();
    }

    fileBuffer.erase(currentDirectiveBegin, directiveEnd - currentDirectiveBegin);
    i = currentDirectiveBegin ? currentDirectiveBegin - 1 : 0;
    currentDirectiveBegin = -1;
}
