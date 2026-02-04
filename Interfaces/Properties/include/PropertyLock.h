#ifndef PROPERTYLOCK_H
#define PROPERTYLOCK_H

struct PropertyLock {
    void *context = nullptr;
    void (*Lock)(void *context) = nullptr;
    void (*Unlock)(void *context) = nullptr;
};

#endif // PROPERTYLOCK_H
