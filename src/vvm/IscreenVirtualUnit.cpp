//
// Created by ferdinand on 5/31/25.
//

#include "IscreenVirtualUnit.h"

#include "vvm/VangersVirtualMachine.h"

void IscreenVirtualUnit::openNewObject(const OftrObjType type) {
    objectsStack.push_back(type);
    currentObject = type;
}

void IscreenVirtualUnit::closeObject() {
    if (objectsStack.empty()) {
        error("Objects stack underflow: Probably misplaced '}' token.");
        return;
    }
    objectsStack.pop_back();
    currentObject = objectsStack.empty() ? I_ABSENT : objectsStack.back();
}

void IscreenVirtualUnit::assertCurrentObject(size_t compatibleTypes) const {
    if (compatibleTypes & BI_COMMAND) {
        if (currentObject == I_COMMAND) {
            return;
        }
        compatibleTypes &= ~BI_COMMAND;
    }

    size_t compareResult = compatibleTypes & currentObject;

    if (compareResult != compatibleTypes) {
        return;
    }

    error("Current instruction/property is not supported for the \"" + std::string(getObjTypeString(currentObject)) + "\" object.");
}

void IscreenVirtualUnit::error(const std::string &message) {
    mainVvm.error("IVU: " + message);
}
