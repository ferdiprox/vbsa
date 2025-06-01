//
// Created by ferdinand on 5/25/25.
//

#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace argparse {

    enum PreActionFlags {
        PreAct_LoadDictionary = 1 << 0,
    };

    enum FinalActionFlags {
        FinalAct_Assemble = 1 << 0,
        FinalAct_Format = 1 << 1,
        FinalAct_Analyze = 1 << 2,
        FinalAct_TestScript = 1 << 3,
    };

    enum AdditionalFlags {
        Add_W_ImplicitStringCast = 1 << 0,

        Add_OverwriteFormatted = 1 << 1,

        Add_Interactive = 1 << 2,
    };

    inline int argc;
    inline char** argv;
    inline int off = 0;
    inline std::string token;

    inline std::string outputFileName;
    inline std::string inputArgument;
    inline std::string dictFileName;
    inline int testScriptType;
    inline int vvmActionHistorySize = 8;

    inline std::vector<std::string> formatTargetNames;

    inline int preActionFlags = 0;
    inline int finalActionFlags = 0;
    inline int additionalFlags = 0;

    inline void load(int programArgc, char** programArgv) {
        argc = programArgc;
        argv = programArgv;
    }

    inline std::string nextToken() {
        if (++off == argc) {
            std::cerr << "Not enough program arguments. Try \"--help\".\n";
            std::exit(1);
        }
        return token = argv[off];
    }

    void getMultipleTokens(std::vector<std::string>& targetBuffer);

    void processArguments();

    void processActions();
}
