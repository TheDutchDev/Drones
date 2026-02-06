#ifndef EDATATYPE_H
#define EDATATYPE_H

#include <cstdint>

enum class EDataType : int8_t {
    Unknown = -1,
    Float,
    String,
    UInt,
    Int,
    Bool,
};

#endif //EDATATYPE_H
