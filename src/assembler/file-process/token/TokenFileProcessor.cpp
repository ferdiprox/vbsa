//
// Created by ferdinand on 5/24/25.
//

#include "TokenFileProcessor.h"

#include "assembler/file-process/inter-directive.h"
#include "assembler/file-process/token/FileMathProcessor.h"

enum TokenizeMode {
    Skip,
    Read,
    ReadQuoted,
};

void TokenFileProcessor::load(const RawFileProcessor &sourceProcessor) {
    fileName = sourceProcessor.fileName;

    sourcesStack.emplace(fileName);
    currentSource = &sourcesStack.top();

    TokenizeMode currentMode = TokenizeMode::Skip;
    Token currentToken;

    char c;

    bool retryCharacter = false;

    for (; i < sourceProcessor.fileBuffer.size(); i++) {
        c = sourceProcessor.fileBuffer[i];

        switch (c) {
            default:
                break;
            case '\n':
                currentSource->line++;
                break;
            case INTERDIRECTIVE_SIGN: {
                i++;

                processInterdirective(sourceProcessor);

                retryCharacter = true;
            }
                break;
        }

        if (retryCharacter) {
            retryCharacter = false;
            continue;
        }

        if ((unsigned char)c < 9) {
            throw std::runtime_error("Unexpected non-text character reached: " + std::to_string(c));
        }

        switch (currentMode) {
            case TokenizeMode::Skip:
                if (!std::iswspace(c)) {
                    if (c == '"') {
                        currentMode = TokenizeMode::ReadQuoted;
                    }
                    else {
                        currentMode = TokenizeMode::Read;
                    }
                    currentToken += c;

                    currentToken.sourceFile = currentSource->fileName;
                    currentToken.sourceLine = currentSource->line;
                    break;
                }
                break;
            case TokenizeMode::Read:
                if (std::iswspace(c)) {
                    currentMode = TokenizeMode::Skip;
                    if (!currentToken.empty()) {
                        tokensList.push_back(currentToken);
                        currentToken.clear();
                    }
                    break;
                }
                currentToken += c;
                break;
            case TokenizeMode::ReadQuoted:
                if (c == '"') {
                    currentMode = TokenizeMode::Skip;
                    formatString(currentToken);
                    tokensList.push_back(currentToken);
                    currentToken.clear();
                    break;
                }
                currentToken += c;
                break;
        }
    }

    if (currentMode != TokenizeMode::Skip) {
        if (!currentToken.empty()) {
            tokensList.push_back(currentToken);
        }
    }
}

void TokenFileProcessor::process() {
    FileMathProcessor mathProcessor(fileName, tokensList);
    mathProcessor.execute();
#ifdef VBSA_DEBUG
    saveTokensDump();
#endif
}

void TokenFileProcessor::processInterdirective(const RawFileProcessor& sourceProcessor) {
    unsigned char interdirectiveType = sourceProcessor.fileBuffer[i];

    i++;

    switch (interdirectiveType) {
        default:
            throw std::runtime_error("Unknown Interdirective reached in Token File Processor");
        case InterdirectiveType::OpenFileSection:
            sourcesStack.emplace(sourceProcessor.fileBuffer.data() + i);

            currentSource = &sourcesStack.top();

            i += currentSource->fileName.size();
            break;
        case InterdirectiveType::CloseFileSection:
            sourcesStack.pop();

            if (sourcesStack.empty()) {
                throw std::underflow_error("Sources stack underflow (extra CloseFileSection interdirective?)");
            }

            currentSource = &sourcesStack.top();

            i--;
            break;
        case InterdirectiveType::ProcessorBreakpoint:
            switch (sourceProcessor.fileBuffer[i]) {
                default:
                    throw std::runtime_error("Unknown breakpoint target: " + std::to_string(sourceProcessor.fileBuffer[i]));
                case BreakpointTargetType::TokenProcessor:
                    call_breakpoint();
                    break;
                case BreakpointTargetType::MathProcessor: {
                    InterdirectiveTemplate breakpointToken = InterdirectiveTemplate(InterdirectiveType::ProcessorBreakpoint);
                    breakpointToken += char(BreakpointTargetType::MathProcessor);
                    tokensList.emplace_back(breakpointToken);
                    break;
                }
                case BreakpointTargetType::Assembler: {
                    InterdirectiveTemplate breakpointToken = InterdirectiveTemplate(InterdirectiveType::ProcessorBreakpoint);
                    breakpointToken += char(BreakpointTargetType::Assembler);
                    tokensList.emplace_back(breakpointToken);
                    break;
                }
            }
            break;
    }
}

void TokenFileProcessor::formatString(std::string &string) {
    for (int i = 0; i < string.size(); i++) {
        if (string[i] != '\\') {
            continue;
        }

        string.erase(i, 1);

        switch (string[i]) {
            default:
                mainLogger.warn() << "Unknown string format char: " << string[i] << "(in string \"" << string << "\")\n";
                break;
            case '\\':
                string.insert(string.begin() + i, '\\');
                break;
            case 'n':
                string[i] = '\n';
                break;
        }
    }
}
