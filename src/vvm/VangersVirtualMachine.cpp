//
// Created by ferdinand on 5/31/25.
//

#include "VangersVirtualMachine.h"

#include "common/parse-data.h"
#include "core/arg-parse.h"

#include "vvm/VvmConsole.h"

int & VangersVirtualMachine::readNumber() {
    inputStream.readOne(numberReg);

    inputOffsetReg += sizeof(int);
    history.push("number(" + std::to_string(numberReg) + ")");
    return numberReg;
}

std::string & VangersVirtualMachine::readString() {
    inputStream.readOne(stringSizeReg);
    stringReg.resize(stringSizeReg);

    inputStream.readSome(stringReg.data(), stringSizeReg);
    inputStream.seek(ffilesys::FO_Current, 1);

    inputOffsetReg += sizeof(stringSizeReg) + stringReg.size() + 1;
    history.push("string(" + stringReg + ")[" + std::to_string(stringSizeReg) + "]");
    return stringReg;
}

instruction_t & VangersVirtualMachine::readInstruction(){
    inputStream.readOne(instructionReg);

    inputOffsetReg += sizeof(instruction_t);
    history.push("instruction(" + std::to_string(instructionReg) + ")");
    return instructionReg;
}

void VangersVirtualMachine::aciHiInstructionStep() {
    VVM_NO_SUPPORT("aci_hi instructions");
}

void VangersVirtualMachine::verifyHeader() {
    int headerSize = strlen(BIN_SCR_HEADER) + 1;

    std::string scriptHeader;
    scriptHeader.resize(headerSize);
    inputStream.readSome(scriptHeader.data(), scriptHeader.size());
    inputOffsetReg += headerSize;

    // erasing null byte at the end otherwise it would be compared too
    scriptHeader.erase(scriptHeader.size() - 1, 1);

    if (scriptHeader != BIN_SCR_HEADER) {
        error("Wrong binary script header, expected \"" + std::string(BIN_SCR_HEADER) + "\", read \"" + scriptHeader + "\"");
    }
}

void VangersVirtualMachine::processCompression() {
    VVM_NO_SUPPORT("comperssed scripts testing");
}

void VangersVirtualMachine::loadBinaryScript(const std::string &fileName, const BinaryScriptType type) {
    if (fileName == "-") {
        return;
    }

    inputStream.open(fileName, ffilesys::FOF_Read | ffilesys::FOF_Binary);
    inputSizeReg = std::filesystem::file_size(fileName);

    scriptType = type;

    verifyHeader();
    if (readNumber()) {
        processCompression();
    }
}

void VangersVirtualMachine::run() {
    switch (scriptType) {
        case BinaryScriptType::Iscreen_Oftr:
            while (inputOffsetReg < inputSizeReg) {
                oftrInstructionStep();
            }
            break;
        case BinaryScriptType::Actint_AciHi:
            VVM_NO_SUPPORT("aci_hi testing");
            break;
    }
}

void VangersVirtualMachine::error(const std::string &message) const {
    mainLogger.err() << "VVM Error at " << (int)inputOffsetReg << ": " << message << '\n';
    printFetch();

    if (mainVvmConsole.isEnabled) {
        mainVvmConsole.vvmErrorFlag = true;
        return;
    }
    mainLogger.abort();
}

void VangersVirtualMachine::printFetch() const {
    mainLogger.info() << "Registers: " <<
        "\ni32  : " << numberReg <<
        "\nss32 :  " << stringSizeReg <<
        "\nsptr : \"" << stringReg << '"' <<
        "\ninst : " << (int)instructionReg <<
        "\nis64 : " << (int)inputSizeReg <<
        "\nip64 : " << (int)inputOffsetReg << '\n';
    history.print();

}
