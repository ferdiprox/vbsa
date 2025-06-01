//
// Created by ferdinand on 5/24/25.
//

#include "file-trace.h"

#include "../../../core/Logger.h"

#include "../../../common/vangers-data.h"

void filetrace::traceIndexToXY(int index, const std::string &source, int &x, int &y) {
    x = y = 0;

    // getting X using just current line
    for (; index >= 0; index--) {
        if (source[index] == '\n') {
            y++;
            break;
        }
        x++;
    }

    // repeating it for others, but can ignore x now
    for (; index >= 0; index--) {
        if (source[index] == '\n') {
            y++;
        }
    }
}

std::string filetrace::substringTraceLine(int index, const std::string& source) {

    int left = index - 1, right = index;

    while (left) {
        if (source[left] == '\n') {
            left++;
            break;
        }

        left--;
    }

    while (right != source.size()) {
        if (source[right] == '\n') {
            break;
        }

        right++;
    }

#ifdef LIMIT_TRACE_LINE
    int currentLength = right - left;
    int delta = currentLength - TRACE_LINE_LENGTH_LIMIT;

    if (delta > 1) {
        if (delta % 2) {
            resolve_delta:

            delta /= 2;

            left += delta;
            right -= delta;
        }
        else {
            delta--;

            goto resolve_delta;
        }
    }
#endif

    return source.substr(left, right - left);
}

void filetrace::_trace_error_impl(int offset, const std::string &text, const std::string &source,
    const std::string &fileName, const std::string &moduleName) {

    int x, y;
    std::string tracedLine = filetrace::substringTraceLine(offset, source);
    std::string traceLinePrefix;

    for (char& c: tracedLine) {
        if (c == '\t') c = ' ';
    }

    traceIndexToXY(offset, source, x,  y);

    traceLinePrefix = "[" + std::to_string(y) + ", " + std::to_string(x + 1) + "]|";

    mainLogger.err() << "[" << offset << "]{" << fileName << "}: " << text << "(" << moduleName << ")\n";
    mainLogger.info()
        << "File trace: \n"
        << traceLinePrefix << tracedLine << '\n';

    int i = 0;
    for (; i < traceLinePrefix.size() + x; i++) {
        std::cout << ' ';
    }
    i = x;

    if (tracedLine[i] == '"') {
        for (++i; i < tracedLine.size(); i++) {
            if (tracedLine[i] == '"') {
                break;
            }
            std::cout << '~';
        }
    }
    else {
        for (; i < tracedLine.size(); i++) {
            if (std::iswspace(tracedLine[i])) {
                break;
            }
            std::cout << '~';
        }
    }

    std::cout << '\n';

    ffilesys::writeFullFile("error-source.scr", source);

    mainLogger.abort();
}