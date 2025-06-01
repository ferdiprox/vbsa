//
// Created by ferdinand on 5/25/25.
//

#include "parse-data.h"

bool isNumberChar(char c) {
    if (c == '-') {
        return true;
    }

    for (char nc : numbersDict) {
        if (nc == c) {
            return true;
        }
    }
    return false;
}

bool isNumberString(const std::string &str) {
    if (str[0] == '-') {
        return isNumberChar(str[1]);
    }
    return isNumberChar(str[0]);
}

std::optional<int> getOperatorIndex(char c) {
    for (int i = 0; i < OperatorType::Max; i++) {
        if (operatorsDict[i] == c) {
            return i;
        }
    }
    
    return std::nullopt;
}

