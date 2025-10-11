#pragma once

namespace GTS {

    class TryLockMutex {

        public:

        explicit TryLockMutex(std::atomic_flag& flag) : m_flag(flag), m_acquired(!flag.test_and_set(std::memory_order_acquire)) {}

        ~TryLockMutex() {
            if (m_acquired) {
                m_flag.clear(std::memory_order_release);
            }
        }

        bool acquired() const {
            return m_acquired;
        }

        private:

        std::atomic_flag& m_flag;
        bool m_acquired;

    };

}