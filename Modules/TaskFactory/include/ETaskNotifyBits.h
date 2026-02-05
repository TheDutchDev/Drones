#ifndef ETASKNOTIFYBITS_H
#define ETASKNOTIFYBITS_H

#include "Os.h"

enum ETaskNotifyBits : uint32_t {
    NOTIFY_FINISHED = (1u << 0),
    NOTIFY_CANCEL   = (1u << 1),
    // other user bits (2..31) are free for user notifications
};

#endif //ETASKNOTIFYBITS_H