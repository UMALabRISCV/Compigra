//===- mmul_blas_asm_config.cpp - ASM configuration utilities ----------===//
//
// Compigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "compigra/Support/mmul_blas_asm_config.h"
#include <algorithm>
#include <fstream>
#include <sstream>

void trim(std::string &s) {
  // remove leading spaces
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  // remove trailing spaces
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

void readTableFromFile(const std::string &filename,
                       std::vector<std::vector<std::string>> &table) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return;
  }

  table.clear();
  std::string line;
  while (std::getline(file, line)) {
    // Skip lines without '{'
    if (line.find('{') == std::string::npos)
      continue;

    // Detect start of a row: line containing '{'
    std::vector<std::string> row;
    std::string rowData = line;

    // Continue if the row spans multiple lines
    while (rowData.find('}') == std::string::npos && std::getline(file, line)) {
      rowData += line;
    }

    // Remove outer braces { }
    size_t start = rowData.find('{');
    size_t end = rowData.rfind('}');
    if (start != std::string::npos && end != std::string::npos && end > start) {
      rowData = rowData.substr(start + 1, end - start - 1);
    }

    // Split by commas, but only at top-level
    bool insideQuotes = false;
    std::string token;
    for (char c : rowData) {
      if (c == '"') {
        insideQuotes = !insideQuotes;
        token += c;
      } else if (c == ',' && !insideQuotes) {
        trim(token);
        if (!token.empty()) {
          // remove surrounding quotes and leading/trailing braces
          if (token.front() == '{')
            token.erase(0, 1);
          if (token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
          }
          row.push_back(token);
        }
        token.clear();
      } else {
        token += c;
      }
    }
    // Add last token
    trim(token);
    if (!token.empty()) {
      if (token.front() == '{')
        token.erase(0, 1);
      if (token.front() == '"' && token.back() == '"') {
        token = token.substr(1, token.size() - 2);
      }
      row.push_back(token);
    }

    if (!row.empty())
      table.push_back(row);
  }

  file.close();
}