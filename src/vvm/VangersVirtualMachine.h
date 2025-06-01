//
// Created by ferdinand on 5/31/25.
//

#pragma once

#include <stack>
#include <string>

#include "common/Dictionary.h"
#include "common/vangers-data.h"
#include "ffilesys/file-io.h"
#include "core/Logger.h"

#define VVM_NO_SUPPORT(action) error("Current VVM version doesn't supports " + std::string(action) + ".\n")

enum BinaryScriptType {
    Iscreen_Oftr,
    Actint_AciHi,
    MaxType,
};

typedef unsigned int instruction_t;

constexpr const char* BINARY_SCRIPT_TYPE_STRING[BinaryScriptType::MaxType] {
    "oftr",
    "aci_hi",
};

inline std::string verboseInstruction(int instr) {
    try { return mainDict.empty() ? std::to_string(instr) : std::to_string(instr) + "(" + mainDict.getToken(instr) + ")"; }
    catch (...) { return std::to_string(instr); }
}

struct ReadingHistory {
    std::string *historyBuffer;
    unsigned int bufferHead = 0;
    unsigned int bufferSize = 0;

    void create(const unsigned int size) {
        bufferSize = size;
        historyBuffer = new std::string[size];
    }

    void push(const std::string& action) {
        if (bufferHead == bufferSize) {
            for (int i = bufferSize - 1; i > 0; i--) {
                historyBuffer[i] = historyBuffer[i - 1];
            }
            historyBuffer[0] = action;
            return;
        }
        historyBuffer[bufferHead++] = action;
    }

    void print() const {
        mainLogger.info() << "Reading history:\n";
        mainLogger << "------------\n";
        for (int i = 0 ; i < bufferHead; i++) {
            mainLogger << (i ? '|' : '^');
            mainLogger << "> read " << historyBuffer[i] << '\n';
        }
        mainLogger << "------------\n";
    }
};

class VangersVirtualMachine {
public:

    inline static int numberReg = 0;
    inline static int stringSizeReg = 0;
    inline static std::string stringReg;
    inline static instruction_t instructionReg = 0;
    inline static size_t inputSizeReg = 0;
    inline static size_t inputOffsetReg = 0;

    ffilesys::File inputStream;
    BinaryScriptType scriptType;

    ReadingHistory history;

    void loadBinaryScript(const std::string& fileName, BinaryScriptType type);
    void run();

    void oftrInstructionStep();
    void aciHiInstructionStep();

    void oftrProcessElement();

    void error(const std::string& message) const;

    void printFetch() const;

private:

    int& readNumber();
    std::string& readString();
    instruction_t& readInstruction();

    void verifyHeader();
    void processCompression();
};

inline VangersVirtualMachine mainVvm;