#pragma once

template <typename T>
class CInitSingleton {

    public:
    static T& GetSingleton() noexcept {
        static_assert(std::is_default_constructible_v<T>);
        static_assert(!std::is_abstract_v<T>);
        static T instance;
        return instance;
    }

    protected:
    CInitSingleton() = default;
    ~CInitSingleton() = default;

    CInitSingleton(const CInitSingleton&) = delete;
    CInitSingleton& operator=(const CInitSingleton&) = delete;

};