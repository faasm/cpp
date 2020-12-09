#include "native_lib.h"

#include <string>
#include <vector>

namespace native {

bool callHostInterface() {
    std::string key("exe_check_state");
    std::vector<uint8_t> dummyState = {0, 1, 2, 3, 4};

    faasmWriteState(key.c_str(), dummyState.data(), dummyState.size());

    std::vector<uint8_t> actualState = {0, 0, 0, 0, 0};
    faasmReadState(key.c_str(), actualState.data(), actualState.size());

    return actualState == dummyState;
}

}  // namespace native
