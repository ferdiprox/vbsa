set(CMAKE_BUILD_TYPE DEBUG)

add_executable(${PROJECT_NAME}
        main.cpp
        assembler/Assembler.cpp
        assembler/Assembler.h
        core/Logger.cpp
        core/Logger.h
        common/Dictionary.cpp
        common/Dictionary.h
        common/DictParser.cpp
        common/DictParser.h
        assembler/file-process/raw/RawFileProcessor.cpp
        assembler/file-process/raw/RawFileProcessor.h
        assembler/file-process/raw/FilePreprocessor.cpp
        assembler/file-process/raw/FilePreprocessor.h
        assembler/file-process/raw/file-trace.h
        assembler/file-process/raw/file-trace.cpp
        assembler/file-process/token/FileMathProcessor.cpp
        assembler/file-process/token/FileMathProcessor.h
        assembler/file-process/token/TokenFileProcessor.cpp
        assembler/file-process/token/TokenFileProcessor.h
        assembler/file-process/token/token-trace.cpp
        assembler/file-process/token/token-trace.h
        common/vangers-data.h
        formatter/Formatter.cpp
        formatter/Formatter.h
        core/arg-parse.h
        core/arg-parse.cpp
        assembler/analyze-tool.h
        assembler/analyze-tool.cpp
        common/parse-data.cpp
        common/parse-data.h
        assembler/file-process/inter-directive.h
        vvm/VangersVirtualMachine.cpp
        vvm/VangersVirtualMachine.h
        vvm/local-dictionaries.h
        vvm/IscreenVirtualUnit.cpp
        vvm/IscreenVirtualUnit.cpp
        vvm/IscreenVirtualUnit.h
        vvm/oftr-test-impl.cpp
        vvm/VvmConsole.cpp
        vvm/VvmConsole.h
)

target_link_libraries(${PROJECT_NAME} ffilesys)