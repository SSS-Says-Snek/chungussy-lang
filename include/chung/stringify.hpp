#pragma once

#include <iostream>

#include "chung/ast.hpp"

std::string stringify(const Operator& op, bool verbose = false);
std::string stringify(const Symbol& op, bool verbose = false);
std::string stringify(const Keyword& op);

std::string stringify(const TokenType& op);
std::string stringify(const Token& token);