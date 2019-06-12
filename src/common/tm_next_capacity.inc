static tm_size_t MERGE_NEXT_CAPACITY(tm_size_t cap) { return 3 * ((cap + 2) / 2); }
#undef MERGE_NEXT_CAPACITY