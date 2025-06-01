//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>
#include <list>
#include <utility>

#include "core/Logger.h"
#include "ffilesys/file-io.h"

struct Token : std::string {
    std::string sourceFile;
    unsigned int sourceLine = 0;

    explicit Token(std::string& string) {
        this->assign(std::move(string));
    }

    explicit Token(const std::string& string) {
        this->assign(string);
    }

    Token() = default;
};

typedef std::list<Token>::iterator token_iter_t;

void saveTokensDump_impl(std::list<Token>& tokensList);

inline void verboseTokenLog(const std::string& message, const token_iter_t& subjectToken){
    mainLogger << "[Line:" + std::to_string(subjectToken->sourceLine) + "]{File:'" << subjectToken->sourceFile << "'}(Subj:'" + *subjectToken + "'): " << message << '\n';
}

#define tokenError(msg, pos) mainLogger.err(); verboseTokenLog(msg, pos); mainLogger.abort();
#define tokenWarn(msg, pos) mainLogger.warn(); verboseTokenLog(msg, pos);
#define saveTokensDump() saveTokensDump_impl(tokensList);