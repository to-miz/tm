#ifndef TM_PLACEMENT_NEW
    #include <new>
    #define TM_PLACEMENT_NEW(dest) ::new (static_cast<void*>(dest))
#endif