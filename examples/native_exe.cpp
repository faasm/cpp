#include "native_lib.h"

int main() {
    bool success = native::callHostInterface();

    if (success) {
        return 0;
    } else {
        return 1;
    }
}
