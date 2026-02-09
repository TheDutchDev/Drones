#ifndef PWMOUTPUT_H
#define PWMOUTPUT_H

#include <cstddef>

using PwmSetDutyFn = void (*)(void *, float);

struct PwmOutput {
    void *Context = nullptr;
    PwmSetDutyFn SetDutyCallback = nullptr;

    void SetDuty(float duty) const {
        if (SetDutyCallback != nullptr) {
            SetDutyCallback(Context, duty);
        }
    }

    bool IsValid() const {
        return SetDutyCallback != nullptr;
    }
};

#endif // PWMOUTPUT_H
