struct RecursiveMutex {
    RecursiveMutex();
    RecursiveMutex(RecursiveMutex&& other);
    RecursiveMutex& operator=(RecursiveMutex&& other);
    ~RecursiveMutex();

    bool lock() const;
    /*!
     * @brief Locks mutex, but throws on failure.
     * @throw
     *  If TMAL_NO_STL is defined:
     *      Throws nullptr_t on failure.
     *  Otherwise:
     *      Throws either std::bad_alloc if mutex wasn't initialized, or std::system_error if locking failed.
     */
    void lock_throws() const;
    bool try_lock() const;
    bool unlock() const;

    /*!
     * @brief Destroys the mutex. Destroying the mutex while it is locked is undefined behavior.
     */
    void destroy();

   private:
    void* internal;
};

struct LockGuard {
    LockGuard() = default;
    /*!
     * @brief Unlocks the mutex if locked.
     */
    ~LockGuard();

    /*!
     * @brief Tries to lock the mutex. Throws std::system_error or nullptr_t if TMAL_NO_STL is defined on failure.
     * @throw
     *  If TMAL_NO_STL is defined:
     *      Throws nullptr_t on failure.
     *  Otherwise:
     *      Throws either std::bad_alloc if mutex wasn't initialized, or std::system_error if locking failed.
     */
    explicit LockGuard(const RecursiveMutex& mutex);

    /*!
     * @brief Tries to lock the mutex.
     * @param mutex[IN] The mutex that will be locked.
     * @return Returns true if the lock operation succeeds, false otherwise.
     */
    bool lock(const RecursiveMutex& mutex);

    bool lock(const RecursiveMutex&& mutex) = delete;

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

   private:
    const RecursiveMutex* mutex = nullptr;
};