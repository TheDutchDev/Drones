#ifndef PROPERTY_H
#define PROPERTY_H

#include "ITypedProperty.h"
#include "PropertyLock.h"

template<typename T>
class Property final : public ITypedProperty<T> {
public:
    void SetLock(PropertyLock lock) {
        LockConfig = lock;
    }

    T Get() const override {
        Lock();
        const T copy = Value;
        Unlock();
        return copy;
    }

    void Modify(const T &value) override {
        Lock();
        const bool changed = !IsEqual(Value, value);
        Value = value;
        Unlock();
        if (changed) {
            Modified.Emit(this);
        }
    }

    Event<IProperty *> &OnModified() override {
        return Modified;
    }

private:
    static bool IsEqual(const T &lhs, const T &rhs) {
        return lhs == rhs;
    }

    void Lock() const {
        if (LockConfig.Lock) {
            LockConfig.Lock(LockConfig.context);
        }
    }

    void Unlock() const {
        if (LockConfig.Unlock) {
            LockConfig.Unlock(LockConfig.context);
        }
    }

    T Value{};
    Event<IProperty *> Modified{};
    mutable PropertyLock LockConfig{};
};

#endif // PROPERTY_H
