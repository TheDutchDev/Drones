#ifndef ICALLBACKBASE_H
#define ICALLBACKBASE_H

#include "EEventOwner.h"

template<typename ...Args>
class ICallbackBase {
public:
    explicit ICallbackBase(const EEventOwner &owner) : Owner(owner) {}
    virtual ~ICallbackBase() = default;
    virtual void Invoke(Args... arg) = 0;
    EEventOwner Owner;
};

#endif //ICALLBACKBASE_H
