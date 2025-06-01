//
// Created by ferdinand on 5/25/25.
//

#pragma once

#include <optional>
#include <string>

enum OperatorType {
    Divide,
    Multiply,
    Minus,
    Plus,

    // not actually an operators, just a sign to cut this field as expression too
    LeftBracket, RightBracket,

    Max
};

constexpr char numbersDict[10] {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
constexpr char operatorsDict[OperatorType::Max] {
    '/',
    '*',
    '-',
    '+',

    '(', ')'
};

std::optional<int> getOperatorIndex(char c);

bool isNumberChar(char c);
bool isNumberString(const std::string &str);
inline bool isOperatorChar(char c) {
    return getOperatorIndex(c).has_value();
}

