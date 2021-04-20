#include <catch2/catch.hpp>

#include <faasm/emulator.h>

#include "faabric/util/func.h"
#include "faasm/input.h"

namespace tests {
TEST_CASE("Test parse string to ints", "[input]")
{
    const char* input = "0 123 99987 5";
    int* actual = faasm::parseStringToIntArray(input, 4);

    REQUIRE(actual[0] == 0);
    REQUIRE(actual[1] == 123);
    REQUIRE(actual[2] == 99987);
    REQUIRE(actual[3] == 5);

    delete[] actual;
}

TEST_CASE("Test string input", "[input]") {
    faabric::Message msg = faabric::util::messageFactory("foo", "bar"); 
    
    std::string expected;
    std::string defaultVal = "default";
    SECTION("No input") {
        expected = defaultVal;
    }

    SECTION("Input") {
        expected = "foobar baz blah";
        msg.set_inputdata(expected);
    }
   
    setEmulatedMessage(msg);

    const char* actual = faasm::getStringInput(defaultVal.c_str());
    REQUIRE(actual == expected);
}
}
