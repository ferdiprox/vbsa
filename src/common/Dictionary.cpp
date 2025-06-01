//
// Created by ferdinand on 5/21/25.
//

#include "Dictionary.h"

#include "DictParser.h"

void Dictionary::load(const std::string &filename) {
    DictParser parser(tokensTable);
    parser.parse(filename);
}

std::optional<unsigned int> Dictionary::getIndex(const std::string &token) const {
    for (unsigned int i = 0; i < tokensTable.size(); i++) {
        if (tokensTable[i] == token) {
            return i;
        }
    }
    return std::nullopt;
}

std::string Dictionary::getToken(const unsigned int index) const {
    if (index > tokensTable.size()) {
        throw std::out_of_range("Required token had out of dictionary table range index: " + std::to_string(index));
    }
    return tokensTable[index];
}
