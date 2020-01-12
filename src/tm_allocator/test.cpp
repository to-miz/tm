// #define TMAL_NO_STL
#define TM_ALLOCATOR_IMPLEMENTATION
#include "main.cpp"

#include "../common/tm_unreferenced_param.inc"

int main(int argc, char const *argv[])
{
    TM_MAYBE_UNUSED(argc);
    TM_MAYBE_UNUSED(argv);

    tml::StackAllocator test;
    auto ptr1 = tml::make_unique<int>(&test, 1);
    auto ptr2 = tml::make_unique<int[]>(&test, 5);
    return 0;
}
