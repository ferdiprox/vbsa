//
// Created by ferdinand on 5/24/25.
//

#include "token-trace.h"

#include "../../../core/Logger.h"

void saveTokensDump_impl(std::list<Token> &tokensList) {
    ffilesys::File dumpfile;
    dumpfile.open(  "error-dump.tdmp", ffilesys::FOF_Write);

    for (token_iter_t iter = tokensList.begin(); iter != tokensList.end(); ++iter) {
        dumpfile.writeString(*iter + "\n");
    }

    dumpfile.close();

    mainLogger.info() << "Tokens dump file created.\n";
}