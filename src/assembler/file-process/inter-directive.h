//
// Created by ferdinand on 5/26/25.
//

#pragma once

/*
 * Inter-Directives are some information in the raw file buffers, which is written by preprocessor and read by token-processor.
 * For example, they used to separate file sections, to save debug information about tokens position.
 */

#include <string>

#ifdef __linux__
#include <csignal>
#define call_breakpoint() raise(SIGTRAP);
#else
// I don't think anyone would want to build this shit on anything other than windows or linux lol
#define call_breakpoint() __debugbreak()
#endif

constexpr char INTERDIRECTIVE_SIGN = 1;

enum BreakpointTargetType {
    TokenProcessor,
    MathProcessor,
    Assembler,
};

enum InterdirectiveType {
    OpenFileSection,
    CloseFileSection,
    ProcessorBreakpoint,
};

struct InterdirectiveTemplate : std::string {
    explicit InterdirectiveTemplate(InterdirectiveType type) {
        *this += INTERDIRECTIVE_SIGN;
        *this += static_cast<unsigned char>(type);
    }
};