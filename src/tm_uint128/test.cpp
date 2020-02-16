#define TM_UINT128_IMPLEMENTATION
#define TMI_BACKEND_UINT64
#include "main.cpp"
#include "../common/tm_unreferenced_param.inc"

#include <stdio.h>
#include <string>

int main() {
    auto conv_and_print = [](const char* number) {
        tmi_uint128_t value = tmi_from_string(number, 16);
        auto str = tml::to_string(value, 16);
        printf("value = %s\n", str.c_str());
        str = tml::to_string(value, 10);
        printf("value = %s\n", str.c_str());
    };

    conv_and_print("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    conv_and_print("651651FFFFFFFFFFFFFFFFF");
    conv_and_print("FFFFFFFFFFFFFFFFF");

    printf("%f\n", tmi_to_float(tmi_from_string("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 16)));
    printf("%f\n", tmi_to_double(tmi_from_string("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 16)));

    return 0;
}