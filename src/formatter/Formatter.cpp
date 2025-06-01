//
// Created by ferdinand on 5/24/25.
//

#include "Formatter.h"

#include "common/parse-data.h"
#include "core/Logger.h"

void Formatter::formatAll() {
    if (targetFiles.empty()) {
        mainLogger.err() << "no format targets!\n";
        return;
    }

    for (const std::string& targetName : targetFiles) {
        formatFile(targetName);
    }
}

void Formatter::formatFile(const std::string &fileName) {
    ffilesys::readFullFile(fileName, buffer);

    for (i = 0; i < buffer.size(); i++) {
        switch (buffer[i]) {
            default:
                break;
            case '"':
                formatString();
                break;
            case '#':
                formatDirective();
                break;
            case '\r':
                buffer.erase(i, 1);
                break;
        }
    }

    ffilesys::writeFullFile(overwriteFormatted ? fileName : fileName + ".frm", buffer);

    mainLogger.info() << "formatted " << fileName << " successful...\n";
}

void Formatter::formatString() {
    size_t beginPos = i;

    for (i++; i < buffer.size(); i++) {

        switch (buffer[i]) {
            default:
                break;
            case '\r':
                buffer.erase(i, 1);
                i--;
                break;
            case '\n':
                buffer[i++] = '\\';
                buffer.insert(buffer.begin() + i, 'n');
                break;
            case '"':
                return;
        }
    }

    mainLogger.err() << "found endless string after string format: " << (int)beginPos << '\n';
    mainLogger.abort();
}

void Formatter::formatDirective() {
    i++;

    std::string directive = substringToken();

    if (directive == "define") {
        localDefinesMap.insert(substringToken());
    }
    else if (directive == "include") {
        const std::string fileName = substringQuoted();
        if (!std::filesystem::exists(fileName)) {
            mainLogger.err() << "include-required file \"" << fileName << "\" does not exists: " << (int)i << '\n';
            mainLogger.abort();
        }
    }
    else {
        mainLogger.err() << "unknown preprocessor directive: \"" << directive << "\": " << (int)i << "\n";
        mainLogger.abort();
    }
}

void Formatter::formatNumber(std::string &number) {
    if (!isNumberString(number)) {
        if (number[0] != '$') {
            mainLogger.warn() << "found define without '$' prefix in math expression\n";

            buffer.insert(buffer.begin() + i - number.size(), '$');

            mainLogger.info() << "automatically fixed to " << '$' << number << '\n';

            i++;
        }
    }
}

void Formatter::skipSpaces() {
    for (; i < buffer.size(); i++) {
        if (!std::iswspace(buffer[i])) {
            break;
        }
    }
}

std::string Formatter::substringToken() {
    skipSpaces();
    int begin = i;

    for (; i < buffer.size(); i++) {
        if (std::iswspace(buffer[i])) {
            break;
        }
    }

    return buffer.substr(begin, i - begin);
}

std::string Formatter::substringQuoted() {
    skipSpaces();
    int begin = i;

    formatString();

    i = ++begin;

    std::string output;

    while (buffer[i] != '"') {
        output += buffer[i++];
    }

    return output;
}
