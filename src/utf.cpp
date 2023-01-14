#include <algorithm>
#include <iterator>

#include "chung/utf.hpp"

// Goofy rn
std::string u32tostring(const std::u32string& s) {
    std::string out;
    std::transform(begin(s), end(s), back_inserter(out), [](char32_t c) {
        return c < 128 ? static_cast<char>(c) : '?';
    });
    return out;
}