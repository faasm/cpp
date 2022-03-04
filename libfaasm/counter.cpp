#include "faasm/counter.h"
#include "faasm/core.h"

namespace faasm {
void writeIntState(const char* key, int val)
{
    auto ptr = BYTES(&val);
    faasmWriteState(key, ptr, sizeof(int));
}

int readIntState(const char* key)
{
    int val;
    auto buf = BYTES(&val);
    faasmReadState(key, buf, sizeof(int));
    return val;
}

void initCounter(const char* counterKey)
{
    int counterBuffer[] = { 0 };
    auto counterBytes = BYTES(counterBuffer);
    faasmWriteState(counterKey, counterBytes, sizeof(int));
}

int getCounter(const char* counterKey)
{
    faasmPullState(counterKey, sizeof(int));

    return readIntState(counterKey);
}

void incrementCounter(const char* counterKey)
{
    incrementCounter(counterKey, 1, true);
}

int incrementCounter(const char* counterKey, int increment, bool globalLock)
{
    if (globalLock) {
        faasmLockStateGlobal(counterKey);
    }

    faasmPullState(counterKey, sizeof(int));

    int val = readIntState(counterKey);
    val += increment;
    writeIntState(counterKey, val);

    faasmPushState(counterKey);

    if (globalLock) {
        faasmUnlockStateGlobal(counterKey);
    }

    return val;
}

AtomicInt::AtomicInt(const char* keyIn)
  : value(0)
  , key(keyIn)
{}

void AtomicInt::reset()
{
    initCounter(key.c_str());
}

int AtomicInt::operator+=(int other)
{
    value = incrementCounter(key.c_str(), other, true);
    return value;
}

int AtomicInt::get()
{
    value = getCounter(key.c_str());
    return value;
}
}
