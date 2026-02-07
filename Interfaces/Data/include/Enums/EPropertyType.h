#ifndef EPROPERTYTYPE_H
#define EPROPERTYTYPE_H

#include <cstdint>

enum class EPropertyType : uint32_t {
    Unknown,
    MotorMaxRpm,
    MotorTargetRpm,
    MotorMinDuty,
    MotorMaxDuty,
    MotorDisarmedDuty
};

#endif //EPROPERTYTYPE_H
