//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>
#include <cwctype>
#include <map>
#include <set>

inline std::map<std::string, std::string> definitionsMap;
inline std::set<std::string> totalIncludedFiles;

class RawFileProcessor;

class FilePreprocessor  {
public:
    std::string & fileName,& fileBuffer;

    FilePreprocessor(RawFileProcessor& parentProcessorRef);

    void execute();
private:
    int i = 0;
    int currentDirectiveBegin = -1;

    void skipSpaces();
    std::string substringToken();
    std::string substringQuoted();

    void processIncludeDirective();
    void processDefineDirective();
    void processDefineInsertion(std::string& token);
    void processBreakpoint();

    void finishDirective();
};
