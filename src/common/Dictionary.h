//
// Created by ferdinand on 5/21/25.
//

#pragma once

#include <vector>
#include <optional>
#include <string>

class Dictionary {
public:
    void load(const std::string& filename);

    std::optional<unsigned int> getIndex(const std::string& token) const;
    std::string getToken(const unsigned int index) const;

    bool empty() const {
        return tokensTable.empty();
    }
private:

    std::vector<std::string> tokensTable;
};

inline Dictionary mainDict;