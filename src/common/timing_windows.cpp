int64_t init_ticks_per_second() {
    LARGE_INTEGER i;
    if (!QueryPerformanceFrequency(&i)) return 1;
    if (i.QuadPart == 0) return 1;
    return i.QuadPart;
}
static const int64_t ticks_per_second = init_ticks_per_second();

int64_t get_ticks() {
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    return i.QuadPart;
}
double ticks_to_ms(int64_t t) { return t * (1000.0 / ticks_per_second); }