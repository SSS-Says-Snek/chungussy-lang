#include <fstream>
#include <sstream>

#include "chung/file.hpp"

std::u32string read_source(const std::string& file_path) {
    std::ifstream file{file_path};
    std::stringstream content_buffer;

    if (file.is_open()) {
        content_buffer << file.rdbuf();
    }

    return stringtou32(content_buffer.str());
}