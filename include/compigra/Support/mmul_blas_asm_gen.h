#ifndef MMUL_BLAS_ASM_GEN_H
#define MMUL_BLAS_ASM_GEN_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void trim(std::string &s);

void readTableFromFile(const std::string &filename,
                       std::vector<std::vector<std::string>> &table);

#endif // MMUL_BLAS_ASM_GEN_H
