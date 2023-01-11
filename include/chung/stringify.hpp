#pragma once

#include <iostream>

#include "chung/ast.hpp"

std::string stringify(const std::u32string& s);
std::string stringify(const Operator& op);
std::string stringify(const TokenType& op);