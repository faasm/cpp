#ifndef FAASM_COUNTER_H
#define FAASM_COUNTER_H

#include <string>

namespace faasm {
void initCounter(const char* counterKey);

int getCounter(const char* counterKey);

void incrementCounter(const char* counterKey);

int incrementCounter(const char* counterKey,
                     int increment,
                     bool globalLock = true);

class AtomicInt
{
  public:
    AtomicInt(const char* keyIn);

    int operator+=(int other);

    int get();

    void reset();

  private:
    int value;

    const std::string key;
};
}

#endif
