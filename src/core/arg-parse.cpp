//
// Created by ferdinand on 5/25/25.
//

#include "arg-parse.h"

#include "common/Dictionary.h"

#include "formatter/Formatter.h"

#include "assembler/analyze-tool.h"
#include "assembler/Assembler.h"

#include "vvm/VvmConsole.h"
#include "vvm/VangersVirtualMachine.h"

namespace argparse {
    constexpr const char* programHelpText = R"(Vangers Binary Scripts Assembler by ferdi

Usage examples:
    vbsa -d oftr.dct -o oftr2.scb -a iscreen/oftr2.scr <- Assemble russian version of oftr script
    vbsa -d oftr.dct -t oftr.scb oftr --interactive <- Run a english oftr script in the VVM through its console
    vbsa -f actint/*.inc --overwrite-formatted <- Format all *.inc files in the actint/ directory

Divides on three main sections: Assembler, Formatter and VVM(Vangers Virtual Machine).

Assembler options:
    -a, --assemble: Assemble script file(s). Takes the input, output and dictionary's file name from vbsa.cfg file.
        -o, --output <file name>: Name of output binary file. Required by Assembler.
        -d, --dictionary <file name>: Load dictionary from file. Required by Assembler.
    -an, --analyze <token>: Analyze token as if it was assembled to the binary script.
    -W<error-type>: Lower the level of error case to warning.

Formatter options:
    -f, --format [Target files...]: Format files to VBSA script standard.
        --overwrite-formatted: Overwrite target files instead of creating new.

VVM options:
    -t, --test <target file> <script type: [oftr|aci_hi]>: Run the binary script through virtual machine which simulates the Vangers script parser.
            You can specify "-" as target file, if you don't want to actually run any script (for example, you just want to open VVM console)
        --interactive: Instead of autorun, VVM console will be opened, where you can track registers, actions, read/unread values, and so on.
        --history-size <size>: Change the size of VVM's actions history.
    Note - you will be able to see the original token of instruction at the error if you load the dictionary of input script.

Other options:
    -h, --help: Print text about VBSA and its options.
    -v, --version: Print program's build type, version, and compilation date.
    -wl: Print all available error types for -W option.
)";

    constexpr const char* programAvailableErrorControls = R"(-Wimplicit-string-cast: when token without quotes are converted by assembler to string with quotes
)";

    std::string programVersion =
#ifdef VBSA_DEBUG
            "debug-" +
#else
            "dist-" +
#endif
        std::string("v1.0");
}

void argparse::getMultipleTokens(std::vector<std::string> &targetBuffer) {
    for (; off < argc; off++) {
        if (argv[off][0] == '-') {
            off--;
            break;
        }

        targetBuffer.emplace_back(argv[off]);
    }
}

void argparse::processArguments() {
    for (off = 1; off < argc; off++) {
        std::string token = argv[off];

        /* ASSEMBLER */

        if (token == "-a" || token == "--assemble") {
            inputArgument = nextToken();
            finalActionFlags |= FinalAct_Assemble;
        }
        else if (token == "-d" || token == "--dictionary") {
            dictFileName = nextToken();
            preActionFlags |= PreAct_LoadDictionary;
        }
        else if (token == "-o" || token == "--output") {
            outputFileName = nextToken();
        }
        else if (token == "-an" || token == "--analyze") {
            inputArgument = nextToken();
            finalActionFlags |= FinalAct_Analyze;
        }
        else if (token[0] == '-' && token[1] == 'W') {
            token.erase(0, 2);

            if (token == "implicit-string-cast") {
                additionalFlags |= Add_W_ImplicitStringCast;
            }
            else {
                std::cerr << "Unknown error level: \"" << token << "\"\n";
            }
        }

        /* FORMATTER */

        else if (token == "-f" || token == "--format") {
            nextToken();

            getMultipleTokens(formatTargetNames);
            finalActionFlags |= FinalAct_Format;
        }
        else if (token == "--overwrite-formatted") {
            additionalFlags |= Add_OverwriteFormatted;
        }

        /* VVM */

        else if (token == "-t" || token == "--test") {
            inputArgument = nextToken();
            std::string scriptType = nextToken();

            testScriptType = -1;

            for (int i = 0; i < BinaryScriptType::MaxType; i++) {
                if (BINARY_SCRIPT_TYPE_STRING[i] == scriptType) {
                    testScriptType = i;
                    break;
                }
            }

            if (testScriptType == -1) {
                std::cerr << "Unknown binary script type to test: " << scriptType << "\nTry --help to see available types.\n";
            }
            else {
                finalActionFlags |= FinalAct_TestScript;
            }
        }
        else if (token == "--interactive") {
            additionalFlags |= Add_Interactive;
        }
        else if (token == "--history-size") {
            vvmActionHistorySize = std::stoi(nextToken());
        }

        /* OTHER */

        else if (token == "-h" || token == "--help") {
            std::cout << programHelpText;
        }
        else if (token == "-v" || token == "--version") {
            std::cout << "VBSA " << programVersion << "\n";
            std::cout << "from " << __DATE__ << "\n";
        }
        else if (token == "-wl") {
            std::cout << programAvailableErrorControls;
        }

        else {
            std::cerr << "Unknown program argument: \"" << token << "\"\n";
        }
    }
}

#define checkLaunchRequirements(action, requirement) \
        if(requirement.empty()) { \
            std::cerr << "Cant " << action << ": " << #requirement << " is empty or not specified.\n"; \
            std::exit(1); \
        }
#define assertLaunchRequirements(action, expression) \
        if(!(expression)) { \
            /* TODO: this is actually not really informative message, but i dont know how to describe it less debug-log like. */ \
            std::cerr << "Cant " << action << ": failed to assert \"" << #expression << "\"\n"; \
            std::exit(1); \
        }

void argparse::processActions() {

    /* PREACTIONS */

    if (preActionFlags & PreAct_LoadDictionary) {
        checkLaunchRequirements("load dictionary", dictFileName);

        mainDict.load(dictFileName);
    }

    /* FINAL ACTIONS */

    if (finalActionFlags & FinalAct_Assemble) {
        checkLaunchRequirements("launch assembler", inputArgument);
        checkLaunchRequirements("launch assembler", outputFileName);
        assertLaunchRequirements("launch assembler", !mainDict.empty());

        mainAssembler.errorImplicitStringCast = !(additionalFlags & Add_W_ImplicitStringCast);

        mainAssembler.assemble(inputArgument, outputFileName);
    }
    if (finalActionFlags & FinalAct_Format) {
        Formatter formatter;

        formatter.targetFiles = formatTargetNames;
        formatter.overwriteFormatted = additionalFlags & Add_OverwriteFormatted;

        formatter.formatAll();
    }
    if (finalActionFlags & FinalAct_Analyze) {
        checkLaunchRequirements("launch analyzer", inputArgument);
        assertLaunchRequirements("launch analyzer", !mainDict.empty());

        analyzer::analyze(inputArgument);
    }
    if (finalActionFlags & FinalAct_TestScript) {
        checkLaunchRequirements("test binary script", inputArgument);
        assertLaunchRequirements("test binary script", testScriptType != -1);

        mainVvm.history.create(vvmActionHistorySize);
        mainVvm.loadBinaryScript(inputArgument, static_cast<BinaryScriptType>(testScriptType));

        if (additionalFlags & Add_Interactive) {
            mainVvmConsole.start();
        }
        else {
            mainVvm.run();
        }
    }
}
