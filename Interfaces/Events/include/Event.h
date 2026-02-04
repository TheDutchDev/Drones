#ifndef EVENT_H
#define EVENT_H

#include <cstddef>
#include <cstring>

template<typename... Args>
class Event {
public:
    template<typename T>
    bool Subscribe(T *instance, void (T::*method)(Args...)) {
        if (!instance || !method) {
            return false;
        }
        static_assert(sizeof(method) <= MemberPointerSize, "Member pointer too large for Event");
        return AddSlot(instance, &InvokeMember<T>, &method, sizeof(method));
    }

    template<typename T>
    bool Subscribe(T *instance, void (T::*method)(Args...) const) {
        if (!instance || !method) {
            return false;
        }
        static_assert(sizeof(method) <= MemberPointerSize, "Member pointer too large for Event");
        return AddSlot(instance, &InvokeConstMember<T>, &method, sizeof(method));
    }

    template<typename T>
    void Unsubscribe(T *instance, void (T::*method)(Args...)) {
        if (!instance || !method) {
            return;
        }
        RemoveSlot(instance, &InvokeMember<T>, &method, sizeof(method));
    }

    template<typename T>
    void Unsubscribe(T *instance, void (T::*method)(Args...) const) {
        if (!instance || !method) {
            return;
        }
        RemoveSlot(instance, &InvokeConstMember<T>, &method, sizeof(method));
    }

    void Emit(Args... args) {
        for (size_t i = 0; i < MaxSubscribers; ++i) {
            if (_slots[i].invoke) {
                _slots[i].invoke(_slots[i].instance, _slots[i].method, args...);
            }
        }
    }

private:
    struct MemberPointerProbe {
        void Dummy() {
        }
    };

    using InvokeFn = void (*)(void *instance, const unsigned char *method, Args... args);

    struct Slot {
        void *instance = nullptr;
        InvokeFn invoke = nullptr;
        unsigned char method[sizeof(void (MemberPointerProbe::*)())] = {};
    };

    static constexpr size_t MaxSubscribers = 8;
    static constexpr size_t MemberPointerSize = sizeof(void (MemberPointerProbe::*)());

    template<typename T>
    static void InvokeMember(void *instance, const unsigned char *method, Args... args) {
        using Method = void (T::*)(Args...);
        Method m = nullptr;
        std::memcpy(&m, method, sizeof(Method));
        (static_cast<T *>(instance)->*m)(args...);
    }

    template<typename T>
    static void InvokeConstMember(void *instance, const unsigned char *method, Args... args) {
        using Method = void (T::*)(Args...) const;
        Method m = nullptr;
        std::memcpy(&m, method, sizeof(Method));
        (static_cast<T *>(instance)->*m)(args...);
    }

    bool AddSlot(void *instance, InvokeFn invoke, const void *method, size_t methodSize) {
        for (size_t i = 0; i < MaxSubscribers; ++i) {
            if (_slots[i].instance == instance &&
                _slots[i].invoke == invoke &&
                std::memcmp(_slots[i].method, method, methodSize) == 0) {
                return true;
            }
        }
        for (size_t i = 0; i < MaxSubscribers; ++i) {
            if (!_slots[i].invoke) {
                _slots[i].instance = instance;
                _slots[i].invoke = invoke;
                std::memset(_slots[i].method, 0, sizeof(_slots[i].method));
                std::memcpy(_slots[i].method, method, methodSize);
                return true;
            }
        }
        return false;
    }

    void RemoveSlot(void *instance, InvokeFn invoke, const void *method, size_t methodSize) {
        for (size_t i = 0; i < MaxSubscribers; ++i) {
            if (_slots[i].instance == instance &&
                _slots[i].invoke == invoke &&
                std::memcmp(_slots[i].method, method, methodSize) == 0) {
                _slots[i].instance = nullptr;
                _slots[i].invoke = nullptr;
                std::memset(_slots[i].method, 0, sizeof(_slots[i].method));
            }
        }
    }

    Slot _slots[MaxSubscribers]{};
};

#endif // EVENT_H
