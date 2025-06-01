//
// Created by ferdinand on 5/21/25.
//

#include "Logger.h"

void Logger::init() {
    outputFile.open("vbsa.log", ffilesys::FOF_Write);
}

Logger& Logger::operator<<(const std::string &var) {
    outputFile.writeString(var);
    std::cout << var;
    return *this;
}

Logger& Logger::operator<<(char var) {
    outputFile.writeChar(var);
    std::cout << var;
    return *this;
}

Logger& Logger::operator<<(int var) {
    outputFile.writeString(std::to_string(var));
    std::cout << var;
    return *this;
}

Logger& Logger::info() {
    outputFile.writeString(std::string("[INFO] "));
    std::cout << "[INFO] ";
    return *this;
}

Logger& Logger::warn() {
    outputFile.writeString(std::string("[WARN] "));
    std::cout << "[WARN] ";
    return *this;
}

Logger& Logger::err() {
    outputFile.writeString(std::string("[ERR] "));
    std::cout << "[ERR] ";
    return *this;
}

void Logger::abort() {
    outputFile.writeString(std::string("Process aborted"));
    outputFile.close();
    std::cout << "Process aborted.\n";
    std::cout << "Press Enter to exit console.\n";
    std::cin.get();
    std::exit(1);
}
