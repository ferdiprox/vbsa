//
// Created by ferdinand on 5/21/25.
//

#include "Assembler.h"

#include "assembler/file-process/raw/RawFileProcessor.h"
#include "assembler/file-process/token/TokenFileProcessor.h"

#include "common/Dictionary.h"
#include "common/parse-data.h"

#include "common/vangers-data.h"
#include "core/arg-parse.h"
#include "file-process/inter-directive.h"

void Assembler::assemble(const std::string &filename, const std::string& outputName) {
    mainLogger.info() << "Starting up an Assembler...\n";

    RawFileProcessor rawProcessor;
    TokenFileProcessor tokenProcessor;

    rawProcessor.load(filename);
    rawProcessor.fullProcess();

    tokenProcessor.load(rawProcessor);
    tokenProcessor.process();

    binarize(outputName, tokenProcessor.tokensList);
}

void Assembler::binarize(const std::string &outputName, std::list<Token> &tokensList) {
    ffilesys::File outputFile;
    outputFile.open(outputName, ffilesys::FOF_Write | ffilesys::FOF_Binary);

    // header
    outputFile.writeString(std::string(BIN_SCR_HEADER));
    outputFile.writeChar(0);
    // compression sign (need to think about compression support in VBSA)
    outputFile.writeOne<int>(0);

    std::optional<unsigned int> tokenIndex;

    int tokenIndexInList = 0;

    for (token_iter_t tokenIterator = tokensList.begin(); tokenIterator != tokensList.end(); ++tokenIterator) {
        Token& token = *tokenIterator;

        if (token[0] == INTERDIRECTIVE_SIGN) {
            if (token.size() == 3) {
                if (token[1] == InterdirectiveType::ProcessorBreakpoint && token[2] == BreakpointTargetType::Assembler) {
                    call_breakpoint();
                }
            }

            continue;
        }

        if (isNumberChar(token[0])) {

            // hex number
            if (token.size() > 1 && token[1] == 'x') {
                token.erase(0, 2);
                outputFile.writeOne<int>(std::stoi(token, nullptr, 16));
                continue;
            }

            outputFile.writeOne<int>(std::stoi(token));
            continue;
        }

        if (token[0] == '"') {
            token.erase(0, 1);

            write_as_string:

            outputFile.writeOne<int>(token.size());
            outputFile.writeString(token);
            outputFile.writeChar(0);
            continue;
        }

        tokenIndex = mainDict.getIndex(token);

        if (!tokenIndex.has_value()) {
            bool isInBlackList = false;
            for (const std::string& blacklistToken : tokensBlackList) {
                if (blacklistToken == token) {
                    isInBlackList = true;
                    break;
                }
            }
            
            if (isInBlackList) {
                continue;
            }

            levelControlledError("implicit cast to string", tokenIterator, errorImplicitStringCast);
            goto write_as_string;
        }

        outputFile.writeOne<int>(tokenIndex.value());
    }

    outputFile.writeOne<int>(EOF_SIGN);
    outputFile.close();

    mainLogger.info() << "Saved binary script in \"" << outputName << "\"\n";
}
