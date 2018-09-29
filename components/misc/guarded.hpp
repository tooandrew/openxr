#ifndef OPENMW_COMPONENTS_MISC_GUARDED_H
#define OPENMW_COMPONENTS_MISC_GUARDED_H

#include <mutex>
#include <memory>

namespace Misc
{
    template <class T>
    class Locked
    {
        public:
            Locked(std::mutex& mutex, T& value)
                : mLock(mutex), mValue(value)
            {}

            T& get() const
            {
                return mValue.get();
            }

            T* operator ->() const
            {
                return std::addressof(get());
            }

            T& operator *() const
            {
                return get();
            }

        private:
            std::unique_lock<std::mutex> mLock;
            std::reference_wrapper<T> mValue;
    };

    template <class T>
    class ScopeGuarded
    {
        public:
            ScopeGuarded() = default;

            ScopeGuarded(const T& value)
                : mValue(value)
            {}

            ScopeGuarded(T&& value)
                : mValue(std::move(value))
            {}

            Locked<T> lock()
            {
                return Locked<T>(mMutex, mValue);
            }

            Locked<const T> lockConst()
            {
                return Locked<const T>(mMutex, mValue);
            }

        private:
            std::mutex mMutex;
            T mValue;
    };

    template <class T>
    class SharedGuarded
    {
        public:
            SharedGuarded()
                : mMutex(std::make_shared<std::mutex>())
            {}

            SharedGuarded(std::shared_ptr<T> value)
                : mMutex(std::make_shared<std::mutex>()), mValue(std::move(value))
            {}

            Locked<T> lock() const
            {
                return Locked<T>(*mMutex, *mValue);
            }

            Locked<const T> lockConst() const
            {
                return Locked<const T>(*mMutex, *mValue);
            }

        private:
            std::shared_ptr<std::mutex> mMutex;
            std::shared_ptr<T> mValue;
    };
}

#endif
