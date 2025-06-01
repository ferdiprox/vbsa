//
// Created by ferdinand on 5/31/25.
//

#pragma once

#include <vector>
#include <string>

#include "vvm/local-dictionaries.h"

class IscreenVirtualUnit {
public:
    std::vector<OftrObjType> objectsStack;
    OftrObjType currentObject = I_ABSENT;

    void openNewObject(OftrObjType type);
    void closeObject();

    void assertCurrentObject(size_t compatibleTypes) const;
private:

    static void error(const std::string& message);
};

inline IscreenVirtualUnit mainIvu;