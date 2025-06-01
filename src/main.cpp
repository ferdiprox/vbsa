
#include "core/Logger.h"
#include "core/arg-parse.h"

int main(int argc, char** argv)
{
    mainLogger.init();

    argparse::load(argc, argv);
    argparse::processArguments();
    argparse::processActions();

    return 0;
}
