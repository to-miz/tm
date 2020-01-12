bool is_power_of_two(size_t value);
bool is_pointer_aligned(const void* ptr, size_t alignment);
bool is_pointer_aligned(uintptr_t ptr, size_t alignment);
/*!
 * @brief Returns the alignment offset to the given pointer.
 * @param ptr[in] Pointer to align.
 * @param alignment[in] Alignment to use.
 * @return Returns how much to adjust ptr to align it to alignment.
 * @example char* aligned_ptr = (char*)ptr + alignment_offset(ptr, alignof(int));
 */
size_t alignment_offset(const void* ptr, size_t alignment);
/*!
 * @brief Returns the alignment offset to the given pointer value.
 * @param ptr[in] Pointer to align.
 * @param alignment[in] Alignment to use.
 * @return Returns how much to adjust ptr to align it to alignment.
 */
size_t alignment_offset(uintptr_t ptr, size_t alignment);
/*!
 * @brief Helper to calculate the next capacity of an array in case of growth.
 * @return Basically returns current_capacity * 1.5 more or less.
 */
inline size_t next_capacity(size_t current_capacity) { return 3 * ((current_capacity + 2) / 2); }

/*!
 * @brief Helper wrapper around mmap/VirtualAlloc depending on platform.
 */
void* tmal_mmap(size_t size);
/*!
 * @brief Helper wrapper around munmmap/VirtualFree depending on platform.
 */
bool tmal_munmap(void* ptr, size_t size);
/*!
 * @brief Helper wrapper around sysconf(_SC_PAGESIZE)/GetSystemInfo(&info) depending on platform.
 */
size_t tmal_get_mmap_granularity();