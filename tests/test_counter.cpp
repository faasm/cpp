#include "utils.h"
#include <catch2/catch.hpp>

#include <faabric/redis/Redis.h>
#include <faabric/state/State.h>
#include <faasm/emulator.h>

#include "faasm/counter.h"

using namespace faasm;

namespace tests {
TEST_CASE("Test normal counter operation", "[counter]")
{
    cleanCppTests();

    const char* key = "test_counter";
    initCounter(key);

    REQUIRE(getCounter(key) == 0);

    incrementCounter(key);
    REQUIRE(getCounter(key) == 1);

    incrementCounter(key);
    REQUIRE(getCounter(key) == 2);

    initCounter(key);
    REQUIRE(getCounter(key) == 0);

    incrementCounter(key);
    REQUIRE(getCounter(key) == 1);
}

TEST_CASE("Test counter over big number", "[counter]")
{
    cleanCppTests();

    const char* key = "test_counter";
    initCounter(key);

    for (int i = 0; i < 1000; i++) {
        incrementCounter(key);
    }

    REQUIRE(getCounter(key) == 1000);
}

TEST_CASE("Test uninitialised counter", "[counter]")
{
    cleanCppTests();

    const char* key = "test_uninit_key";
    initCounter(key);
    REQUIRE(getCounter(key) == 0);

    incrementCounter(key);
    REQUIRE(getCounter(key) == 1);
}
}
