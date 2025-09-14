#pragma once
#include <utility>

template <typename T>
class CInitSingleton {
    public:
    static T& GetSingleton() {
        return instance_;
    }

    protected:
    CInitSingleton() = default;
    ~CInitSingleton() = default;

    CInitSingleton(const CInitSingleton&) = delete;
    CInitSingleton& operator=(const CInitSingleton&) = delete;

    private:
    // This ensures CRT init (not lazy)
    static T instance_;
};

// definition
template <typename T>
T CInitSingleton<T>::instance_{};