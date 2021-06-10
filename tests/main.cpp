#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <faabric/util/logging.h>

struct LogListener : Catch::TestEventListenerBase
{
    using TestEventListenerBase::TestEventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        SPDLOG_INFO("---------------------------------------------");
        SPDLOG_INFO("TEST: {}", testInfo.name);
        SPDLOG_INFO("---------------------------------------------");
    }
};

int main(int argc, char* argv[])
{
    faabric::util::initLogging();

    int result = Catch::Session().run(argc, argv);

    fflush(stdout);

    return result;
}
