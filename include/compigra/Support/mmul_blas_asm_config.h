#ifndef MMUL_BLAS_ASM_CONFIG_H
#define MMUL_BLAS_ASM_CONFIG_H

#include "mlir/IR/BuiltinAttributes.h"
#include <string>
#include <vector>

// Function declarations
void trim(std::string &s);
void readTableFromFile(const std::string &filename,
                       std::vector<std::vector<std::string>> &table);

struct ASMGenConfig {
  int cgraSize;
  int pcOffset;
  int insertionRow;
  int ctrl0Row;
  int ctrl1Row1;
  int ctrl1Row2;
  int ctrl0Col;
  int ctrl1Col1;
  int ctrl1Col2;
  bool hasCtrl1;
  // std::vector<std::vector<std::string>> asmTable;
};

// Configuration factory functions
inline ASMGenConfig getConfigForSize3() {
  return {3, 24, 32, 31, 39, 44, 7, 3, 2, false};
}

inline ASMGenConfig getConfigForSize4() {
  return {4, 11, 17, 16, 23, 26, 9, 4, 3, true};
}

struct ASMGenBLAS {
  int initPC = 0;
  ASMGenConfig config;
  std::string ctrl0;
  std::string ctrl1;

  ASMGenBLAS(int initPC, const ASMGenConfig &cfg)
      : initPC(initPC), config(cfg) {
    ctrl0 = "BNE R1, R2, " + std::to_string(initPC + config.pcOffset);
    if (config.hasCtrl1) {
      ctrl1 = "BNE R1, R2, " + std::to_string(initPC + config.pcOffset - 1);
    }
  }

  std::vector<std::vector<std::string>>
  generatePreCompileCode(std::string mulAsm = "", std::string addAsm = "",
                         mlir::ArrayAttr additionalAttrs = {}) const {
    std::vector<std::vector<std::string>> table;

    std::string filename = "../kernels/" + std::to_string(config.cgraSize) +
                           "x" + std::to_string(config.cgraSize) + ".txt";
    readTableFromFile(filename, table);

    int insertionRow = config.insertionRow;

    // Handle mulAsm insertion
    if (mulAsm != "") {
      std::vector<std::string> mulRow(config.cgraSize * config.cgraSize,
                                      mulAsm);
      table.insert(table.begin() + insertionRow, mulRow);
      insertionRow++;
    }

    // Handle addAsm insertion
    if (addAsm != "") {
      std::vector<std::string> addRow(config.cgraSize * config.cgraSize,
                                      addAsm);
      table.insert(table.begin() + insertionRow, addRow);
      insertionRow++;
    }

    // Handle additionalAttrs insertion
    if (additionalAttrs) {
      for (auto attr : additionalAttrs) {
        if (auto stringAttr = attr.dyn_cast<mlir::StringAttr>()) {
          std::vector<std::string> attrRow(config.cgraSize * config.cgraSize,
                                           stringAttr.getValue().str());
          table.insert(table.begin() + insertionRow, attrRow);
          insertionRow++;
        }
      }
    }

    // Calculate adjusted control row indices
    int numInserted = (mulAsm != "" ? 1 : 0) + (addAsm != "" ? 1 : 0);
    if (additionalAttrs) {
      for (auto attr : additionalAttrs) {
        if (attr.dyn_cast<mlir::StringAttr>())
          numInserted++;
      }
    }

    int ctrl1Row1 = config.ctrl1Row1 + numInserted;
    int ctrl1Row2 = config.ctrl1Row2 + numInserted;

    // Apply control signals
    if (config.cgraSize == 3) {
      table[config.ctrl0Row][config.ctrl0Col] = ctrl0;

      if (ctrl1Row1 < table.size()) {
        table[ctrl1Row1][config.ctrl1Col1] = ctrl0;
      }
      if (ctrl1Row2 < table.size()) {
        table[ctrl1Row2][config.ctrl1Col2] = ctrl0;
      }
    } else if (config.cgraSize == 4) {
      if (config.ctrl0Row < table.size()) {
        table[config.ctrl0Row][config.ctrl0Col] = ctrl0;
      }
      if (ctrl1Row1 < table.size()) {
        table[ctrl1Row1][config.ctrl1Col1] = ctrl1;
      }
      if (ctrl1Row2 < table.size()) {
        table[ctrl1Row2][config.ctrl1Col2] = ctrl1;
      }
    }

    return table;
  }
};

#endif // MMUL_BLAS_ASM_CONFIG_H
