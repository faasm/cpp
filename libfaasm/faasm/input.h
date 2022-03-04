#ifndef FAASM_INPUT_H
#define FAASM_INPUT_H

#include "faasm/core.h"

namespace faasm {
const char* getStringInput(const char* defaultValue);

void setStringOutput(const char* val);

int getIntInput();

int* parseStringToIntArray(const char* inStr, int expected);
}

#endif
