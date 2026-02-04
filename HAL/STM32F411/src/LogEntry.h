#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <cstddef>
#include <cstdint>

static constexpr size_t LogBufferSize = 256;

struct LogEntry {
    uint16_t len = 0;
    char buf[LogBufferSize] = {};
};

#endif // LOGENTRY_H
