#define TM_HASHMAPX_IMPLEMENTATION
#define TMH_X_NAME strmap
#define TMH_X_VALUE int
#define TMH_X_VALUE_IS_TRIVIAL
#define TMH_X_KEY_IS_STRING
#define TMH_X_IMPLEMENT
#include <tm_hashmapx.h>

#include <stdio.h>

int main() {
    strmap s = strmap_create();
    if (!s.capacity) {
        printf("Out of memory\n");
        return -1;
    }
    strmap_destroy(&s);
    return 0;
}
