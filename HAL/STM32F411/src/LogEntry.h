#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <cstddef>
#include <cstdint>

static constexpr size_t kLogBufferSize = 256;

struct LogEntry {
    uint16_t len = 0;
    char buf[kLogBufferSize] = {};
};

#endif // LOGENTRY_H
