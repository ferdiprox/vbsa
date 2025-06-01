//
// Created by ferdinand on 5/31/25.
//

#include "VvmConsole.h"

#include "IscreenVirtualUnit.h"
#include "local-dictionaries.h"
#include "vvm/VangersVirtualMachine.h"

constexpr const char* vvmcHelpText =  R"(Commands:
    help: print vvmc commands and their usage
    pr <[register name]>: print data contained in the VVM register
    pt <token index>: print original token from the dictionary (requires the dictionary to be loaded)
    pti <token string>: print token index from the dictionary (requires the dictionary to be loaded)
    pco: print current iscreen object
    pcs: print current iscreen objects stack
    ph: print reading history of VVM
    pf: print full fetch of VVM information
    exit: abort current VVM session and exit
    exit-y: same as exit but without confirmation
    stepi: step one instruction in the VVM
    run: step instructions until the EOF or VVM error
    echo <text>: just a test command, prints text into the terminal

Available register and their names for 'pr' command:
)";

void VvmConsole::start() {
    isEnabled = true;

    std::cout << "VVM Console is active now. Enter 'help' to get help information.\n";

    while (true) {
        command();
    }
}

#define NEXT_TOKEN() if(!(parser >> std::quoted(token))) { return; }

void VvmConsole::command() {
    std::cout << " > ";
    std::getline(std::cin, token);
    parser = std::istringstream();
    parser.str(token);

    if (!(parser >> token)) {
        return;
    }

    if (token == "help") {
        std::cout << vvmcHelpText;
        printAvailableRegisters();
    }
    else if (token == "exit") {
        std::cout << "Are you sure you want to abort the current VVM test and exit? (y/n): ";
        char answer;

        std::cin >> answer;
        if (std::tolower(answer) == 'y') {
            std::exit(0);
        }
    }
    else if (token == "exit-y") {
        std::exit(0);
    }
    else if (token == "stepi") {
        stepInstruction();
        vvmErrorFlag = false;
    }
    else if (token == "run") {
        if (mainVvm.inputOffsetReg == mainVvm.inputSizeReg) {
            std::cout << "No available script information in the VVM.\n";
            return;
        }

        while (mainVvm.inputOffsetReg < mainVvm.inputSizeReg) {
            stepInstruction();

            if (vvmErrorFlag) {
                break;
            }
        }
        vvmErrorFlag = false;
    }
    else if (token == "pr") {
        NEXT_TOKEN();

        std::cout << getRegisterValue(token) << '\n';
    }
    else if (token == "pt") {
        NEXT_TOKEN();

        if (mainDict.empty()) {
            std::cerr << "Dictionary is empty!\n";
            return;
        }

        try {
            std::string dictToken = mainDict.getToken(std::stoi(token));
            std::cout << mainDict.getToken(std::stoi(token)) << '\n';
        } catch (const std::exception& exception) {
            std::cerr << "Exception: " << exception.what() << '\n';
        }
    }
    else if (token == "pti") {
        NEXT_TOKEN();

        if (mainDict.empty()) {
            std::cerr << "Dictionary is empty!\n";
            return;
        }

        std::cout << mainDict.getIndex(token).value_or(-1) << '\n';
    }
    else if (token == "pco") {
        std::cout << getObjTypeString(mainIvu.currentObject) << '\n';
    }
    else if (token == "pcs") {
        if (mainIvu.objectsStack.empty()) {
            std::cout << "It's empty.\n";
            return;
        }

        for (int i = 0; i < mainIvu.objectsStack.size(); i++) {
            std::cout << i << ": " << getObjTypeString(mainIvu.objectsStack[i]) << '\n';
        }
    }
    else if (token == "ph") {
        mainVvm.history.print();
    }
    else if (token == "pf") {
        mainVvm.printFetch();
    }
    else if (token == "echo") {
        NEXT_TOKEN();

        std::cout << token << '\n';
    }
    else {
        std::cerr << "Unknown VVMC command: \"" << token << "\"\n";
    }
}

void VvmConsole::printAvailableRegisters() {
    for (int i = 0; i < VvmRegister::MaxRegister; i++) {
        std::cout << '\t' << VVM_REGISTER_DESCRIPTION[i] << ": " << VVM_REGISTER_STRING[i] << '\n';
    }
}

std::string VvmConsole::getRegisterValue(const std::string &name) {
    int registerId = -1;

    for (int i = 0; i < VvmRegister::MaxRegister; i++) {
        if (VVM_REGISTER_STRING[i] == name) {
            registerId = i;
            break;
        }
    }

    if (registerId == -1) {
        return "Unknown register name. See 'help' command to get available registers names.\n";
    }

    switch (registerId) {
        case VvmRegister::NumberReg:
            return std::to_string(VangersVirtualMachine::numberReg);
        case VvmRegister::StringSizeReg:
            return std::to_string(VangersVirtualMachine::stringSizeReg);
        case VvmRegister::StringDataReg:
            return '"' + VangersVirtualMachine::stringReg + '"';
        case VvmRegister::InstructionReg:
            return std::to_string(VangersVirtualMachine::instructionReg);
        case VvmRegister::InputSizeReg:
            return std::to_string(VangersVirtualMachine::inputSizeReg);
        case VvmRegister::InputOffsetReg:
            return std::to_string(VangersVirtualMachine::inputOffsetReg);
        default:
            throw std::runtime_error("Unexpected register type: " + std::to_string(registerId));
    }
}

void VvmConsole::stepInstruction() {
    switch (mainVvm.scriptType) {
        case BinaryScriptType::Iscreen_Oftr:
            mainVvm.oftrInstructionStep();
            break;
        case BinaryScriptType::Actint_AciHi:
            mainVvm.aciHiInstructionStep();
            break;
    }
}
