#ifndef CALLBACK_H
#define CALLBACK_H

#include "ICallbackBase.h"
#include "EEventOwner.h"

template<typename T, typename ...Args>
class Callback : public ICallbackBase<Args...> {
public:
    T *Instance;
    void (T::*Method)(Args...);

    Callback(T *instance, void (T::*method)(Args...))
            : ICallbackBase<Args...>(EEventOwner::None), Instance(instance), Method(method) {}

    Callback(const EEventOwner &owner, T *instance, void (T::*method)(Args...))
            : ICallbackBase<Args...>(owner), Instance(instance), Method(method) {
    }

    void Invoke(Args... args) override {
        (Instance->*Method)(args...);
    }
};

#endif //CALLBACK_H
