#ifndef PWMOUTPUT_H
#define PWMOUTPUT_H

struct PwmOutput {
    void *context = nullptr;
    void (*SetDuty)(void *context, float duty) = nullptr;
};

#endif // PWMOUTPUT_H
