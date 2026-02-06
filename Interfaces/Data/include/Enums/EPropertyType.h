#ifndef EPROPERTYTYPE_H
#define EPROPERTYTYPE_H

#include <cstdint>

enum class EPropertyType : uint32_t {
    Unknown,
    MaxRpm,
    TargetRpm
};

#endif //EPROPERTYTYPE_H
