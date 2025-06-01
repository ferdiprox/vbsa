//
// Created by ferdinand on 5/31/25.
//

#pragma once

#include <iostream>
#include <sstream>

enum VvmRegister {
    NumberReg,
    StringSizeReg,
    StringDataReg,
    InstructionReg,
    InputSizeReg,
    InputOffsetReg,
    MaxRegister,
};

constexpr const char* VVM_REGISTER_STRING[VvmRegister::MaxRegister] {
    "i32",
    "ss32",
    "sptr",
    "inst",
    "is64",
    "ip64",
};

constexpr const char* VVM_REGISTER_DESCRIPTION[VvmRegister::MaxRegister] {
    "Number register",
    "String size register",
    "String data register",
    "Instruction register",
    "Input size register",
    "Input offset register",
};

class VvmConsole {
public:
    void start();

    bool isEnabled = false;
    bool vvmErrorFlag = false;
private:
    void command();

    void printAvailableRegisters();

    std::string getRegisterValue(const std::string& name);

    void stepInstruction();

    std::istringstream parser;
    std::string token;
};

inline VvmConsole mainVvmConsole;