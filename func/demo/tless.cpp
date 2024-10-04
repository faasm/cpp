#include <cstdint>
#include <iostream>
#include <string>

extern "C" {
void __tless_get_attestation_jwt(char** jwtPtrPtr, int32_t* jwtPtrSize);
}

int main()
{
    char* jwt;
    int32_t jwtSize;

    // JWT is heap-allocated
    __tless_get_attestation_jwt(&jwt, &jwtSize);

    std::string jwtStr(jwt);

    std::cout << "JWT: " << jwtStr << std::endl;

    return 0;
}
