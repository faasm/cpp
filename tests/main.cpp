#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <faabric/util/logging.h>

struct LogListener : Catch::TestEventListenerBase
{
    using TestEventListenerBase::TestEventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        SPDLOG_DEBUG("---------------------------------------------");
        SPDLOG_DEBUG("TEST: {}", testInfo.name);
        SPDLOG_DEBUG("---------------------------------------------");
    }
};

int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);

    fflush(stdout);

    return result;
}
