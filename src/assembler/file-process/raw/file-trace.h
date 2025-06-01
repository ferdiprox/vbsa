//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>

#ifndef MODULE_NAME
#define MODULE_NAME "UNKNOWN-MODULE"
#endif

namespace filetrace {
#ifdef LIMIT_TRACE_LINE
    static constexpr int TRACE_LINE_LENGTH_LIMIT = 40;
#endif

    void traceIndexToXY(int index, const std::string& source, int& x, int& y);

    std::string substringTraceLine(int index, const std::string& source);

    [[noreturn]] void _trace_error_impl(int offset, const std::string& text, const std::string& source, const std::string& fileName, const std::string& moduleName);
}

#define traceError(msg) filetrace::_trace_error_impl(i, msg, fileBuffer, fileName, MODULE_NAME)