#include <algorithm>

#include "chung/token.hpp"

bool is_keyword(const std::string& identifier) {
    static const std::vector<std::string> keyword_identifiers{
        "def", "let", "__omg"
    };

    if (std::find(std::begin(keyword_identifiers), std::end(keyword_identifiers), identifier) != std::end(keyword_identifiers)) {
        return true;
    } else {
        return false;
    }
}