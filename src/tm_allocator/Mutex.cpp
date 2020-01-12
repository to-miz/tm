tml::LockGuard::LockGuard(const tml::RecursiveMutex& mut) : mutex(&mut) { mutex->lock_throws(); }
bool tml::LockGuard::lock(const tml::RecursiveMutex& mut) {
    TM_ASSERT(!this->mutex);
    this->mutex = &mut;
    return mut.lock();
}
tml::LockGuard::~LockGuard() {
    if (mutex) mutex->unlock();
}

tml::RecursiveMutex::RecursiveMutex(RecursiveMutex&& other) : internal(other.internal) { other.internal = nullptr; }
tml::RecursiveMutex& tml::RecursiveMutex::operator=(RecursiveMutex&& other) {
    if (this != &other) {
        auto temp = internal;
        internal = other.internal;
        other.internal = temp;
    }
    return *this;
}

#ifndef TMAL_NO_STL
    #include <mutex>
    tml::RecursiveMutex::RecursiveMutex() {
        internal = TM_MALLOC(sizeof(std::recursive_mutex), sizeof(void*));
        if (internal) {
            TM_PLACEMENT_NEW(internal) std::recursive_mutex();
        }
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        try {
            static_cast<std::recursive_mutex*>(internal)->lock();
            return true;
        } catch (std::exception&) {
            return false;
        }
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) throw std::bad_alloc();
        static_cast<std::recursive_mutex*>(internal)->lock();
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        try {
            return static_cast<std::recursive_mutex*>(internal)->try_lock();
        } catch (std::exception&) {
            return false;
        }
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        try {
            static_cast<std::recursive_mutex*>(internal)->unlock();
            return true;
        } catch (std::exception&) {
            return false;
        }
    }
    void tml::RecursiveMutex::destroy() {
        if (internal) {
            using std::recursive_mutex;
            static_cast<recursive_mutex*>(internal)->~recursive_mutex();
            TM_FREE(internal, sizeof(recursive_mutex), sizeof(void*));
            internal = nullptr;
        }
    }

#elif defined(_WIN32)
    tml::RecursiveMutex::RecursiveMutex() {
        CRITICAL_SECTION* cs = (CRITICAL_SECTION*)TM_MALLOC(sizeof(CRITICAL_SECTION), sizeof(void*));
        if (cs) InitializeCriticalSection(cs);
        internal = cs;
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        EnterCriticalSection((CRITICAL_SECTION*)internal);
        return true;
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) {
#ifndef TMAL_NO_STL
            throw std::bad_alloc();
#else
            throw nullptr;
#endif
        }
        EnterCriticalSection((CRITICAL_SECTION*)internal);
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        return TryEnterCriticalSection((CRITICAL_SECTION*)internal);
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        LeaveCriticalSection((CRITICAL_SECTION*)internal);
        return true;
    }
    void tml::RecursiveMutex::destroy() {
        if (internal) {
            DeleteCriticalSection((CRITICAL_SECTION*)internal);
            TM_FREE(internal, sizeof(CRITICAL_SECTION), sizeof(void*));
            internal = nullptr;
        }
    }

#else
    // Assume a platform with pthread
    #include <pthread.h>
    tml::RecursiveMutex::RecursiveMutex() {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_t* mutex = (pthread_mutex_t*)TM_MALLOC(sizeof(pthread_mutex_t), sizeof(void*));
        int result = pthread_mutex_init(mutex, &attr);
        if (result != 0) {
            TM_FREE(mutex, sizeof(pthread_mutex_t), sizeof(void*));
        }
        pthread_mutexattr_destroy(&attr);
        internal = mutex;
    }
    tml::RecursiveMutex::~RecursiveMutex() { destroy(); }
    bool tml::RecursiveMutex::lock() const {
        if (!internal) return false;
        return pthread_mutex_lock((pthread_mutex_t*)internal) == 0;
    }
    void tml::RecursiveMutex::lock_throws() const {
        if (!internal) {
#ifndef TMAL_NO_STL
            throw std::bad_alloc();
#else
            throw nullptr;
#endif
        }
        int result = pthread_mutex_lock((pthread_mutex_t*)internal);
        if (result != 0) {
#ifndef TMAL_NO_STL
            throw std::system_error(std::make_error_code((std::errc)result));
#else
            throw nullptr;
#endif
        }
    }
    bool tml::RecursiveMutex::try_lock() const {
        if (!internal) return false;
        return pthread_mutex_trylock((pthread_mutex_t*)internal) == 0;
    }
    bool tml::RecursiveMutex::unlock() const {
        if (!internal) return false;
        return pthread_mutex_unlock((pthread_mutex_t*)internal) == 0;
    }
    void tml::RecursiveMutex::destroy() {
        if (!internal) return;
        if (pthread_mutex_destroy((pthread_mutex_t*)internal) == 0) {
            TM_FREE(internal, sizeof(pthread_mutex_t), sizeof(void*));
            internal = nullptr;
            return true;
        }
        TM_ASSERT(0 && "Undefined behavior, mutex is still locked.");
    }
#endif